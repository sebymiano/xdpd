#include <inttypes.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/common/datapacket.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/platform/packet.h>
#include <rofl/datapath/hal/openflow/openflow1x/of1x_cmm.h>
#include <rofl/datapath/pipeline/common/ipv6_exthdr.h>
#include <rofl/common/utils/c_logger.h>

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <vtss_api/vtss_api.h>
#include <vtss_api/vtss_packet_api.h>

extern "C" {

#include <fsl_utils/fsl_utils.h>

}

#include "../io/datapacketx86.h"
#include "../io/bufferpool.h"
#include "../io/packet_classifiers/pktclassifier.h"
#include "../config.h"
#include "../vtss_l2sw/ports.h"

//
// Configuration to include packet_proto_meta_imp.h
//

#define GET_CLAS_STATE_PTR(PKT)\
 	( &( ( (xdpd::gnu_linux::datapacketx86*) PKT ->platform_state) ->clas_state) )

#include "packet_proto_meta_imp.h"

//
// Driver specific functions
//

using namespace xdpd::gnu_linux;

/* Cloning of the packet */
void clone_pkt_contents(datapacket_t* src, datapacket_t* dst) {

	//Initialize buffer
	datapacketx86 *pack_src = (datapacketx86*) src->platform_state;
	datapacketx86 *pack_dst = (datapacketx86*) dst->platform_state;

	//Initialize the buffer and copy but do not classify
	pack_dst->init(pack_src->get_buffer(), pack_src->get_buffer_length(), pack_src->lsw, pack_src->clas_state.port_in,
			pack_src->clas_state.phy_port_in, false, true);

	//Copy output_queue
	pack_dst->output_queue = pack_src->output_queue;
	//Copy classification state
	pack_dst->clas_state.type = pack_src->clas_state.type;
	// do not overwrite clas_state.base and clas_state.len, as they are pointing to pack_src and were set already when calling pack_dst->init(...)
	pack_dst->clas_state.port_in = pack_src->clas_state.port_in;
	pack_dst->clas_state.phy_port_in = pack_src->clas_state.phy_port_in;
	pack_dst->clas_state.calculate_checksums_in_sw = pack_src->clas_state.calculate_checksums_in_sw;
}

void platform_packet_set_queue(datapacket_t* pkt, uint32_t queue) {
	datapacketx86 *pack = (datapacketx86*) pkt->platform_state;
	pack->output_queue = queue;
}

void platform_packet_drop(datapacket_t* pkt) {
	ROFL_DEBUG(DRIVER_NAME"[pkt] Dropping packet(%p)\n", pkt);

	//Release buffer
	bufferpool::release_buffer(pkt);

}

datapacket_t* platform_packet_replicate(datapacket_t* pkt) {

	//Get a free buffer
	datapacket_t* copy = bufferpool::get_buffer();

	if (!copy) {
		return copy;
		return NULL;
	}

	//Make sure everything is memseted to 0
	memcpy(&copy->write_actions, &pkt->write_actions, sizeof(pkt->write_actions));

	//mark as replica
	copy->is_replica = true;
	copy->sw = pkt->sw;

	//Clone contents
	clone_pkt_contents(pkt, copy);
	return copy;
}

static void output_single_packet(uint8_t* pack, int output_phy_port, size_t size) {
	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	if (is_valid_port(output_phy_port) && !is_internal_port(output_phy_port)) {

		ROFL_DEBUG("["DRIVER_NAME"] preparing to output packet into port %u, port is valid \n", output_phy_port);

		if (vtss_packet_tx_frame_port(NULL, output_phy_port, pack, size) != VTSS_RC_OK) {
			ROFL_DEBUG("["DRIVER_NAME"] writing to a physical port unsuccessful \n");
		} else {
			ROFL_DEBUG("["DRIVER_NAME"] writing to a physical port successful \n");
		}
	}
}

/**
 * Output packet to the port(s)
 * The action HAS to implement the destruction/release of the pkt
 * (including if the pkt is a replica).
 *
 * If a flooding output actions needs to be done, the function
 * has itself to deal with packet replication.
 */
void platform_packet_output(datapacket_t* pkt, switch_port_t* output_port) {
	of_switch_t const* sw;
	datapacketx86* pack;
	int output_phy_port = -1;
	vtss_l2sw_port_t* state;

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	if (unlikely(output_port == NULL)) {
		assert(0);
		return;
	}

	//Check whether dpx86 is NULL
	pack = (datapacketx86*) (pkt->platform_state);
	assert(pack != NULL);

	//flood_meta_port is a static variable defined in the physical_switch
	//the meta_port
	if (output_port == flood_meta_port || output_port == all_meta_port) {
		ROFL_DEBUG("["DRIVER_NAME"] packet.cc: we need to FLOOD\n");
		switch_port_t* port_it;

		//Get switch
		//sw = pkt->sw;
		sw = pack->lsw;

		if (unlikely(!sw)) {
			bufferpool::release_buffer(pkt);
			ROFL_DEBUG("["DRIVER_NAME"] packet.cc: sw is NULL");
			return;
		}

		ROFL_DEBUG("["DRIVER_NAME"] packet.cc: getting ports from lsi: %lu, name: %s, ports: %lu\n", sw->dpid, sw->name,
				sw->num_of_ports);
		for (unsigned i = 0; i < sw->num_of_ports; i++) {
			port_it = sw->logical_ports[i].port;

			if (!port_it) {
				ROFL_DEBUG("["DRIVER_NAME"] packet.cc: port is NULL");
			}

			if (sw->logical_ports[i].attachment_state == LOGICAL_PORT_STATE_ATTACHED) {
				ROFL_DEBUG("["DRIVER_NAME"] packet.cc: port state attached");
			} else {
				ROFL_DEBUG("["DRIVER_NAME"] packet.cc: port state not attached");
			}

			//Check port is not incoming port, exists, and is up
			if ((i == pack->clas_state.port_in) || !port_it || port_it->no_flood) {
				ROFL_DEBUG("["DRIVER_NAME"] packet.cc: skipping port, iteration: %u, port_in = %u\n", i,
						pack->clas_state.port_in);
				continue;
			}

			state = (vtss_l2sw_port_t*) port_it->platform_port_state;
			output_phy_port = state->vtss_l2sw_port_num;
			ROFL_DEBUG("["DRIVER_NAME"] packet.cc: preparing to output packet into port %u \n", output_phy_port);
			output_single_packet(pack->get_buffer(), output_phy_port, pack->get_buffer_length());

		}

		//We need to flood
		/*for (unsigned i = 0; i < LOGICAL_SWITCH_MAX_LOG_PORTS; ++i) {
		 port_it = sw->logical_ports[i].port;

		 //Check port is not incoming port, exists, and is up
		 if ((i == pack->clas_state.port_in) || !port_it || port_it->no_flood) {
		 ROFL_DEBUG("["DRIVER_NAME"] packet.cc: skipping port\n");
		 continue;
		 }

		 state = (vtss_l2sw_port_t*) port_it->platform_port_state;
		 output_phy_port = state->vtss_l2sw_port_num;
		 ROFL_DEBUG("["DRIVER_NAME"] packet.cc: preparing to output packet into port %u \n", output_phy_port);
		 output_single_packet(pack->get_buffer(), output_phy_port, pack->get_buffer_length());
		 }*/

		//discard the original packet always (has been replicated)
		ROFL_DEBUG("["DRIVER_NAME"] packet.cc: discard the original packet");
		bufferpool::release_buffer(pkt);
	} else if (output_port == in_port_meta_port) {
		ROFL_DEBUG("["DRIVER_NAME"] packet.cc: we need to output in the INPUT port\n");

		//In port
		switch_port_t* port;
		//sw = pkt->sw;
		sw = pack->lsw;

		if (unlikely(pack->clas_state.port_in >= LOGICAL_SWITCH_MAX_LOG_PORTS)) {
			assert(0);
			return;
		}

		port = sw->logical_ports[pack->clas_state.port_in].port;
		if (unlikely(port == NULL)) {
			assert(0);
			return;

		}

		state = (vtss_l2sw_port_t*) port->platform_port_state;
		output_phy_port = state->vtss_l2sw_port_num;
		ROFL_DEBUG("["DRIVER_NAME"] preparing to output packet into port %u \n", output_phy_port);
		output_single_packet(pack->get_buffer(), output_phy_port, pack->get_buffer_length());
	} else {
		ROFL_DEBUG("["DRIVER_NAME"] packet.cc: SINGLE output\n");
		//Single output
		state = (vtss_l2sw_port_t*) output_port->platform_port_state;
		output_phy_port = state->vtss_l2sw_port_num;
		ROFL_DEBUG("["DRIVER_NAME"] preparing to output packet into port %u \n", output_phy_port);
		output_single_packet(pack->get_buffer(), output_phy_port, pack->get_buffer_length());
	}

}

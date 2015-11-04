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

#include "../io/datapacketx86.h"
#include "../io/bufferpool.h"
#include "../io/packet_classifiers/pktclassifier.h"


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
void clone_pkt_contents(datapacket_t* src, datapacket_t* dst){

	//Initialize buffer
	datapacketx86 *pack_src = (datapacketx86*)src->platform_state;
	datapacketx86 *pack_dst = (datapacketx86*)dst->platform_state;
	
	//Initialize the buffer and copy but do not classify
	pack_dst->init(pack_src->get_buffer(), pack_src->get_buffer_length(), pack_src->lsw, pack_src->clas_state.port_in, pack_src->clas_state.phy_port_in, false, true);

	//Copy output_queue
	pack_dst->output_queue = pack_src->output_queue;
	//Copy classification state
	pack_dst->clas_state.type = pack_src->clas_state.type;
	// do not overwrite clas_state.base and clas_state.len, as they are pointing to pack_src and were set already when calling pack_dst->init(...)
	pack_dst->clas_state.port_in = pack_src->clas_state.port_in;
	pack_dst->clas_state.phy_port_in = pack_src->clas_state.phy_port_in;
	pack_dst->clas_state.calculate_checksums_in_sw = pack_src->clas_state.calculate_checksums_in_sw;
}

void platform_packet_set_queue(datapacket_t* pkt, uint32_t queue)
{
	datapacketx86 *pack = (datapacketx86*)pkt->platform_state;
	pack->output_queue = queue;	
}

void platform_packet_drop(datapacket_t* pkt)
{
	ROFL_DEBUG(DRIVER_NAME"[pkt] Dropping packet(%p)\n",pkt);
	
	//Release buffer
	bufferpool::release_buffer(pkt);

}


datapacket_t* platform_packet_replicate(datapacket_t* pkt){

	//Get a free buffer
	datapacket_t* copy = bufferpool::get_buffer();
	
	if(!copy){
		return copy;
		return NULL;
	}
	
	//Make sure everything is memseted to 0
	memcpy(&copy->write_actions, &pkt->write_actions ,sizeof(pkt->write_actions));

	//mark as replica
	copy->is_replica = true;
	copy->sw = pkt->sw;

	//Clone contents
	clone_pkt_contents(pkt,copy);
	return copy;	
}


/**
* Output packet to the port(s)
* The action HAS to implement the destruction/release of the pkt
* (including if the pkt is a replica).
*
* If a flooding output actions needs to be done, the function
* has itself to deal with packet replication.
*/
void platform_packet_output(datapacket_t* pkt, switch_port_t* output_port){


}

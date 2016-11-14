#include <assert.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/of1x_async_events_hooks.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.h>
#include <rofl/datapath/hal/openflow/openflow1x/of1x_cmm.h>
#include <rofl/common/utils/c_logger.h>
#include "../config.h"

#include "../io/bufferpool.h"
#include "../io/datapacketx86.h"
#include "../pipeline-imp/ls_internal_state.h"
#include "../util/l2switch_utils.h"
#include "../vtss_l2sw/vtss_l2sw.h"

using namespace xdpd::gnu_linux;

/*
 * Hooks for configuration of the switch
 */
rofl_result_t platform_post_init_of1x_switch(of1x_switch_t* sw) {

	ROFL_INFO("["DRIVER_NAME"] %s(): function start\n", __FUNCTION__);

	//Set OF1X flow table
	unsigned int i;
	of1x_flow_table_t* table;

	table = sw->pipeline.tables;

	for (i = 0; i < sw->pipeline.num_of_tables; ++i) {
		//Set appropriate flags (default: send packet to controller)
		table->default_action = OF1X_TABLE_MISS_CONTROLLER;
	}

	//Create GNU/Linux FWD_Module additional state (platform state)
	struct logical_switch_internals* ls_int = (struct logical_switch_internals*) calloc(1, sizeof(struct logical_switch_internals));

	ls_int->storage = new datapacket_storage( IO_PKT_IN_STORAGE_MAX_BUF, IO_PKT_IN_STORAGE_EXPIRATION_S); // TODO: make this value configurable

	sw->platform_state = (of_switch_platform_state_t*) ls_int;

	//Set number of buffers
	sw->pipeline.num_of_buffers = IO_PKT_IN_STORAGE_MAX_BUF;

	return ROFL_SUCCESS;

}

rofl_result_t platform_pre_destroy_of1x_switch(of1x_switch_t* sw) {

	ROFL_INFO("["DRIVER_NAME"] %s(): function start\n", __FUNCTION__);

	struct logical_switch_internals* ls_int = (struct logical_switch_internals*) sw->platform_state;

	delete ls_int->storage;
	free(sw->platform_state);

	return ROFL_SUCCESS;
}

//Async notifications

/*
 * Packet in
 */
void platform_of1x_packet_in(const of1x_switch_t* sw, uint8_t table_id, datapacket_t* pkt, uint16_t send_len,
		of_packet_in_reason_t reason) {
	ROFL_INFO("["DRIVER_NAME"] %s(): function start\n", __FUNCTION__);

	datapacketx86* pkt_x86;
	hal_result_t rv;
	storeid id;
	struct logical_switch_internals* ls_state = (struct logical_switch_internals*) sw->platform_state;
	packet_matches_t matches;

	if (!pkt)
		return;

	ROFL_DEBUG("["DRIVER_NAME"] %s(): Enqueuing PKT_IN event for packet(%p) in switch: %s\n", __FUNCTION__, pkt, sw->name);

	//Recover platform state
	pkt_x86 = (datapacketx86*) pkt->platform_state;

	//Store packet in the storage system. Packet is NOT returned to the bufferpool
	id = ls_state->storage->store_packet(pkt);

	if (id == datapacket_storage::ERROR) {
		ROFL_DEBUG(
				"["DRIVER_NAME"] %s(): PKT_IN for packet(%p) could not be stored in the storage. Dropping..\n", __FUNCTION__, pkt);

		//Return to the bufferpool
		bufferpool::release_buffer(pkt);
		return;
	}

	//Fill matches
	fill_packet_matches(pkt, &matches);

	//Process packet in
	rv = hal_cmm_process_of1x_packet_in(sw->dpid, table_id, reason, pkt_x86->clas_state.port_in, id, pkt->__cookie,
			pkt_x86->get_buffer(), send_len, pkt_x86->get_buffer_length(), &matches);

	if (rv == HAL_FAILURE) {
		ROFL_DEBUG(
				"["DRIVER_NAME"] %s():  PKT_IN for packet(%p) could not be sent to sw:%s controller. Dropping..\n",
				__FUNCTION__, pkt, sw->name);
		//Take packet out from the storage
		if (unlikely(ls_state->storage->get_packet(id) != pkt)) {
			ROFL_ERR(
					"["DRIVER_NAME"] %s(): Storage corruption. get_packet(%u) returned a different pkt pointer (should have been %p)\n",
					__FUNCTION__, id, pkt);

			assert(0);
		}

		//Return to datapacket_storage
		ls_state->storage->get_packet(id);

		//Return to the bufferpool
		bufferpool::release_buffer(pkt);
	}
}

//Flow removed
void platform_of1x_notify_flow_removed(const of1x_switch_t* sw, of1x_flow_remove_reason_t reason,
		of1x_flow_entry_t* removed_flow_entry) {

	ROFL_INFO("["DRIVER_NAME"] %s(): function start\n", __FUNCTION__);

	hal_cmm_process_of1x_flow_removed(sw->dpid, (uint8_t) reason, removed_flow_entry);

}

void plaftorm_of1x_add_entry_hook(of1x_flow_entry_t* new_entry) {
	ROFL_INFO("["DRIVER_NAME"] %s(): function start\n", __FUNCTION__);

	ROFL_INFO("["DRIVER_NAME"] calling %s(): checking if the entry is valid for the hardware offload\n", __FUNCTION__);

	if (!is_valid_entry(new_entry)) {
		ROFL_ERR("["DRIVER_NAME"] %s(): not a valid hardware entry\n", __FUNCTION__);
		return;
	}

	vtss_l2sw_add_flow_entry(new_entry);

	ROFL_INFO("["DRIVER_NAME"] calling %s(): Entry added to the hardware :)\n", __FUNCTION__);

}

void platform_of1x_modify_entry_hook(of1x_flow_entry_t* old_entry, of1x_flow_entry_t* mod, int reset_count) {
	ROFL_INFO("["DRIVER_NAME"] %s(): function start\n", __FUNCTION__);

	ROFL_INFO("["DRIVER_NAME"] calling %s(): checking if the entry is valid for the hardware offload\n", __FUNCTION__);

	if (!is_valid_entry(mod)) {
		ROFL_ERR("["DRIVER_NAME"] %s(): not a valid hardware entry\n", __FUNCTION__);
		return;
	}

	vtss_l2sw_delete_flow_entry(old_entry);
	vtss_l2sw_add_flow_entry(mod);

	ROFL_INFO("["DRIVER_NAME"] calling %s(): Entry modified to the hardware :)\n", __FUNCTION__);
}

void platform_of1x_remove_entry_hook(of1x_flow_entry_t* entry) {

	ROFL_INFO("["DRIVER_NAME"] %s(): function start\n", __FUNCTION__);

	ROFL_INFO("["DRIVER_NAME"] calling %s(): checking if the entry is valid for the hardware offload before remove it\n", __FUNCTION__);

	if (!is_valid_entry(entry)) {
		ROFL_ERR("["DRIVER_NAME"] %s(): not a valid hardware entry\n", __FUNCTION__);
		return;
	}

	vtss_l2sw_delete_flow_entry(entry);

	ROFL_INFO("["DRIVER_NAME"] calling %s(): Entry removed from the hardware :)\n", __FUNCTION__);
}

void platform_of1x_update_stats_hook(of1x_flow_entry_t* entry) {

}

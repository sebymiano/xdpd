#include <assert.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/of1x_async_events_hooks.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_statistics.h>
#include <rofl/datapath/hal/openflow/openflow1x/of1x_cmm.h>
#include <rofl/common/utils/c_logger.h>
#include "../config.h"

#include "../io/bufferpool.h"
#include "../io/datapacketx86.h"
#include "../pipeline-imp/ls_internal_state.h"

using namespace xdpd::gnu_linux;

/*
 * Hooks for configuration of the switch
 */
rofl_result_t platform_post_init_of1x_switch(of1x_switch_t* sw) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	//Set OF1X flow table
	unsigned int i;
	of1x_flow_table_t* table;

	table = sw->pipeline.tables;

	for (i = 0; i < sw->pipeline.num_of_tables; ++i) {
		//Set appropiate flags (default: send packet to controller)
		table->default_action = OF1X_TABLE_MISS_CONTROLLER;
	}

	//Create GNU/Linux FWD_Module additional state (platform state)
	struct logical_switch_internals* ls_int =
			(struct logical_switch_internals*) calloc(1,
					sizeof(struct logical_switch_internals));

	ls_int->storage = new datapacket_storage( IO_PKT_IN_STORAGE_MAX_BUF,
	IO_PKT_IN_STORAGE_EXPIRATION_S); // TODO: make this value configurable

	sw->platform_state = (of_switch_platform_state_t*) ls_int;

	//Set number of buffers
	sw->pipeline.num_of_buffers = IO_PKT_IN_STORAGE_MAX_BUF;

	return ROFL_SUCCESS;

}

rofl_result_t platform_pre_destroy_of1x_switch(of1x_switch_t* sw) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	struct logical_switch_internals* ls_int = (struct logical_switch_internals*) sw->platform_state;

	delete ls_int->storage;
	free(sw->platform_state);

	return ROFL_SUCCESS;
}

//Async notifications

/*
 * Packet in
 */
void platform_of1x_packet_in(const of1x_switch_t* sw, uint8_t table_id,
		datapacket_t* pkt, uint16_t send_len, of_packet_in_reason_t reason) {
	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);
}

//Flow removed
void platform_of1x_notify_flow_removed(const of1x_switch_t* sw,
		of1x_flow_remove_reason_t reason,
		of1x_flow_entry_t* removed_flow_entry) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

}

void plaftorm_of1x_add_entry_hook(of1x_flow_entry_t* new_entry) {

}

void platform_of1x_modify_entry_hook(of1x_flow_entry_t* old_entry,
		of1x_flow_entry_t* mod, int reset_count) {

}

void platform_of1x_remove_entry_hook(of1x_flow_entry_t* entry) {

}

void platform_of1x_update_stats_hook(of1x_flow_entry_t* entry) {

}

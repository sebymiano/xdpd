#include <rofl/datapath/hal/openflow/openflow1x/of1x_driver.h>
#include <rofl/common/utils/c_logger.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/openflow/of_switch_pp.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_statistics.h>
#include "../../../config.h"
#include "../../../vtss_l2sw/ports.h"
#include "../../../util/l2switch_utils.h"
#include <vtss_api/vtss_api.h>
extern "C" {

#include <fsl_utils/fsl_utils.h>

}

#include "../../../io/bufferpool.h"
#include "../../../io/datapacket_storage.h"
#include "../../../io/datapacketx86.h"
#include "../../../pipeline-imp/ls_internal_state.h"

using namespace xdpd::gnu_linux;

//Port config

/**
 * @name    hal_driver_of1x_set_port_drop_received_config
 * @brief   Instructs driver to modify port config state 
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param drop_received		Drop packets received
 */
hal_result_t hal_driver_of1x_set_port_drop_received_config(uint64_t dpid, unsigned int port_num, bool drop_received) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return HAL_FAILURE;
}

/**
 * @name    hal_driver_of1x_set_port_no_flood_config
 * @brief   Instructs driver to modify port config state 
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param no_flood		No flood allowed in port
 */
hal_result_t hal_driver_of1x_set_port_no_flood_config(uint64_t dpid, unsigned int port_num, bool no_flood) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return HAL_FAILURE;
}

/**
 * @name    hal_driver_of1x_set_port_forward_config
 * @brief   Instructs driver to modify port config state 
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param forward		Forward packets
 */
hal_result_t hal_driver_of1x_set_port_forward_config(uint64_t dpid, unsigned int port_num, bool forward) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	switch_port_t* port = physical_switch_get_port_by_num(dpid, port_num);
	vtss_l2sw_port* vtss_port_instance;

	if (!port)
		return HAL_FAILURE;

	vtss_port_instance = (vtss_l2sw_port*) port->platform_port_state;
	int port_no = vtss_port_instance->vtss_l2sw_port_num;

	//Attempt to update the forwarding state of the port
	if (is_valid_port(port_no) && !is_internal_port(port_no)) {
		if (forward) {
			if (vtss_port_state_set(NULL, port_no, TRUE) != VTSS_RC_OK) {
				ROFL_ERR("["DRIVER_NAME"] error calling vtss_port_state_set in function %s()\n", __FUNCTION__);
				return HAL_FAILURE;
			}

			port->forward_packets = true;
		} else {
			if (vtss_port_state_set(NULL, port_no, FALSE) != VTSS_RC_OK) {
				ROFL_ERR("["DRIVER_NAME"] error calling vtss_port_state_set in function %s()\n", __FUNCTION__);
				return HAL_FAILURE;
			}

			port->forward_packets = false;
		}
	}

	return HAL_SUCCESS;
}
/**
 * @name    hal_driver_of1x_set_port_generate_packet_in_config
 * @brief   Instructs driver to modify port config state 
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param generate_packet_in	Generate packet in events for this port 
 */
hal_result_t hal_driver_of1x_set_port_generate_packet_in_config(uint64_t dpid, unsigned int port_num,
bool generate_packet_in) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return HAL_SUCCESS;
}

/**
 * @name    hal_driver_of1x_set_port_advertise_config
 * @brief   Instructs driver to modify port advertise flags 
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 			Datapath ID of the switch 
 * @param port_num		Port number 	
 * @param advertise		Bitmap advertised
 */
hal_result_t hal_driver_of1x_set_port_advertise_config(uint64_t dpid, unsigned int port_num, uint32_t advertise) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return HAL_FAILURE;
}

/**
 * @name    hal_driver_of1x_set_pipeline_config
 * @brief   Instructs driver to process a PACKET_OUT event
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch 
 * @param flags		Capabilities bitmap (OF12_CAP_FLOW_STATS, OF12_CAP_TABLE_STATS, ...)
 * @param miss_send_len	OF MISS_SEND_LEN
 */
hal_result_t hal_driver_of1x_set_pipeline_config(uint64_t dpid, unsigned int flags, uint16_t miss_send_len) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	of_switch_t* lsw;

	//Recover switch
	lsw = physical_switch_get_logical_switch_by_dpid(dpid);

	//Check switch and port
	if (!lsw) {
		//TODO: log this... should never happen
		assert(0);
		return HAL_FAILURE;
	}

	//Simply store the new config
	((of1x_switch_t*) lsw)->pipeline.capabilities = flags;
	((of1x_switch_t*) lsw)->pipeline.miss_send_len = miss_send_len;

	return HAL_SUCCESS;
}

/**
 * @name    hal_driver_of1x_set_table_config
 * @brief   Instructs driver to set table configuration(default action)
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch
 * @param table_id	Table ID or 0xFF for all 
 * @param miss_send_len Table miss config	
 */
hal_result_t hal_driver_of1x_set_table_config(uint64_t dpid, unsigned int table_id,
		of1x_flow_table_miss_config_t config) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	of1x_switch_t* lsw;
	unsigned int i;

	//Recover switch
	lsw = (of1x_switch_t*) physical_switch_get_logical_switch_by_dpid(dpid);

	//Check switch and port
	if (!lsw || ((table_id != OF1X_FLOW_TABLE_ALL) && (table_id >= lsw->pipeline.num_of_tables))) {
		//TODO: log this... should never happen
		assert(0);
		return HAL_FAILURE;
	}

	//Simply store the new config
	if (table_id == OF1X_FLOW_TABLE_ALL) {
		for (i = 0; i < lsw->pipeline.num_of_tables; i++) {
			lsw->pipeline.tables[i].default_action = config;
		}
	} else {
		lsw->pipeline.tables[table_id].default_action = config;
	}

	//TODO: Here I should modify the default rule in the switch

	return HAL_SUCCESS;
}

/**
 * @name    hal_driver_of1x_process_packet_out
 * @brief   Instructs driver to process a PACKET_OUT event
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to process PACKET_OUT
 * @param buffer_id	Buffer ID
 * @param in_port 	Port IN
 * @param action_group 	Action group to apply
 * @param buffer		Pointer to the buffer
 * @param buffer_size	Buffer size
 */
hal_result_t hal_driver_of1x_process_packet_out(uint64_t dpid, uint32_t buffer_id, uint32_t in_port,
		of1x_action_group_t* action_group, uint8_t* buffer, uint32_t buffer_size) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return HAL_SUCCESS;
}

/**
 * @name    hal_driver_of1x_process_flow_mod
 * @brief   Instructs driver to process a FLOW_MOD event
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id to install the flowmod
 * @param flow_entry	Flow entry to be installed
 * @param buffer_id	Buffer ID
 * @param out_port 	Port to output
 * @param check_overlap	Check OVERLAP flag
 * @param check_counts	Check RESET_COUNTS flag
 */

hal_fm_result_t hal_driver_of1x_process_flow_mod_add(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t** flow_entry,
		uint32_t buffer_id,
		bool check_overlap, bool reset_counts) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	of1x_switch_t* lsw;
	rofl_of1x_fm_result_t result;

	//Recover port
	lsw = (of1x_switch_t*) physical_switch_get_logical_switch_by_dpid(dpid);

	if (!lsw) {
		assert(0);
		return HAL_FM_FAILURE;
	}

	if (table_id >= lsw->pipeline.num_of_tables) {
		ROFL_INFO("["DRIVER_NAME"] calling %s()\n: invalid table id %u\n", __FUNCTION__, table_id);
		return HAL_FM_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: checking if entry is only a l2 entry\n", __FUNCTION__);

	if (!is_l2_entry(*flow_entry)) {
		ROFL_INFO("["DRIVER_NAME"] hal_driver_of1x_process_flow_mod_add: This driver accepts only l2 entries\n");
		return HAL_FM_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: entry is only l2, adding entry into LSI table...\n", __FUNCTION__);

	if ((result = of1x_add_flow_entry_table(&lsw->pipeline, table_id, flow_entry, check_overlap, reset_counts))
			!= ROFL_OF1X_FM_SUCCESS)
		return hal_fm_map_pipeline_retcode(result);

	if (buffer_id && buffer_id != OF1XP_NO_BUFFER) {

		datapacket_t* pkt = ((struct logical_switch_internals*) lsw->platform_state)->storage->get_packet(buffer_id);

		if (!pkt) {
			//Return failure (buffer ID was invalid/expired)
			return HAL_FM_FAILURE;
		}

		of_process_packet_pipeline(ROFL_PIPELINE_LOCKED_TID, (of_switch_t*) lsw, pkt);
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: entry added\n", __FUNCTION__);

#ifdef DEBUG
	of1x_dump_table(&lsw->pipeline.tables[table_id], false);
#endif

	return HAL_FM_SUCCESS;
}

/**
 * @name    hal_driver_of1x_process_flow_mod_modify
 * @brief   Instructs driver to process a FLOW_MOD modify event
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id from which to modify the flowmod
 * @param flow_entry	Flow entry 
 * @param buffer_id	Buffer ID
 * @param strictness 	Strictness (STRICT NON-STRICT)
 * @param check_counts	Check RESET_COUNTS flag
 */
hal_fm_result_t hal_driver_of1x_process_flow_mod_modify(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t** flow_entry,
		uint32_t buffer_id, of1x_flow_removal_strictness_t strictness, bool reset_counts) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	of1x_switch_t* lsw;
	rofl_of1x_fm_result_t result;

	//Recover port
	lsw = (of1x_switch_t*) physical_switch_get_logical_switch_by_dpid(dpid);

	if (!lsw) {
		assert(0);
		return HAL_FM_FAILURE;
	}

	if (table_id >= lsw->pipeline.num_of_tables) {
		return HAL_FM_INVALID_TABLE_ID_FAILURE;
		ROFL_INFO("["DRIVER_NAME"] calling %s()\n: invalid table id %u\n", __FUNCTION__, table_id);
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: checking if entry is only a l2 entry\n", __FUNCTION__);

	if (!is_l2_entry(*flow_entry)) {
		ROFL_INFO("["DRIVER_NAME"] hal_driver_of1x_process_flow_mod_modify: This driver accepts only l2 entries\n");
		return HAL_FM_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: entry is only l2, modifying entry into LSI table...\n", __FUNCTION__);

	if ((result = of1x_modify_flow_entry_table(&lsw->pipeline, table_id, flow_entry, strictness, reset_counts))
			!= ROFL_OF1X_FM_SUCCESS)
		return hal_fm_map_pipeline_retcode(result);

	if (buffer_id && buffer_id != OF1XP_NO_BUFFER) {

		datapacket_t* pkt = ((struct logical_switch_internals*) lsw->platform_state)->storage->get_packet(buffer_id);

		if (!pkt) {
			//Return failure (buffer ID was invalid/expired)
			return HAL_FM_FAILURE;
		}

		of_process_packet_pipeline(ROFL_PIPELINE_LOCKED_TID, (of_switch_t*) lsw, pkt);
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: entry added\n", __FUNCTION__);

	return HAL_FM_SUCCESS;
}

/**
 * @name    hal_driver_of1x_process_flow_mod_delete
 * @brief   Instructs driver to process a FLOW_MOD event
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id to install the flowmod
 * @param flow_entry	Flow entry to be installed
 * @param out_port 	Out port that entry must include
 * @param out_group 	Out group that entry must include	
 * @param strictness 	Strictness (STRICT NON-STRICT)
 */
hal_fm_result_t hal_driver_of1x_process_flow_mod_delete(uint64_t dpid, uint8_t table_id, of1x_flow_entry_t* flow_entry,
		uint32_t out_port, uint32_t out_group, of1x_flow_removal_strictness_t strictness) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	of1x_switch_t* lsw;
	unsigned int i;
	rofl_of1x_fm_result_t result;

	//Recover port
	lsw = (of1x_switch_t*) physical_switch_get_logical_switch_by_dpid(dpid);

	if (!lsw) {
		assert(0);
		return HAL_FM_FAILURE;
	}

	if (table_id >= lsw->pipeline.num_of_tables && table_id != OF1X_FLOW_TABLE_ALL) {
		return HAL_FM_INVALID_TABLE_ID_FAILURE;
		ROFL_INFO("["DRIVER_NAME"] calling %s()\n: invalid table id %u", __FUNCTION__, table_id);
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: checking if entry is only a l2 entry\n", __FUNCTION__);

	if (!is_l2_entry(flow_entry)) {
		ROFL_INFO("["DRIVER_NAME"] hal_driver_of1x_process_flow_mod_delete: This driver accepts only l2 entries\n");
		return HAL_FM_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: entry is only l2, remove entry from table...\n", __FUNCTION__);

	if (table_id == OF1X_FLOW_TABLE_ALL) {
		for (i = 0; i < lsw->pipeline.num_of_tables; i++) {
			if ((result = of1x_remove_flow_entry_table(&lsw->pipeline, i, flow_entry, strictness, out_port, out_group))
					!= ROFL_OF1X_FM_SUCCESS)
				return hal_fm_map_pipeline_retcode(result);
		}
	} else {
		//Single table
		if ((result = of1x_remove_flow_entry_table(&lsw->pipeline, table_id, flow_entry, strictness, out_port,
				out_group)) != ROFL_OF1X_FM_SUCCESS)
			return hal_fm_map_pipeline_retcode(result);
	}

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n: entry removed\n", __FUNCTION__);

	return HAL_FM_SUCCESS;
}

//
// Statistics
//

/**
 * @name    hal_driver_of1x_get_flow_stats
 * @brief   Recovers the flow stats given a set of matches 
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id to get the flows of 
 * @param cookie	Cookie to be applied 
 * @param cookie_mask	Mask for the cookie
 * @param out_port 	Out port that entry must include
 * @param out_group 	Out group that entry must include	
 * @param matches	Matches
 */
of1x_stats_flow_msg_t* hal_driver_of1x_get_flow_stats(uint64_t dpid, uint8_t table_id, uint32_t cookie,
		uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, of1x_match_group_t* matches) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	of1x_switch_t* lsw;

	//Recover port
	lsw = (of1x_switch_t*) physical_switch_get_logical_switch_by_dpid(dpid);

	if (!lsw) {
		assert(0);
		return NULL;
	}

	if (table_id >= lsw->pipeline.num_of_tables && table_id != OF1X_FLOW_TABLE_ALL)
		return NULL;

	return of1x_get_flow_stats(&lsw->pipeline, table_id, cookie, cookie_mask, out_port, out_group, matches);
}

/**
 * @name    driver_of1x_get_flow_aggregate_stats
 * @brief   Recovers the aggregated flow stats given a set of matches 
 * @ingroup of1x_hal_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the FLOW_MOD
 * @param table_id 	Table id to get the flows of 
 * @param cookie	Cookie to be applied 
 * @param cookie_mask	Mask for the cookie
 * @param out_port 	Out port that entry must include
 * @param out_group 	Out group that entry must include	
 * @param matches	Matches
 */
of1x_stats_flow_aggregate_msg_t* hal_driver_of1x_get_flow_aggregate_stats(uint64_t dpid, uint8_t table_id,
		uint32_t cookie, uint32_t cookie_mask, uint32_t out_port, uint32_t out_group, of1x_match_group_t* matches) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	of1x_switch_t* lsw;

	//Recover port
	lsw = (of1x_switch_t*) physical_switch_get_logical_switch_by_dpid(dpid);

	if (!lsw) {
		assert(0);
		return NULL;
	}

	if (table_id >= lsw->pipeline.num_of_tables && table_id != OF1X_FLOW_TABLE_ALL)
		return NULL;

	return of1x_get_flow_aggregate_stats(&lsw->pipeline, table_id, cookie, cookie_mask, out_port, out_group, matches);
}
/**
 * @name    hal_driver_of1x_group_mod_add
 * @brief   Instructs driver to add a new GROUP
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the GROUP
 */
hal_gm_result_t hal_driver_of1x_group_mod_add(uint64_t dpid, of1x_group_type_t type, uint32_t id,
		of1x_bucket_list_t **buckets) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return HAL_GM_FAILURE;
}

/**
 * @name    hal_driver_of1x_group_mod_modify
 * @brief   Instructs driver to modify the GROUP with identification ID
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the GROUP
 */
hal_gm_result_t hal_driver_of1x_group_mod_modify(uint64_t dpid, of1x_group_type_t type, uint32_t id,
		of1x_bucket_list_t **buckets) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return HAL_GM_FAILURE;
}

/**
 * @name    hal_driver_of1x_group_mod_del
 * @brief   Instructs driver to delete the GROUP with identification ID
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch to install the GROUP
 */
hal_gm_result_t hal_driver_of1x_group_mod_delete(uint64_t dpid, uint32_t id) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return HAL_GM_FAILURE;
}

/**
 * @ingroup core_of1x
 * Retrieves a copy of the group and bucket structure
 * @return of1x_stats_group_desc_msg_t instance that must be destroyed using of1x_destroy_group_desc_stats()
 */
of1x_stats_group_desc_msg_t *hal_driver_of1x_get_group_desc_stats(uint64_t dpid) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return NULL;
}

/**
 * @name    hal_driver_of1x_get_group_stats
 * @brief   Instructs driver to fetch the GROUP statistics
 * @ingroup of1x_driver_async_event_processing
 *
 * @param dpid 		Datapath ID of the switch where the GROUP is
 */
of1x_stats_group_msg_t * hal_driver_of1x_get_group_stats(uint64_t dpid, uint32_t id) {

	ROFL_INFO("["DRIVER_NAME"] calling %s()\n", __FUNCTION__);

	return NULL;
}

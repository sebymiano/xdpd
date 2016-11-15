#include "stats.h"
#include "../vtss_l2sw/vtss_l2sw.h"
#include <rofl/common/utils/c_logger.h>
#include "flow_entry.h"
#include "../config.h"

void dump_l2sw_flow_acl_entries() {
    uint32_t i;
    vtss_ace_counter_t counter;

	for(i = 0; i < FSCALE_L2SW_MAX_ACL_ID; i++){
		if(vtss_ace_counter_get(NULL, i, &counter) != VTSS_RC_OK){
			ROFL_INFO("["DRIVER_NAME"] calling %s(): Unable to read the counter for ACL ID %u\n", __FUNCTION__, i);
			return;
		}

		if(counter > 0){
			ROFL_INFO("["DRIVER_NAME"] calling %s(): The counter's value for ACL ID %d is: %u\n", __FUNCTION__, i, counter);
		}
	}
}

rofl_result_t vtss_l2sw_update_entry_stats(of1x_flow_entry_t* entry){
	vtss_l2sw_flow_entry_t* vtss_entry;
	vtss_ace_counter_t counter;

	if (!is_valid_entry(entry)) {
		ROFL_ERR("["DRIVER_NAME"] %s(): not a hardware entry\n", __FUNCTION__);
		return ROFL_FAILURE;
	}

	vtss_entry = (vtss_l2sw_flow_entry_t*) entry->platform_state;

	if(!vtss_entry){
		ROFL_ERR("["DRIVER_NAME"] %s(): not a valid hardware entry\n", __FUNCTION__);
		return ROFL_FAILURE;
	}

	if(vtss_entry->type == VTSS_ENTRY_TYPE_ACL) {
		if(vtss_ace_counter_get(NULL, vtss_entry->acl_id, &counter) != VTSS_RC_OK){
			ROFL_INFO("["DRIVER_NAME"] calling %s(): Unable to read the counter for ACL ID %u\n", __FUNCTION__, vtss_entry->acl_id);
			return ROFL_FAILURE;
		}

		entry->stats.s.counters.packet_count = counter;
	}

	return ROFL_SUCCESS;
}

#include "flow_entry.h"

#include <rofl/common/utils/c_logger.h>
#include <fsl_utils/fsl_utils.h>

static int acl_id = 1;

vtss_rc vtss_l2sw_generate_acl_entry_matches(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry) {

	of1x_match_t* match;
	of1x_match_type_t type = OF1X_MATCH_MAX;
	int num_of_matches = 0;

	ROFL_DEBUG("%s  %d num_of_matches: %x", __FILE__, __LINE__, of1x_entry->matches.num_elements);

	if (acl_id == VTSS_ACE_ID_LAST) {
		ROFL_ERR("flow_entry.c: reached maximum number of acl rules");
		return VTSS_RC_ERROR;
	}

	//Go through all the matches and set entry matches
	for (match = of1x_entry->matches.head; match; match = match->next) {
		ROFL_DEBUG("%s  %d  of1x_entry->type : %x, ", __FILE__, __LINE__, match->type);

		switch (match->type) {
		case OF1X_MATCH_IN_PORT:
			uint8_t port_no = of1x_get_match_value32(match);

			if (!add_or_update_match_in_port(acl_entry, type, port_no)) {
				ROFL_ERROR("vtss_l2sw_generate_acl_entry: add_or_update_match_in_port failed", port);
				return VTSS_RC_ERROR;
			}

			break;
		case OF1X_MATCH_IN_PHY_PORT:
			uint8_t port_no = of1x_get_match_value32(match);

			if (!add_or_update_match_in_port(acl_entry, type, port_no)) {
				ROFL_ERROR("vtss_l2sw_generate_acl_entry: add_or_update_match_in_port failed", port);
				return VTSS_RC_ERROR;
			}

			break;
		case OF1X_MATCH_ETH_DST:
			break;
		case OF1X_MATCH_ETH_SRC:
			break;
		default:
			match = OF1X_MATCH_MAX;
			break;
		}

		type = match->type;
	}

	return VTSS_RC_ERROR;
}

vtss_rc vtss_l2sw_generate_acl_entry_actions(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry){
	//TODO: Add implementation
	return VTSS_RC_ERROR;
}

static bool add_or_update_match_in_port(vtss_ace_t* acl_entry, of1x_match_type_t type, int port_no) {
	if (is_valid_port(port_no) && !is_internal_port(port_no)) {
		if (type == OF1X_MATCH_MAX) {
			//This is the first match
			if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ANY, acl_entry) != VTSS_RC_OK) {
				ROFL_ERROR("vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_IN_PORT", port);
				return VTSS_RC_ERROR;
			}
			//Forward to CPU
			acl_entry->action.cpu = false;
			//Only first frame forwarded to CPU
			acl_entry->action.cpu_once = false;
			//CPU queue
			//acl_entry->action.cpu_queue = NULL;
			//Allow learning
			acl_entry->action.learn = false;
		}

		acl_entry->port_list[port_no] = TRUE;

		return true;
	}

	return false;

}


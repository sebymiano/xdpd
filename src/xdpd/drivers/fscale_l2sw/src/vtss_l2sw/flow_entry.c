#include "flow_entry.h"

#include <stdint.h>
#include <rofl/common/utils/c_logger.h>
#include <fsl_utils/fsl_utils.h>
#include "../config.h"

int aclId = 1;

vtss_rc vtss_l2sw_generate_acl_entry_matches(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry) {

	of1x_match_t* match;
	of1x_match_type_t type = OF1X_MATCH_MAX;
	uint8_t port_no;

	ROFL_DEBUG("["DRIVER_NAME"] %s num_of_matches: %x\n", __FUNCTION__, of1x_entry->matches.num_elements);

	if (aclId == VTSS_ACE_ID_LAST) {
		ROFL_ERR("["DRIVER_NAME"] flow_entry.c: reached maximum number of acl rules\n");
		return VTSS_RC_ERROR;
	}

	if (of1x_entry->matches.num_elements == 0) {
		if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ANY, acl_entry) != VTSS_RC_OK) {
			ROFL_ERR("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry\n");
			return VTSS_RC_ERROR;
		}
		ROFL_DEBUG("["DRIVER_NAME"] %s no matches for this entry, acl initialized with type ANY\n", __FUNCTION__);

		/* Monitor all ports */
		memset(acl_entry->port_list, TRUE, VTSS_PORT_ARRAY_SIZE * sizeof(acl_entry->port_list[0]));

		return VTSS_RC_OK;
	}

	//Go through all the matches and set entry matches
	for (match = of1x_entry->matches.head; match; match = match->next) {
		ROFL_DEBUG("["DRIVER_NAME"] of1x_entry->type : %x \n", match->type);

		switch (match->type) {
		case OF1X_MATCH_IN_PORT:
			port_no = of1x_get_match_value32(match);

			if (!add_or_update_match_in_port(acl_entry, type, port_no)) {
				ROFL_ERR("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: add_or_update_match_in_port failed\n");
				return VTSS_RC_ERROR;
			}

			break;
		case OF1X_MATCH_IN_PHY_PORT:
			port_no = of1x_get_match_value32(match);

			if (!add_or_update_match_in_port(acl_entry, type, port_no)) {
				ROFL_ERR("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: add_or_update_match_in_port failed\n");
				return VTSS_RC_ERROR;
			}

			break;
		case OF1X_MATCH_ETH_DST:
			break;
		case OF1X_MATCH_ETH_SRC:
			break;
		default:
			type = OF1X_MATCH_MAX;
			break;
		}

		type = match->type;
	}

	return VTSS_RC_OK;
}

vtss_rc vtss_l2sw_generate_acl_entry_actions(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry) {

	of1x_packet_action_t* action;
	uint16_t port;
	int i;

	//If entry has not actions we are done (should we really install it down there?)
	if (!of1x_entry->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions) {
		return VTSS_RC_OK;
	}

	action = of1x_entry->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions->head;

	if (!action) {
		assert(0);
		return VTSS_RC_ERROR;
	}

	for (i = VTSS_PORT_NO_START; i < VTSS_PORT_NO_END && is_valid_port(i); i++) {
		acl_entry->action.port_list[i] = false;
	}

	//Loop over apply actions only
	for (; action; action = action->next) {
		switch (action->type) {

		case OF1X_AT_OUTPUT:
			port = of1x_get_packet_action_field32(action);
			ROFL_DEBUG("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry_actions   ENTRY port %d\n", port);
			switch (of1x_get_packet_action_field32(action)) {
			case OF1X_PORT_CONTROLLER:
				ROFL_DEBUG("["DRIVER_NAME"] flow_entry.c: action for redirect packets to controller\n");
				acl_entry->action.learn = false;
				/* Enable CPU redirection */
				acl_entry->action.cpu = true;

				/*
				 // Disable forwarding of the frames
				 acl_entry->action.port_action = VTSS_ACL_PORT_ACTION_FILTER;
				 memset(acl_entry->action.port_list, FALSE, VTSS_PORT_ARRAY_SIZE * sizeof(acl_entry->action.port_list[0]));
				 */
				break;
			case OF1X_PORT_FLOOD:
			case OF1X_PORT_NORMAL:
			case OF1X_PORT_ALL:
			case OF1X_PORT_IN_PORT:
				ROFL_ERR("["DRIVER_NAME"] flow_entry.c: action not supported\n");
				return VTSS_RC_ERROR;
				break;
			default:
				if (is_valid_port(port) && !is_internal_port(port)) {
					//TODO: Here I should check for flooding or mirroring ecc...
					acl_entry->action.learn = false;
					acl_entry->action.port_action = VTSS_ACL_PORT_ACTION_REDIR;
					acl_entry->action.port_list[port] = true;
				} else {
					ROFL_ERR("["DRIVER_NAME"] flow_entry.c: wrong port in vtss_l2sw_generate_acl_entry_actions\n");
					assert(0);
				}
				break;
			}
			break;
		default:
			break;
		}
	}

	return VTSS_RC_OK;
}

vtss_l2sw_flow_entry_t* vtss_l2sw_init_vtss_flow_entry() {
	vtss_l2sw_flow_entry_t* entry;

	entry = malloc(sizeof(vtss_l2sw_flow_entry_t));
	if (!entry)
		return NULL;

	//memset entry
	memset(entry, 0, sizeof(*entry));

	entry->acl_id = ACL_INVALID_ID;

	return entry;
}

void vtss_l2sw_destroy_vtss_flow_entry(vtss_l2sw_flow_entry_t* entry) {
	if (!entry)
		return;

	free(entry);
}

bool add_or_update_match_in_port(vtss_ace_t* acl_entry, of1x_match_type_t type, int port_no) {
	if (is_valid_port(port_no) && !is_internal_port(port_no)) {
		if (type == OF1X_MATCH_MAX) {
			//This is the first match
			if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ANY, acl_entry) != VTSS_RC_OK) {
				ROFL_ERR(
						"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_IN_PORT\n");
				return VTSS_RC_ERROR;
			}

			//TODO: Maybe this initialization should be done directly in the action generate_acl_entry_actions
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


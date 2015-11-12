#include "flow_entry.h"

#include <stdint.h>
#include <rofl/common/utils/c_logger.h>
#include <fsl_utils/fsl_utils.h>
#include "../config.h"
#include "../util/flow_entry_utils.h"

int aclId = 1;

vtss_rc vtss_l2sw_generate_acl_entry_matches(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry) {

	of1x_match_t* match;
	of1x_match_type_t type = OF1X_MATCH_MAX;

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
			if (!add_or_update_match_in_port(acl_entry, type, match)) {
				ROFL_ERR("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: add_or_update_match_in_port failed\n");
				return VTSS_RC_ERROR;
			}
			break;
		case OF1X_MATCH_IN_PHY_PORT:
			if (!add_or_update_match_in_port(acl_entry, type, match)) {
				ROFL_ERR("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: add_or_update_match_in_port failed\n");
				return VTSS_RC_ERROR;
			}
			break;
		case OF1X_MATCH_ETH_DST:
			if (!add_or_update_eth_dst(acl_entry, type, match)) {
				ROFL_ERR("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: add_or_update_eth_dst failed\n");
				return VTSS_RC_ERROR;
			}
			break;
		case OF1X_MATCH_ETH_SRC:
			if (!add_or_update_eth_src(acl_entry, type, match)) {
				ROFL_ERR("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: add_or_update_eth_src failed\n");
				return VTSS_RC_ERROR;
			}
			break;
		default:
			//type = OF1X_MATCH_MAX;
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

vtss_rc vtss_l2sw_generate_mac_entry(vtss_mac_table_entry_t* mac_entry, of1x_flow_entry_t* of1x_entry) {
	of1x_match_t* match;
	of1x_packet_action_t* action;
	uint16_t port;

	ROFL_DEBUG("["DRIVER_NAME"] calling %s \n", __FUNCTION__);

	memset(mac_entry, 0, sizeof(vtss_mac_table_entry_t));
	//Default VLAN
	mac_entry->vid_mac.vid = 1;
	mac_entry->locked = true; /* static */
	memset(&mac_entry->destination, false, VTSS_PORT_ARRAY_SIZE * sizeof(mac_entry->destination[0]));
	//Go through all the matches and set entry matches
	for (match = of1x_entry->matches.head; match; match = match->next) {
		switch (match->type) {
		case OF1X_MATCH_ETH_DST:
			memcpy(&mac_entry->vid_mac.mac, &(match->__tern->value.u64), 6);
			break;
		case OF1X_MATCH_ETH_SRC:
			ROFL_DEBUG("["DRIVER_NAME"] flow_entry.c: skipping ethernet src in mac entry\n");
			break;
		case OF1X_MATCH_VLAN_VID:
			mac_entry->vid_mac.vid = of1x_get_match_value16(match);
			break;
		default:
			return VTSS_RC_ERROR;
			break;
		}
	}

	ROFL_DEBUG("["DRIVER_NAME"] flow_entry.c: added mac and vlan...adding actions\n");

	//Now add the destination port

	action = of1x_entry->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions->head;

	if (!action) {
		assert(0);
		return VTSS_RC_ERROR;
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
				mac_entry->copy_to_cpu = true;
				break;
			case OF1X_PORT_ALL:
			case OF1X_PORT_FLOOD:
				ROFL_DEBUG("["DRIVER_NAME"] flow_entry.c: action for flooding packets\n");
				memset(&mac_entry->destination, true, VTSS_PORT_ARRAY_SIZE * sizeof(mac_entry->destination[0]));
				break;
			case OF1X_PORT_NORMAL:
			case OF1X_PORT_IN_PORT:
				ROFL_ERR("["DRIVER_NAME"] flow_entry.c: action not supported\n");
				return VTSS_RC_ERROR;
			default:
				if (is_valid_port(port)) {
					mac_entry->destination[port] = true;
				} else {
					ROFL_ERR("["DRIVER_NAME"] flow_entry.c: wrong port in vtss_l2sw_generate_mac_entry\n");
					assert(0);
				}
				break;
			}
			break;
		default:
			return VTSS_RC_ERROR;
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


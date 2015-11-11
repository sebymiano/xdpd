#include "flow_entry_utils.h"

#include <stdint.h>
#include <rofl/common/utils/c_logger.h>
#include <fsl_utils/fsl_utils.h>
#include "../config.h"

bool add_or_update_match_in_port(vtss_ace_t* acl_entry, of1x_match_type_t type, of1x_match_t* match) {
	uint8_t port_no = of1x_get_match_value32(match);

	if (is_valid_port(port_no) && !is_internal_port(port_no)) {
		if (type == OF1X_MATCH_MAX) {
			//This is the first match
			if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ANY, acl_entry) != VTSS_RC_OK) {
				ROFL_ERR(
						"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_IN_PORT\n");
				return false;
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

bool add_or_update_eth_dst(vtss_ace_t* acl_entry, of1x_match_type_t type, of1x_match_t* match) {
	//FIXME: When I initialize the struct for in_port I put the FRAME_ANY but here I need the FRAME_ETH
	// the solution is: parse the match bitmap and if the match contains only a port_in let the FRAME_ANY
	//otherwise initialize with FRAME_ETH

	uint8_t addr[6];
	uint8_t addr_mask[6];

	memcpy(&addr, &(match->__tern->value.u64), 6);
	memcpy(&addr_mask, &(match->__tern->mask.u64), 6);

	if (type == OF1X_MATCH_MAX) {
		//This is the first match
		if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ETYPE, acl_entry) != VTSS_RC_OK) {
			ROFL_ERR(
					"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_ETH_DST\n");
			return false;
		}

		//TODO: Maybe this initialization should be done directly in the action generate_acl_entry_actions
		//Forward to CPU
		acl_entry->action.cpu = false;
		//Only first frame forwarded to CPU
		acl_entry->action.cpu_once = false;
		//CPU queue
		//acl_entry->action.cpu_queue = NULL;
		//Disable learning
		acl_entry->action.learn = false;
	} else if (type == OF1X_MATCH_IN_PORT || type == OF1X_MATCH_IN_PHY_PORT) {
		BOOL port_list[VTSS_PORT_ARRAY_SIZE];

		//Save the current value
		memcpy(&port_list, &acl_entry->port_list, VTSS_PORT_ARRAY_SIZE * sizeof(port_list[0]));

		if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ETYPE, acl_entry) != VTSS_RC_OK) {
			ROFL_ERR(
					"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_ETH_DST\n");
			return false;
		}

		//Restore the previous port_in
		memcpy(&acl_entry->port_list, &port_list, VTSS_PORT_ARRAY_SIZE * sizeof(port_list[0]));
	}
	//FIXME: Maybe here I should create the eth struct if it has been initialized with FRAME_ANY
	memcpy(&acl_entry->frame.etype.dmac.value, &addr, 6);
	memcpy(&acl_entry->frame.etype.dmac.mask, &addr_mask, 6);

	return true;
}

bool add_or_update_eth_src(vtss_ace_t* acl_entry, of1x_match_type_t type, of1x_match_t* match) {

	uint8_t addr[6];
	uint8_t addr_mask[6];

	memcpy(&addr, &(match->__tern->value.u64), 6);
	memcpy(&addr_mask, &(match->__tern->mask.u64), 6);

	if (type == OF1X_MATCH_MAX) {
		//This is the first match
		if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ETYPE, acl_entry) != VTSS_RC_OK) {
			ROFL_ERR(
					"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_ETH_DST\n");
			return false;
		}

		//TODO: Maybe this initialization should be done directly in the action generate_acl_entry_actions
		//Forward to CPU
		acl_entry->action.cpu = false;
		//Only first frame forwarded to CPU
		acl_entry->action.cpu_once = false;
		//CPU queue
		//acl_entry->action.cpu_queue = NULL;
		//Disable learning
		acl_entry->action.learn = false;
	} else if (type == OF1X_MATCH_IN_PORT || type == OF1X_MATCH_IN_PHY_PORT) {
		BOOL port_list[VTSS_PORT_ARRAY_SIZE];

		//Save the current value
		memcpy(&port_list, &acl_entry.port_list, VTSS_PORT_ARRAY_SIZE * sizeof(acl_entry.port_list[0]));

		if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ETYPE, acl_entry) != VTSS_RC_OK) {
			ROFL_ERR(
					"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_ETH_DST\n");
			return false;
		}

		//Restore the previous port_in
		memcpy(&acl_entry.port_list, &port_list, VTSS_PORT_ARRAY_SIZE * sizeof(port_list[0]));
	}

	//FIXME: Maybe here I should create the eth struct if it has been initialized with FRAME_ANY
	memcpy(&acl_entry->frame.etype.smac.value, &addr, 6);
	memcpy(&acl_entry->frame.etype.smac.mask, &addr_mask, 6);

	return true;
}

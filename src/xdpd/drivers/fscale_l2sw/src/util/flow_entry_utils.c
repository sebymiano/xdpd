#include "flow_entry_utils.h"

#include <stdint.h>
#include <rofl/common/utils/c_logger.h>
#include <fsl_utils/fsl_utils.h>
#include "../config.h"
#include "ports.h"
#include <rofl/datapath/pipeline/physical_switch.h>

bool add_or_update_match_in_port(vtss_ace_t* acl_entry, of1x_match_type_t type, of1x_match_t* match) {
	uint16_t lg_port;
	uint16_t hw_port;
	dpid_list_t* dpid_list;
	switch_port_t* switch_port;
	vtss_l2sw_port_t* vtss_port;

	lg_port = of1x_get_match_value32(match);

	dpid_list = physical_switch_get_all_lsi_dpids();
	if(dpid_list && dpid_list->dpids){
		switch_port = physical_switch_get_port_by_num(dpid_list->dpids[0], lg_port);
		dpid_list_destroy(dpid_list);
		if(switch_port){
			vtss_port = (vtss_l2sw_port_t*)switch_port->platform_port_state;
			hw_port = vtss_port->vtss_l2sw_port_num;
			ROFL_INFO("["DRIVER_NAME"] %s : Match in for logical port %u -> hardware port %u\n", __FUNCTION__, lg_port, hw_port);
		} else {
			ROFL_ERR("["DRIVER_NAME"] %s: Unable to retrieve the hardware port\n", __FUNCTION__);
			return VTSS_RC_ERROR;
		}
	} else {
		ROFL_ERR("["DRIVER_NAME"] %s: Unable to retrieve the logical switch\n", __FUNCTION__);
		return VTSS_RC_ERROR;
	}

	if (is_valid_port(hw_port) && !is_internal_port(hw_port)) {
		if (type == OF1X_MATCH_MAX) {
			//This is the first match
			if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ANY, acl_entry) != VTSS_RC_OK) {
				ROFL_ERR(
						"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_IN_PORT\n");
				return false;
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

		acl_entry->port_list[hw_port] = TRUE;

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

		/* Monitor all ports */
		memset(acl_entry->port_list, TRUE, VTSS_PORT_ARRAY_SIZE * sizeof(acl_entry->port_list[0]));

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

		/* Monitor all ports */
		memset(acl_entry->port_list, TRUE, VTSS_PORT_ARRAY_SIZE * sizeof(acl_entry->port_list[0]));

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
	memcpy(&acl_entry->frame.etype.smac.value, &addr, 6);
	memcpy(&acl_entry->frame.etype.smac.mask, &addr_mask, 6);

	return true;
}

bool add_or_update_eth_type(vtss_ace_t* acl_entry, of1x_match_type_t type, of1x_match_t* match) {

	uint8_t eth_type[2];
	uint8_t eth_type_mask[2];

	memcpy(&eth_type, &(match->__tern->value.u16), 2);
	memcpy(&eth_type_mask, &(match->__tern->mask.u16), 2);

	if (type == OF1X_MATCH_MAX) {
		//This is the first match
		if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ETYPE, acl_entry) != VTSS_RC_OK) {
			ROFL_ERR(
					"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_ETH_TYPE\n");
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

		/* Monitor all ports */
		memset(acl_entry->port_list, TRUE, VTSS_PORT_ARRAY_SIZE * sizeof(acl_entry->port_list[0]));

	} else if (type == OF1X_MATCH_IN_PORT || type == OF1X_MATCH_IN_PHY_PORT) {
		BOOL port_list[VTSS_PORT_ARRAY_SIZE];

		//Save the current value
		memcpy(&port_list, &acl_entry->port_list, VTSS_PORT_ARRAY_SIZE * sizeof(port_list[0]));

		if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ETYPE, acl_entry) != VTSS_RC_OK) {
			ROFL_ERR(
					"["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry for OF1X_MATCH_ETH_TYPE\n");
			return false;
		}

		//Restore the previous port_in
		memcpy(&acl_entry->port_list, &port_list, VTSS_PORT_ARRAY_SIZE * sizeof(port_list[0]));
	}

	//FIXME: Maybe here I should create the eth struct if it has been initialized with FRAME_ANY
	memcpy(&acl_entry->frame.etype.etype.value, &eth_type, 2);
	memcpy(&acl_entry->frame.etype.etype.mask, &eth_type_mask, 2);

	return true;
}


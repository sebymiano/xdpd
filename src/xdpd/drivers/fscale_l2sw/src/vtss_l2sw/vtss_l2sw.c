/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vtss_l2sw.h"
#include "ports.h"
#include <rofl/common/utils/c_logger.h>
#include <vtss_api/vtss_api.h>
#include <vtss_api/vtss_misc_api.h>
#include <vtss_api/vtss_l2_api.h>
#include <board_init/board_init.h>
#include <fsl_utils/fsl_utils.h>
#include <port_setup/port_setup.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include "../config.h"

#include <sys/types.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "flow_entry.h"
#include "../util/acl_util.h"

rofl_result_t vtss_l2sw_init() {
	vtss_inst_create_t create;
	vtss_inst_t inst;
	vtss_trace_conf_t conf;
	vtss_trace_layer_t layer;
	vtss_trace_group_t group;
	vtss_learn_mode_t learn_mode;

	int port_no;
	switch_port_t* port;

	for (group = 0; group < VTSS_TRACE_GROUP_COUNT; group++) {
		(void) vtss_trace_conf_get(group, &conf);
		for (layer = 0; layer < VTSS_TRACE_LAYER_COUNT; layer++)
			conf.level[layer] = VTSS_TRACE_LEVEL_ERROR;
		(void) vtss_trace_conf_set(group, &conf);
	}

	//Initialize create structure for target (Seville switch)
	vtss_inst_get(VTSS_TARGET_SEVILLE, &create);
	//Create target instance
	vtss_inst_create(&create, &inst);

	//Initialize the userspace driver. It maps the device registers to userspace memory
	//and sets the appropriate functions to read and write from these registers.
	//Moreover, it initializes the MDIO devices used to read and write the received
	//frames from the physical switch ports.
	board_init();

	//Automatic learning done by switch chip (default enabled)
	learn_mode.automatic = FALSE;
	//Learn frames copied to CPU (default disabled)
	learn_mode.cpu = FALSE;
	//Learn frames discarded (default disabled)
	learn_mode.discard = FALSE;

	for (port_no = VTSS_PORT_NO_START; port_no < VTSS_PORT_NO_END; port_no++) {
		//Check only if the port is in the valid range. N.B I don't want to add the internal ports
		if (is_valid_port(port_no)) {
			port_setup_init(port_no, 0);
			vtss_mac_table_port_flush(NULL, port_no);

			if (!is_internal_port(port_no)) {
			    if (fscale_l2sw_initialize_port(port_no, &port) != ROFL_SUCCESS || !port)
				       continue;

			    //Add to available ports
			    if (physical_switch_add_port(port) != ROFL_SUCCESS) {
				      return ROFL_FAILURE;
			    }
		  }

			//Disable forwarding on port
			vtss_port_state_set(NULL, port_no, FALSE);
			vtss_learn_port_mode_set(NULL, port_no, &learn_mode);
			port->up = false;
		}
	}

	vtss_l2sw_remove_all_acl();

	//The default action is to send all received packets to the controller
	vtss_l2sw_add_default_acl();

	return ROFL_SUCCESS;
}


rofl_result_t vtss_l2sw_destroy() {

	unsigned int max_ports, i;
	switch_port_t** array;

	array = physical_switch_get_physical_ports(&max_ports);
	for (i = 0; i < max_ports; i++) {
		if (array[i] != NULL) {
			fscale_l2sw_destroy_port(array[i]);
		}
	}

	//FIXME: Probably not needed
	array = physical_switch_get_virtual_ports(&max_ports);
	for (i = 0; i < max_ports; i++) {
		if (array[i] != NULL) {
			fscale_l2sw_destroy_port(array[i]);
		}
	}

	vtss_l2sw_remove_all_acl();

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_remove_all_acl(){
	uint32_t i;

	for(i = 0; i < FSCALE_L2SW_MAX_ACL_ID; i++){
		vtss_ace_del(NULL, i);
	}

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_add_default_acl(){
	vtss_ace_t acl_entry;
	int aclID;

	ROFL_INFO("["DRIVER_NAME"] %s(): generating default entry matches...\n", __FUNCTION__);

	if (vtss_ace_init(NULL, VTSS_ACE_TYPE_ANY, &acl_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] vtss_l2sw_generate_acl_entry: failed to initialize ACL entry\n");
		return VTSS_RC_ERROR;
	}

	/* Monitor all ports */
	memset(acl_entry.port_list, TRUE, VTSS_PORT_ARRAY_SIZE * sizeof(acl_entry.port_list[0]));

	ROFL_INFO("["DRIVER_NAME"] %s(): generating default entry actions...\n", __FUNCTION__);
	ROFL_INFO("["DRIVER_NAME"] %s(): action for redirect packets to controller\n", __FUNCTION__);

	acl_entry.action.learn = false;
	acl_entry.action.cpu = true;
	acl_entry.action.port_action = VTSS_ACL_PORT_ACTION_FILTER;

	aclID = getAclID_C();
	ROFL_INFO("["DRIVER_NAME"] %s(): adding default ACL with id: %d\n", __FUNCTION__, aclID);
	acl_entry.id = aclID;

	/* Add ACL entry */
	if (vtss_ace_add(NULL, VTSS_ACE_ID_LAST, &acl_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): vtss_ace_add failed, unable to add the ACL\n", __FUNCTION__);
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): default ACL added...\n", __FUNCTION__);

	return ROFL_SUCCESS;
}



rofl_result_t vtss_l2sw_add_flow_entry(of1x_flow_entry_t* entry) {
	vtss_ace_t acl_entry;
	vtss_l2sw_flow_entry_t* vtss_entry;
	int aclID;

	ROFL_INFO("["DRIVER_NAME"] %s(): generating ACL entry matches...\n", __FUNCTION__);

	if (vtss_l2sw_generate_acl_entry_matches(&acl_entry, entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): generation of ACL matches failed");
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): generating ACL entry actions...\n", __FUNCTION__);

	if (vtss_l2sw_generate_acl_entry_actions(&acl_entry, entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): generation of ACL actions failed\n", __FUNCTION__);
		return ROFL_FAILURE;
	}
	aclID = getAclID_C();

	ROFL_INFO("["DRIVER_NAME"] %s(): adding ACL with id: %d\n", __FUNCTION__, aclID);

	acl_entry.id = aclID;

	vtss_entry = vtss_l2sw_init_vtss_flow_entry();
	if (!vtss_entry)
		return ROFL_FAILURE;

	vtss_entry->type = VTSS_ENTRY_TYPE_ACL;
	vtss_entry->acl_id = aclID;
	//Do the association with the vtss_l2sw state
	entry->platform_state = (of1x_flow_entry_platform_state_t*) vtss_entry;

	/* Add ACL entry */
	if (vtss_ace_add(NULL, aclID + 1, &acl_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): vtss_ace_add failed, unable to add the ACL\n", __FUNCTION__);
		vtss_l2sw_destroy_vtss_flow_entry(vtss_entry);
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): ACL added...\n", __FUNCTION__);

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_delete_flow_entry(of1x_flow_entry_t* entry) {
	//Recover the position
	vtss_l2sw_flow_entry_t* hw_entry = (vtss_l2sw_flow_entry_t*) entry->platform_state;

	//Check
	if (!hw_entry || hw_entry->acl_id == ACL_INVALID_ID)
		return ROFL_FAILURE;

	ROFL_INFO("["DRIVER_NAME"] %s(): removing ACL with ID: %d\n", __FUNCTION__, hw_entry->acl_id);

	if (vtss_ace_del(NULL, hw_entry->acl_id) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): vtss_ace_del failed, unable to remove the ACL\n", __FUNCTION__);
		return ROFL_FAILURE;
	}

	releaseAclID_C(hw_entry->acl_id);
	vtss_l2sw_destroy_vtss_flow_entry(hw_entry);

	//FIXME: Here I should also release the id of this entry
	ROFL_INFO("["DRIVER_NAME"] %s(): ACL removed...\n", __FUNCTION__);

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_add_mac_entry(of1x_flow_entry_t* entry) {
	vtss_mac_table_entry_t mac_entry;
	vtss_l2sw_flow_entry_t* vtss_entry;

	ROFL_INFO("["DRIVER_NAME"] %s(): calling vtss_l2sw_add_mac_entry...\n", __FUNCTION__);

	if (vtss_l2sw_generate_mac_entry(&mac_entry, entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): vtss_l2sw_add_mac_entry failed", __FUNCTION__);
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): adding MAC entry...\n", __FUNCTION__);

	vtss_entry = vtss_l2sw_init_vtss_flow_entry();
	if (!vtss_entry)
		return ROFL_FAILURE;

	vtss_entry->type = VTSS_ENTRY_TYPE_MAC;
	memcpy(&vtss_entry->mac_entry, &mac_entry.vid_mac, sizeof(vtss_vid_mac_t));
	entry->platform_state = (of1x_flow_entry_platform_state_t*) vtss_entry;

	// Add MAC Table entry
	if (vtss_mac_table_add(NULL, &mac_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): vtss_mac_table_add failed, unable to add the MAC entry\n", __FUNCTION__);
		vtss_l2sw_destroy_vtss_flow_entry(vtss_entry);
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): MAC entry added...\n", __FUNCTION__);

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_detele_mac_entry(of1x_flow_entry_t* entry) {
	//Recover the position
	vtss_l2sw_flow_entry_t* hw_entry = (vtss_l2sw_flow_entry_t*) entry->platform_state;

	//Check
	if (!hw_entry || hw_entry->type != VTSS_ENTRY_TYPE_MAC)
		return ROFL_FAILURE;

	ROFL_INFO("["DRIVER_NAME"] %s(): removing MAC entry...\n", __FUNCTION__);

	if (vtss_mac_table_del(NULL, &hw_entry->mac_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): vtss_l2sw_detele_mac_entry failed, unable to remove the ACL\n", __FUNCTION__);
		return ROFL_FAILURE;
	}

	vtss_l2sw_destroy_vtss_flow_entry(hw_entry);

	ROFL_INFO("["DRIVER_NAME"] %s(): MAC entry removed...\n", __FUNCTION__);

	return ROFL_SUCCESS;
}

/*rofl_result_t vtss_l2sw_add_mac_entry_acl(of1x_flow_entry_t* entry) {
	vtss_ace_t mac_acl_entry;
	vtss_l2sw_flow_entry_t* vtss_entry;

	ROFL_INFO("["DRIVER_NAME"] %s(): calling vtss_l2sw_add_mac_entry_acl...\n", __FUNCTION__);

	if (vtss_l2sw_generate_mac_entry_acl(&mac_acl_entry, entry) != VTSS_RC_OK) {
		ROFL_ERR("%s(): vtss_l2sw_add_mac_entry_acl failed", __FUNCTION__);
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): adding ACL MAC entry...\n", __FUNCTION__);

	mac_acl_entry.id = aclId;

	vtss_entry = vtss_l2sw_init_vtss_flow_entry();
	if (!vtss_entry)
		return ROFL_FAILURE;

	vtss_entry->type = VTSS_ENTRY_TYPE_ACL;
	vtss_entry->acl_id = aclId;
	//Do the association with the vtss_l2sw state
	entry->platform_state = (of1x_flow_entry_platform_state_t*) vtss_entry;

	// Add ACL entry
	if (vtss_ace_add(NULL, VTSS_ACE_ID_LAST, &mac_acl_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] %s(): vtss_l2sw_add_mac_entry_acl failed, unable to add the MAC entry ACL\n", __FUNCTION__);
		vtss_l2sw_destroy_vtss_flow_entry(vtss_entry);
		return ROFL_FAILURE;
	}

	aclId++;

	ROFL_INFO("["DRIVER_NAME"] %s(): MAC entry ACL added...\n", __FUNCTION__);

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_detele_mac_entry_acl(of1x_flow_entry_t* entry) {
	ROFL_INFO("["DRIVER_NAME"] %s(): calling vtss_l2sw_detele_mac_entry_acl...\n", __FUNCTION__);
	//Recover the position
	vtss_l2sw_flow_entry_t* hw_entry = (vtss_l2sw_flow_entry_t*) entry->platform_state;

	//Check
	if (!hw_entry || hw_entry->acl_id == ACL_INVALID_ID)
		return ROFL_FAILURE;

	ROFL_INFO("["DRIVER_NAME"] %s(): removing MAC entry ACL...\n", __FUNCTION__);

	if (vtss_ace_del(NULL, hw_entry->acl_id) != VTSS_RC_OK) {
		ROFL_ERR(
				"["DRIVER_NAME"] %s(): vtss_l2sw_detele_mac_entry_acl failed, unable to remove the MAC entry ACL\n", __FUNCTION__);
		return ROFL_FAILURE;
	}

	vtss_l2sw_destroy_vtss_flow_entry(hw_entry);

	//FIXME: Here I should also release the id of this entry
	ROFL_INFO("["DRIVER_NAME"] %s(): MAC entry ACL removed...\n", __FUNCTION__);

	return ROFL_SUCCESS;
}*/

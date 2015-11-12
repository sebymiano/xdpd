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

rofl_result_t vtss_l2sw_init() {
	vtss_inst_create_t create;
	vtss_inst_t inst;
	vtss_trace_conf_t conf;
	vtss_trace_layer_t layer;
	vtss_trace_group_t group;

	int port_no;
	switch_port_t* port;

	for (group = 0; group < VTSS_TRACE_GROUP_COUNT; group++) {
		(void) vtss_trace_conf_get(group, &conf);
		for (layer = 0; layer < VTSS_TRACE_LAYER_COUNT; layer++)
			conf.level[layer] = VTSS_TRACE_LEVEL_ERROR;
		(void) vtss_trace_conf_set(group, &conf);
	}

	vtss_inst_get(VTSS_TARGET_SEVILLE, &create);
	vtss_inst_create(&create, &inst);

	board_init();

	for (port_no = VTSS_PORT_NO_START; port_no < VTSS_PORT_NO_END; port_no++) {
		//Check only if the port is in the valid range
		if (is_valid_port(port_no)) {
			port_setup_init(port_no, 0);
			vtss_mac_table_port_flush(NULL, port_no);
			if (!is_internal_port(port_no)) {

				if (initialize_port(port_no, &port) != ROFL_SUCCESS || !port)
					continue;

				//Add to available ports
				if (physical_switch_add_port(port) != ROFL_SUCCESS) {
					return ROFL_FAILURE;
				}

				vtss_port_state_set(NULL, port_no, FALSE);
				port->up = false;

			} else {
				//Enable forwarding only on the internal ports in the initialization phase
				vtss_port_state_set(NULL, port_no, TRUE);
			}
		}
	}

	//Here maybe I should create a new thread for receiving new frame

	return ROFL_SUCCESS;

}

rofl_result_t vtss_l2sw_destroy() {

	unsigned int max_ports, i;
	switch_port_t** array;

	array = physical_switch_get_physical_ports(&max_ports);
	for (i = 0; i < max_ports; i++) {
		if (array[i] != NULL) {
			destroy_port(array[i]);
		}
	}

	//FIXME: Probably not needed
	array = physical_switch_get_virtual_ports(&max_ports);
	for (i = 0; i < max_ports; i++) {
		if (array[i] != NULL) {
			destroy_port(array[i]);
		}
	}

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_add_flow_entry(of1x_flow_entry_t* entry) {
	vtss_ace_t acl_entry;
	vtss_l2sw_flow_entry_t* vtss_entry;

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: calling  generate_acl_entry_matches...\n");

	if (vtss_l2sw_generate_acl_entry_matches(&acl_entry, entry) != VTSS_RC_OK) {
		ROFL_ERR("vtss_l2sw.c: vtss_l2sw_add_flow_entry failed");
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: calling  generate_acl_entry_actions...\n");

	if (vtss_l2sw_generate_acl_entry_actions(&acl_entry, entry) != VTSS_RC_OK) {
		ROFL_ERR("vtss_l2sw.c: vtss_l2sw_generate_acl_entry_actions failed\n");
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: adding acl...\n");

	acl_entry.id = aclId;

	vtss_entry = vtss_l2sw_init_vtss_flow_entry();
	if (!vtss_entry)
		return ROFL_FAILURE;

	vtss_entry->type = VTSS_ENTRY_TYPE_ACL;
	vtss_entry->acl_id = aclId;
	//Do the association with the vtss_l2sw state
	entry->platform_state = (of1x_flow_entry_platform_state_t*) vtss_entry;

	/* Add ACL entry */
	if (vtss_ace_add(NULL, VTSS_ACE_ID_LAST, &acl_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] vtss_l2sw.c: vtss_ace_add failed, unable to add the acl\n");
		vtss_l2sw_destroy_vtss_flow_entry(vtss_entry);
		return ROFL_FAILURE;
	}

	aclId++;

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: acl added...\n");

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_delete_flow_entry(of1x_flow_entry_t* entry) {
	//Recover the position
	vtss_l2sw_flow_entry_t* hw_entry = (vtss_l2sw_flow_entry_t*) entry->platform_state;

	//Check
	if (!hw_entry || hw_entry->acl_id == ACL_INVALID_ID)
		return ROFL_FAILURE;

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: removing acl...\n");

	if (vtss_ace_del(NULL, hw_entry->acl_id) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] vtss_l2sw.c: vtss_ace_del failed, unable to remove the acl\n");
		return ROFL_FAILURE;
	}

	vtss_l2sw_destroy_vtss_flow_entry(hw_entry);

	//FIXME: Here I should also release the id of this entry
	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: acl removed...\n");

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_add_mac_entry(of1x_flow_entry_t* entry) {
	vtss_mac_table_entry_t mac_entry;
	vtss_l2sw_flow_entry_t* vtss_entry;

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: calling vtss_l2sw_add_mac_entry...\n");

	if (vtss_l2sw_generate_mac_entry(&mac_entry, entry) != VTSS_RC_OK) {
		ROFL_ERR("vtss_l2sw.c: vtss_l2sw_add_mac_entry failed");
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: adding mac entry...\n");

	vtss_entry = vtss_l2sw_init_vtss_flow_entry();
	if (!vtss_entry)
		return ROFL_FAILURE;

	vtss_entry->type = VTSS_ENTRY_TYPE_MAC;
	memcpy(&vtss_entry->mac_entry, &mac_entry.vid_mac, sizeof(vtss_vid_mac_t));
	entry->platform_state = (of1x_flow_entry_platform_state_t*) vtss_entry;

	/* Add MAC Table entry */
	if (vtss_mac_table_add(NULL, &mac_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] vtss_l2sw.c: vtss_mac_table_add failed, unable to add the mac entry\n");
		vtss_l2sw_destroy_vtss_flow_entry(vtss_entry);
		return ROFL_FAILURE;
	}

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: mac entry added...\n");

	return ROFL_SUCCESS;
}

rofl_result_t vtss_l2sw_detele_mac_entry(of1x_flow_entry_t* entry) {
	//Recover the position
	vtss_l2sw_flow_entry_t* hw_entry = (vtss_l2sw_flow_entry_t*) entry->platform_state;

	//Check
	if (!hw_entry || hw_entry->type != VTSS_ENTRY_TYPE_MAC)
		return ROFL_FAILURE;

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: removing acl...\n");

	if (vtss_mac_table_del(NULL, hw_entry->mac_entry) != VTSS_RC_OK) {
		ROFL_ERR("["DRIVER_NAME"] vtss_l2sw.c: vtss_ace_del failed, unable to remove the acl\n");
		return ROFL_FAILURE;
	}

	vtss_l2sw_destroy_vtss_flow_entry(hw_entry);

	ROFL_INFO("["DRIVER_NAME"] vtss_l2sw.c: acl removed...\n");

	return ROFL_SUCCESS;
}


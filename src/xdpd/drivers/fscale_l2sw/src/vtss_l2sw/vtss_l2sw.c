/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vtss_l2sw.h"
#include "ports.h"
#include <rofl/common/utils/c_logger.h>
#include <vtss_api/vtss_api.h>
#include <fscale_l2sw/board_init/board_init.h>
#include <fscale_l2sw/fsl_utils/fsl_utils.h>
#include <fscale_l2sw/port_setup/port_setup.h>
#include <rofl/datapath/pipeline/physical_switch.h>

#include <sys/types.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>

rofl_result_t vtss_l2sw_init() {
	vtss_inst_create_t create;
	vtss_inst_t inst;
	vtss_trace_conf_t conf;
	vtss_trace_layer_t layer;
	vtss_trace_group_t group;

	int port_no;
	switch_port_t* port;

	for (group = 0; group < VTSS_TRACE_GROUP_COUNT; group++) {
	 (void)vtss_trace_conf_get(group, &conf);
	 for (layer = 0; layer < VTSS_TRACE_LAYER_COUNT; layer++)
	 conf.level[layer] = VTSS_TRACE_LEVEL_ERROR;
	 (void)vtss_trace_conf_set(group, &conf);
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

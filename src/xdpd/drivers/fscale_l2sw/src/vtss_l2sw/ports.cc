#include "ports.h"
#include "vtss_l2sw.h"
#include <stdio.h>
#include <rofl/common/utils/c_logger.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <vtss_api/vtss_api.h>
#include <vtss_api/vtss_phy_api.h>
#include <vtss_api/vtss_l2_api.h>
#include "../config.h"
extern "C" {

#include <fsl_utils/fsl_utils.h>

}

rofl_result_t fscale_l2sw_bring_port_up(vtss_l2sw_port_t* port) {
	int port_no;
	vtss_port_status_t status_phy;

	port_no = port->vtss_l2sw_port_num;

	if (is_valid_port(port_no)) {
		if (vtss_phy_status_get(NULL, port_no, &status_phy) != VTSS_RC_OK) {
			ROFL_ERR("["DRIVER_NAME"] %s(): Can't get status for port phy\n", __FUNCTION__);
			//Can't get status for port phy
			return ROFL_FAILURE;
		}

		/* if there is no link, no need to
		 * bring up the port
		 */
		//if (status_phy.link == FALSE || status_phy.link_down == TRUE) {
		//	ROFL_ERR("["DRIVER_NAME"] %s(): Attempt to bring up a port that has no link\n", __FUNCTION__);
		//	return ROFL_FAILURE;
		//}

		//Enable forwarding on the specified port (bring up)
		vtss_port_state_set(NULL, port_no, TRUE);
		ROFL_INFO("["DRIVER_NAME"] %s(): port %d forwarding enabled\n", __FUNCTION__, port_no);

		return ROFL_SUCCESS;
	}

	return ROFL_FAILURE;
}

rofl_result_t fscale_l2sw_bring_port_down(vtss_l2sw_port_t* port) {
	int port_no;
	vtss_port_status_t status_phy;

	port_no = port->vtss_l2sw_port_num;

	if (is_valid_port(port_no)) {
		if (vtss_phy_status_get(NULL, port_no, &status_phy) != VTSS_RC_OK) {
			ROFL_ERR("["DRIVER_NAME"] %s(): Can't get status for port phy\n", __FUNCTION__);
			//Can't get status for port phy
			return ROFL_FAILURE;
		}

		//Disable forwarding on the specified port (bring down)
		vtss_port_state_set(NULL, port_no, FALSE);
		ROFL_INFO("["DRIVER_NAME"] %s(): port %d forwarding disabled\n", __FUNCTION__, port_no);

		return ROFL_SUCCESS;
	}

	return ROFL_FAILURE;
}

rofl_result_t fscale_l2sw_initialize_port(int port_no, switch_port_t** port) {
	char iface_name[FSCALE_L2SW_INTERFACE_NAME_LEN] = "0";

	//Set port operational state (TRUE if link is up)
	vtss_port_state_set(NULL, port_no, FALSE);

	//Set the physical internface name used internally by the driver (fswX)
	snprintf(iface_name, FSCALE_L2SW_INTERFACE_NAME_LEN,
	FSCALE_L2SW_INTERFACE_BASE_NAME"%d", port_no);

	//Initialize the switch_port structure
	*port = switch_port_init(iface_name, true, PORT_TYPE_PHYSICAL, PORT_STATE_NONE);

	vtss_l2sw_port_t* vtss_port = (vtss_l2sw_port_t*) malloc(sizeof(vtss_l2sw_port_t));

	if(!vtss_port){
		switch_port_destroy(*port);
		return ROFL_FAILURE;
	}

	vtss_port->vtss_l2sw_port_num = port_no;

	//Initialize platform specific info
	(*port)->platform_port_state = (platform_port_state_t*) vtss_port;

	return ROFL_SUCCESS;
}

rofl_result_t fscale_l2sw_destroy_port(switch_port_t* port) {
	vtss_l2sw_port_t* vtss_port;

	vtss_port = (vtss_l2sw_port_t*) port->platform_port_state;

	if (vtss_port) {
		free(vtss_port);
		return ROFL_SUCCESS;
	} else {
		return ROFL_FAILURE;
	}

}

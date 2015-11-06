#include "ports.h"
#include "vtss_l2sw.h"
#include <stdio.h>
#include <rofl/common/utils/c_logger.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <vtss_api/vtss_api.h>
#include <vtss_api/vtss_phy_api.h>
#include <vtss_api/vtss_l2_api.h>
extern "C" {

#include <fsl_utils/fsl_utils.h>

}

rofl_result_t vtss_l2sw_bring_port_up(vtss_l2sw_port_t* port) {
	int port_no;
	vtss_port_status_t status_phy;

	port_no = port->vtss_l2sw_port_num;

	if (is_valid_port(port_no)) {
		if (vtss_phy_status_get(NULL, port_no, &status_phy) != VTSS_RC_OK) {
			ROFL_ERR("[vtss_l2sw]port.c: can't get status for port phy\n");
			//Can't get status for port phy
			return ROFL_FAILURE;
		}

		/* if there is no link, no need to
		 * bring up the port
		 */
		if (status_phy.link == FALSE || status_phy.link_down == TRUE) {
			ROFL_ERR("[vtss_l2sw]port.c: attempt to bring up a port that has no link\n");
			return ROFL_FAILURE;
		}

		//Enable forwarding on the specified port (bring up)
		vtss_port_state_set(NULL, port_no, TRUE);
		ROFL_ERR("[vtss_l2sw]port.c: port %d activated\n", port_no);

		return ROFL_SUCCESS;
	}

	return ROFL_FAILURE;
}

rofl_result_t vtss_l2sw_bring_port_down(vtss_l2sw_port_t* port) {
	int port_no;
	vtss_port_status_t status_phy;

	port_no = port->vtss_l2sw_port_num;

	if (is_valid_port(port_no)) {
		if (vtss_phy_status_get(NULL, port_no, &status_phy) != VTSS_RC_OK) {
			ROFL_ERR("[vtss_l2sw]port.c: can't get status for port phy\n");
			//Can't get status for port phy
			return ROFL_FAILURE;
		}

		//Disable forwarding on the specified port (bring down)
		vtss_port_state_set(NULL, port_no, FALSE);
		ROFL_ERR("[vtss_l2sw]port.c: port %d disactivated\n", port_no);

		return ROFL_SUCCESS;
	}

	return ROFL_FAILURE;
}

rofl_result_t initialize_port(int port_no, switch_port_t** port) {
	char iface_name[FSCALE_L2SW_INTERFACE_NAME_LEN] = "0";

	vtss_port_state_set(NULL, port_no, FALSE);

	snprintf(iface_name, FSCALE_L2SW_INTERFACE_NAME_LEN,
	FSCALE_L2SW_INTERFACE_BASE_NAME"%d", port_no);

	//Initialize pipeline structure
	*port = switch_port_init(iface_name, true, PORT_TYPE_PHYSICAL, PORT_STATE_LIVE);

	vtss_l2sw_port_t* vtss_port = (vtss_l2sw_port_t*) malloc(sizeof(vtss_l2sw_port_t));
	vtss_port->vtss_l2sw_port_num = port_no;

	//Initialize platform specific info
	(*port)->platform_port_state = (platform_port_state_t*) vtss_port;

	return ROFL_SUCCESS;
}

rofl_result_t destroy_port(switch_port_t* port) {
	vtss_l2sw_port_t* vtss_port;

	vtss_port = (vtss_l2sw_port_t*) port->platform_port_state;

	if (vtss_port) {
		free(vtss_port);
		return ROFL_SUCCESS;
	} else {
		return ROFL_FAILURE;
	}

}

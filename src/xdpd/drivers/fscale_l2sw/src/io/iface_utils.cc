#include <linux/ethtool.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <linux/sockios.h>
//#include <netpacket/packet.h>
#include <netinet/in.h>
//#include <net/if.h>
#include <stdio.h>
#include <map>
#include <unistd.h>

//Prototypes
#include <rofl/common/utils/c_logger.h>
#include <rofl/datapath/pipeline/platform/memory.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/common/datapacket.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/hal/cmm.h>
#include "iface_utils.h"

#include "../vtss_l2sw/ports.h"

using namespace xdpd::gnu_linux;

/*
 *
 * Port management
 *
 * All of the functions related to physical port management
 *
 */

rofl_result_t update_port_status(char * name, bool up) {
	switch_port_t *port;
	switch_port_snapshot_t *port_snapshot;

	port = physical_switch_get_port_by_name(name);

	if (!port)
		return ROFL_SUCCESS; //Port deleted

	vtss_l2sw_port_t* vtss_port =
			((vtss_l2sw_port_t*) port->platform_port_state);

	if (up) {
		if (vtss_l2sw_bring_port_up(vtss_port) != ROFL_SUCCESS) {
			return ROFL_FAILURE;
		}
	} else {
		if (vtss_l2sw_bring_port_down(vtss_port) != ROFL_SUCCESS) {
			return ROFL_FAILURE;
		}
	}

	//Notify the change of state to the CMM
	port_snapshot = physical_switch_get_port_snapshot(port->name);
	hal_cmm_notify_port_status_changed(port_snapshot);

	return ROFL_SUCCESS;
}

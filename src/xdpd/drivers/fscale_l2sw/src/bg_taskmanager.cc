#include "bg_taskmanager.h"

#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/rtnetlink.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>

#include <rofl/common/utils/c_logger.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/hal/driver.h>
#include <rofl/datapath/hal/cmm.h>

#include "io/iface_utils.h"
#include "io/datapacket_storage.h"
#include "io/bufferpool.h"
#include "pipeline-imp/ls_internal_state.h"
#include "util/time_utils.h"
#include "vtss_l2sw/vtss_l2sw.h"
#include "vtss_l2sw/ports.h"

#include <vtss_api/vtss_api.h>
extern "C" {

#include <fsl_utils/fsl_utils.h>

}
#include "util/time_utils.h"
#include "io/iface_utils.h"

using namespace xdpd::gnu_linux;

//Local static variable for background manager thread
static pthread_t bg_thread;
static bool bg_continue_execution = true;
/**
 * This piece of code is meant to manage a thread that is support for:
 *
 * - execute the expiration of the flow entries.
 * - update the status of the ports
 * - TODO: Get the packet_in events from the kernel and send appropriate OF messages
 * - free space in the buffer pool when a buffer is too old
 * - more?
 */

void check_port_status() {
	vtss_port_no_t port_no;
	vtss_port_status_t status_phy;
	switch_port_t* port;
	char iface_name[FSCALE_L2SW_INTERFACE_NAME_LEN] = "0";

	for (port_no = VTSS_PORT_NO_START; port_no < VTSS_PORT_NO_END; port_no++) {
		if (is_valid_port(port_no) && !is_internal_port(port_no)) {
			if (vtss_phy_status_get(NULL, port_no, &status_phy) != VTSS_RC_OK) {
				ROFL_ERR("["DRIVER_NAME"] %s(): Can't get status for port phy\n", __FUNCTION__);
				//Can't get status for port phy
				continue;
			}

			snprintf(iface_name, FSCALE_L2SW_INTERFACE_NAME_LEN,
			FSCALE_L2SW_INTERFACE_BASE_NAME"%d", port_no);

			port = physical_switch_get_port_by_name(iface_name);

			if (port->up && (status_phy.link == FALSE || status_phy.link_down == TRUE)) {
				//Port changed to down state
				ROFL_INFO("["DRIVER_NAME"] %s(): Port changed to down state, updating state...\n", __FUNCTION__);
				update_port_status(iface_name, FALSE);
				ROFL_INFO("["DRIVER_NAME"] %s(): Port state updated", __FUNCTION__);
			} else if (!port->up && (status_phy.link == TRUE && status_phy.link_down == FALSE)) {
				ROFL_INFO("["DRIVER_NAME"] %s(): Port changed to up state, updating state...\n", __FUNCTION__);
				//Port changed to up state
				update_port_status(iface_name, TRUE);
				ROFL_INFO("["DRIVER_NAME"] %s(): Port state updated\n", __FUNCTION__);
			}
		}
	}

}

void update_misc_stats() {
	unsigned int i, max_ports;
	switch_port_t** array;
	vtss_port_counters_t counters;

	array = physical_switch_get_physical_ports(&max_ports);
	for (i = 0; i < max_ports; i++) {
		if (array[i] != NULL) {
			int port_no = ((vtss_l2sw_port_t *) array[i]->platform_port_state)->vtss_l2sw_port_num;
			if (is_valid_port(port_no)) {
				if (vtss_port_counters_get(NULL, port_no, &counters) != VTSS_RC_OK) {
					//Cannot read counters for this port
					continue;
				}

				array[i]->stats.rx_packets = counters.rmon.rx_etherStatsPkts;
				array[i]->stats.tx_packets = counters.rmon.tx_etherStatsPkts;
				array[i]->stats.rx_bytes = counters.rmon.rx_etherStatsOctets;
				array[i]->stats.tx_bytes = counters.rmon.tx_etherStatsOctets;
				array[i]->stats.rx_dropped = counters.rmon.rx_etherStatsDropEvents;
				array[i]->stats.tx_dropped = counters.rmon.tx_etherStatsDropEvents;
				array[i]->stats.rx_errors = counters.if_group.ifInErrors;
				array[i]->stats.tx_errors = counters.if_group.ifOutDiscards;
				array[i]->stats.rx_frame_err = counters.rmon.rx_etherStatsCRCAlignErrors;
				array[i]->stats.rx_over_err = counters.rmon.rx_etherStatsOversizePkts;
				array[i]->stats.rx_crc_err = counters.rmon.rx_etherStatsCRCAlignErrors;
				array[i]->stats.collisions = counters.rmon.tx_etherStatsCollisions;
			}
		}
	}
}

int process_timeouts() {
	datapacket_t* pkt;
	unsigned int i, max_switches;
	struct timeval now;
	of_switch_t** logical_switches;
	static struct timeval last_time_entries_checked = { 0, 0 }, last_time_pool_checked = { 0, 0 };

	ROFL_INFO("["DRIVER_NAME"] getting current date time\n");
	gettimeofday(&now, NULL);

	ROFL_INFO("["DRIVER_NAME"] getting logical switches\n");
	//Retrieve the logical switches list
	logical_switches = physical_switch_get_logical_switches(&max_switches);

	if (get_time_difference_ms(&now, &last_time_entries_checked) >= LSW_TIMER_SLOT_MS) {
#ifdef DEBUG
		static int dummy = 0;
#endif

		//TIMERS FLOW ENTRIES
		for (i = 0; i < max_switches; i++) {

			if (logical_switches[i] != NULL) {
				of_process_pipeline_tables_timeout_expirations(logical_switches[i]);

#ifdef DEBUG
				if(dummy%20 == 0)
				of1x_full_dump_switch((of1x_switch_t*)logical_switches[i], false);
#endif
			}
		}

#ifdef DEBUG
		dummy++;
		ROFL_INFO("["DRIVER_NAME"] <%s:%d> Checking flow entries expirations %lu:%lu\n",__func__,__LINE__,now.tv_sec,now.tv_usec);
#endif
		last_time_entries_checked = now;
	}

	if (get_time_difference_ms(&now, &last_time_pool_checked) >= LSW_TIMER_BUFFER_POOL_MS) {
		uint32_t buffer_id;
		datapacket_storage *dps = NULL;

		for (i = 0; i < max_switches; i++) {

			if (logical_switches[i] != NULL) {

				dps = ((struct logical_switch_internals*) logical_switches[i]->platform_state)->storage;
				//TODO process buffers in the storage
				while (dps->oldest_packet_needs_expiration(&buffer_id)) {

					ROFL_INFO("<%s:%d> trying to erase a datapacket from storage\n", __func__, __LINE__);
					if ((pkt = dps->get_packet(buffer_id)) == NULL) {
						ROFL_INFO("Error in get_packet_wrapper %u\n", buffer_id);
					} else {
						ROFL_INFO("Datapacket expired correctly %u\n", buffer_id);
						//Return buffer to bufferpool
						bufferpool::release_buffer(pkt);
					}

				}
			}
		}

#ifdef DEBUG
		ROFL_ERR("["DRIVER_NAME"] <%s:%d> Checking pool buffers expirations %lu:%lu\n",__func__,__LINE__,now.tv_sec,now.tv_usec);
#endif
		last_time_pool_checked = now;
	}

	return ROFL_SUCCESS;
}

/**
 * @name x86_background_tasks_thread
 * @brief contents the infinite loop checking for ports and timeouts
 */
void* x86_background_tasks_routine(void* param) {
	while (bg_continue_execution) {

		sleep(1);

		ROFL_INFO("[fscale_l2sw]bg_taskmanager.cc: updating statistics...\n");
		update_misc_stats();

		ROFL_INFO("[fscale_l2sw]bg_taskmanager.cc: processing timeouts...\n");
		process_timeouts();

		ROFL_INFO("[fscale_l2sw]bg_taskmanager.cc: checking port status...\n");
		check_port_status();

	}

	//Printing some information
	ROFL_INFO("[bg] Finishing thread execution\n");

	//Exit
	pthread_exit(NULL);
}

/**
 * launches the main thread
 */
rofl_result_t launch_background_tasks_manager() {
	//Set flag
	bg_continue_execution = true;

	ROFL_INFO("[fscale_l2sw]bg_taskmanager.cc: launching background tasks manager\n");

	if (pthread_create(&bg_thread, NULL, x86_background_tasks_routine, NULL) < 0) {
		ROFL_ERR("<%s:%d> pthread_create failed\n", __func__, __LINE__);
		return ROFL_FAILURE;
	}
	return ROFL_SUCCESS;
}

rofl_result_t stop_background_tasks_manager() {

	ROFL_INFO("[fscale_l2sw]bg_taskmanager.cc: stopping background tasks manager\n");

	bg_continue_execution = false;
	pthread_join(bg_thread, NULL);

	ROFL_INFO("[fscale_l2sw]bg_taskmanager.cc: background tasks manager stopped\n");
	return ROFL_SUCCESS;
}

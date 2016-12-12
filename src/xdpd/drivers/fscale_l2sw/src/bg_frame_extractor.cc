#include "bg_frame_extractor.h"

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
#include <inttypes.h>

#include <rofl/common/utils/c_logger.h>
#include <rofl/datapath/hal/driver.h>
#include <rofl/datapath/hal/cmm.h>
#include <rofl/datapath/pipeline/switch_port.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>

#include <rofl/datapath/hal/openflow/openflow1x/of1x_driver.h>
#include <rofl/datapath/hal/openflow/openflow1x/of1x_cmm.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_statistics.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline_pp.h>
#include <rofl/datapath/pipeline/openflow/of_switch_pp.h>

#include "io/iface_utils.h"
#include "io/datapacket_storage.h"
#include "io/bufferpool.h"
#include "io/datapacketx86.h"
#include "pipeline-imp/ls_internal_state.h"
#include "config.h"
#include "vtss_l2sw/ports.h"
#include "vtss_l2sw/vtss_l2sw.h"

#include <vtss_api/vtss_api.h>
#include <vtss_api/vtss_packet_api.h>
extern "C" {

#include <fsl_utils/fsl_utils.h>

}
#include "util/time_utils.h"
#include "io/iface_utils.h"

//Local static variable for background manager thread
static pthread_t bg_thread;
static bool bg_continue_execution = true;

/**
 * @name x86_background_tasks_thread
 * @brief contents the infinite loop checking for ports and timeouts
 */
void* bg_frame_extractor_routine(void* param) {
	u32 xtr_grp_cfg_old, inj_grp_cfg_old;

	l2sw_reg_read(0, XTR_GRP_CFG_REG, &xtr_grp_cfg_old);
	l2sw_reg_read(0, INJ_GRP_CFG_REG, &inj_grp_cfg_old);

	l2sw_reg_write(0, XTR_GRP_CFG_REG, xtr_grp_cfg_old & ~(CPU_FRAME_BYTE_SWAP | CPU_FRAME_EOF_WORD_POS_AFTER));
	l2sw_reg_write(0, INJ_GRP_CFG_REG, inj_grp_cfg_old & ~(CPU_FRAME_BYTE_SWAP));

	while (bg_continue_execution) {
		sleep(1);

		ROFL_INFO("["DRIVER_NAME"] %s(): reading from CPU queues...\n", __FUNCTION__);
		read_frame_from_cpu_queues(MAX_FRAMES_RECV);
	}

	//Printing some information
	ROFL_INFO("["DRIVER_NAME"]  %s(): Finishing bg_frame_extractor execution\n", __FUNCTION__);

	/* restore CPU injection/extraction configuration */
	l2sw_reg_write(0, XTR_GRP_CFG_REG, xtr_grp_cfg_old);
	l2sw_reg_write(0, INJ_GRP_CFG_REG, inj_grp_cfg_old);

	//Exit
	pthread_exit(NULL);
}

/**
 * launches the main thread
 */
rofl_result_t launch_background_frame_extractor() {
	//Set flag
	bg_continue_execution = true;

	ROFL_INFO("["DRIVER_NAME"] %s(): launching background frame extractor\n", __FUNCTION__);

	if (pthread_create(&bg_thread, NULL, bg_frame_extractor_routine, NULL) < 0) {
		ROFL_ERR("<%s:%d> pthread_create failed\n", __func__, __LINE__);
		return ROFL_FAILURE;
	}
	return ROFL_SUCCESS;
}

rofl_result_t stop_background_frame_extractor() {

	ROFL_INFO("["DRIVER_NAME"] %s(): stopping background frame extractor\n", __FUNCTION__);

	bg_continue_execution = false;
	pthread_join(bg_thread, NULL);

	ROFL_INFO("["DRIVER_NAME"] %s(): background frame extractor stopped\n", __FUNCTION__);
	return ROFL_SUCCESS;
}

void generate_new_packet_in(vtss_packet_rx_header_t *header, vtss_packet_rx_queue_t queue, u8 *frame) {
	packet_matches_t matches;
	char iface_name[FSCALE_L2SW_INTERFACE_NAME_LEN] = "0";
	switch_port_t* port;

	snprintf(iface_name, FSCALE_L2SW_INTERFACE_NAME_LEN,
	FSCALE_L2SW_INTERFACE_BASE_NAME"%d", header->port_no);

	port = physical_switch_get_port_by_name(iface_name);

	if ((port != NULL) && (port->attached_sw != NULL) && port->of_generate_packet_in) {

		//Retrieve an empty buffer
		datapacket_t* pkt = xdpd::gnu_linux::bufferpool::get_buffer();

		if (!pkt)
			return;

		xdpd::gnu_linux::datapacketx86* pack = (xdpd::gnu_linux::datapacketx86*) pkt->platform_state;

		pack->init(frame, header->length, port->attached_sw, port->of_port_num, 0, true, false);

		of1x_switch_t* sw = (of1x_switch_t*) port->attached_sw;

		of_switch_t* lsw;
		lsw = physical_switch_get_logical_switch_by_dpid(sw->dpid);

		xdpd::gnu_linux::datapacket_storage* storage;
		storage = ((logical_switch_internals*) lsw->platform_state)->storage;

		//TODO: Check if this assignment is correct
		pack->pktin_send_len = ((of1x_switch_t*) sw)->pipeline.miss_send_len;

		xdpd::gnu_linux::storeid storage_id = storage->store_packet(pkt);
		ROFL_INFO("["DRIVER_NAME"] %s(): PACKET_IN storage ID %d for datapacket pkt %d dpid %d  \n",
				__FUNCTION__, storage_id, pkt, sw->dpid);

		//Fill matches
		fill_packet_matches(pkt, &matches);

		//Process packet through the software pipeline
		of_process_packet_pipeline(ROFL_PIPELINE_LOCKED_TID, lsw, pkt);

		/*//Process packet in
		hal_result r = hal_cmm_process_of1x_packet_in(sw->dpid, pack->pktin_table_id, pack->pktin_reason,
				pack->clas_state.port_in, storage_id, pkt->__cookie, pack->get_buffer(), pack->pktin_send_len,
				pack->get_buffer_length(), &matches);

		ROFL_INFO(
				"["DRIVER_NAME"] %s(): sw->dpid = %u, pack->pktin_table_id = %u, reason = %u \
						 port_in = %u, storage_id = %u, pktin_send_len = %u, buffer_length = %u\n",
				__FUNCTION__, sw->dpid, pack->pktin_table_id, pack->pktin_reason, pack->clas_state.port_in, storage_id,
				pack->pktin_send_len, pack->get_buffer_length());

		if (HAL_FAILURE == r) {
			ROFL_INFO("["DRIVER_NAME"] %s(): cmm packet_in unsuccessful \n", __FUNCTION__);

			//TODO: I should remove also the packet from the storage (is not possible)
			xdpd::gnu_linux::bufferpool::release_buffer(pkt);
		}
		if (HAL_SUCCESS == r)
			ROFL_INFO("["DRIVER_NAME"] %s(): cmm packet_in successful \n", __FUNCTION__);*/
	} else {
		if (!port) {
			ROFL_INFO("["DRIVER_NAME"] %s(): port doesn't exist", __FUNCTION__);
			return;
		}

		if (!port->is_attached_to_sw) {
			ROFL_INFO("["DRIVER_NAME"] %s(): port %d is not attached to a logical switch\n", __FUNCTION__, header->port_no);
		}
		if (!port->of_generate_packet_in) {
			ROFL_INFO("["DRIVER_NAME"] %s(): port %d cannot generate pkt_in \n", __FUNCTION__, header->port_no);
		}
	}

}

/*
 * Receive at most <count> frames
 */
void read_frame_from_cpu_queues(u32 count) {
	vtss_packet_rx_queue_t queue;
	vtss_packet_rx_header_t header;
	u8 frame[FRAME_MAX_SIZE + FRAME_HEADER_SIZE];
	u32 frames = 0;
	bool nothing_received;

	while (count > frames && bg_continue_execution) {
		nothing_received = true;

		for (queue = VTSS_PACKET_RX_QUEUE_START; queue < VTSS_PACKET_RX_QUEUE_END; queue++) {
			/* Read the frame using VTSS API */
			if (vtss_packet_rx_frame_get(NULL, queue, &header, frame, FRAME_MAX_SIZE) != VTSS_RC_OK)
				continue;

			frames++;
			nothing_received = false;
			ROFL_INFO("["DRIVER_NAME"] %s(): Received frame on port: %u, queue: %u, length: %u\n",
					__FUNCTION__, header.port_no, queue, header.length);
			//dump_frame(&header, queue, frame);
			generate_new_packet_in(&header, queue, frame);
		}

		if (nothing_received)
			break;
	}
}

/* Dump frame */
/*static void dump_frame(vtss_packet_rx_header_t *header, vtss_packet_rx_queue_t queue, u8 *frame) {
 char buf[100], *p;
 u32 i;

 ROFL_INFO("["DRIVER_NAME"] Received frame on port: %u, queue: %u, length: %u\n", header->port_no, queue,
 header->length);
 for (i = 0, p = &buf[0]; i < header->length; i++) {
 if ((i % 16) == 0) {
 p = &buf[0];
 p += sprintf(p, "%04x: ", i);
 }

 p += sprintf(p, "%02x%c", frame[i], ((i + 9) % 16) == 0 ? '-' : ' ');
 if (((i + 1) % 16) == 0 || (i + 1) == header->length)
 ROFL_INFO("%s\n", buf);
 }
 ROFL_INFO("\n");
 }*/

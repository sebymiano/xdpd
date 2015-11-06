#ifndef _BACKGROUND_FRAME_EXTRACTOR_
#define _BACKGROUND_FRAME_EXTRACTOR_

#include <pthread.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl_datapath.h>

#define XTR_GRP_CFG_REG		(0x080000 >> 2)
#define INJ_GRP_CFG_REG		(0x080024 >> 2)

#define CPU_FRAME_BYTE_SWAP		0x00000001
#define CPU_FRAME_EOF_WORD_POS_AFTER	0x00000002

#define MAX_FRAMES_RECV	100

#define FRAME_MAX_SIZE	2000
#define FRAME_HEADER_SIZE	16

/* Number of control frames received per chunk */
#define CONTROL_FRAMES_RECV	100
//C++ extern C
ROFL_BEGIN_DECLS

/**
 * launches the main thread
 */

void read_frame_from_cpu_queues(u32 count);

void generate_new_packet_in(vtss_packet_rx_header_t *header, vtss_packet_rx_queue_t queue, u8 *frame);

rofl_result_t launch_background_frame_extractor(void);

rofl_result_t stop_background_frame_extractor(void);

//C++ extern C
ROFL_END_DECLS

#endif //_BACKGROUND_FRAME_EXTRACTOR_


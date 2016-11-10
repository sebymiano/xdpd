#ifndef _VTSS_L2SW_PORTS_H_
#define _VTSS_L2SW_PORTS_H_

#include <rofl_datapath.h>
#include <rofl/datapath/pipeline/switch_port.h>

typedef struct vtss_l2sw_port {
	//This is the number of the physical port in the L2 switch
	int vtss_l2sw_port_num;

} vtss_l2sw_port_t;

//C++ extern C
ROFL_BEGIN_DECLS

/**
 * @brief Detach all physical ports to switch
 */
rofl_result_t fscale_l2sw_bring_port_up(vtss_l2sw_port_t* port);

/**
 * @brief Attach all physical ports to switch
 */
rofl_result_t fscale_l2sw_bring_port_down(vtss_l2sw_port_t* port);

/**
 * @brief Destroy a port releasing all memory allocated
 */
rofl_result_t fscale_l2sw_destroy_port(switch_port_t* port);

/**
 * @brief Destroy a port releasing all memory allocated
 */
rofl_result_t fscale_l2sw_initialize_port(int port_no, switch_port_t** port);

//C++ extern C
ROFL_END_DECLS

#endif /* _VTSS_L2SW_PORTS_H_ */

#include <rofl_datapath.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <rofl/datapath/hal/driver.h>
#include <rofl/common/utils/c_logger.h>
#include <rofl/datapath/hal/cmm.h>
#include <rofl/datapath/pipeline/platform/memory.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h>

#include "../config.h"

//DPDK includes
#include <rte_config.h> 
#include <rte_common.h> 
#include <rte_eal.h> 
#include <rte_errno.h> 
#include <rte_launch.h> 
#include <rte_mempool.h> 
#include <rte_mbuf.h> 
#include <rte_ethdev.h> 

//only for Test
#include <stdlib.h>
#include <string.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/common/datapacket.h>
#include "../bg_taskmanager.h"
#include "../io/bufferpool.h"
#include "../io/iface_manager.h"
#include "../io/pktin_dispatcher.h"
#include "../processing/processing.h"

//Extensions
#include "nf_extensions.h"
//+] Add more here...

extern int optind;

using namespace xdpd::gnu_linux;

//Extra params MACROS
#define DRIVER_EXTRA_COREMASK "coremask"
#define DRIVER_EXTRA_POOL_SIZE "pool_size"

//Some useful macros
#define STR(a) #a
#define XSTR(a) STR(a)

#define GNU_LINUX_DPDK_CODE_NAME "gnu-linux-dpdk"
#define GNU_LINUX_DPDK_VERSION VERSION
#define GNU_LINUX_DPDK_DESC \
"GNU/Linux DPDK driver.\n\n"\
"The GNU/Linux DPDK driver is a user-space software driver that uses Data Plane Development Kit(DPDK)[1] as the I/O Framework.\n\n"\
"This driver supports Network Function port extensions. This functions enable xDPd to communicate to other entities via dedicated ports. The port type mapping from abstract xDPd ports to DPDK is the following:\n"\
"   -NATIVE: not supported.\n"\
"   -SHMEM: rte_ring shared memory port. The NF name is the name of the RTE ring.\n"\
"   -EXTERNAL: KNI port.\n"\
"\n\n"\
"[1] http://www.dpdk.org"
#define GNU_LINUX_DPDK_USAGE  \
"\t\t\t\t" DRIVER_EXTRA_COREMASK "=<hexadecimal mask>;\t - DPDK coremask.\n"\
"\t\t\t\t" DRIVER_EXTRA_POOL_SIZE "=<#bufs>;\t\t - Number of MBUFs in the pool (per CPU socket).\n"

#define GNU_LINUX_DPDK_EXTRA_PARAMS "This driver has a number of optional \"extra parameters\" that can be used using -e option, specifying them as key1=value1;key2=value2... :\n\n"\
"   " DRIVER_EXTRA_COREMASK "=<hexadecimal mask>\t - Overrides default coremaskDPDK EAL coremask. Default: " XSTR(DEFAULT_RTE_CORE_MASK) ".\n"\
"   " DRIVER_EXTRA_POOL_SIZE "=<#bufs>;\t\t - Override the number of MBUFs or size of the pool (per CPU socket). Default: " XSTR(DEFAULT_NB_MBUF) ".\n\n"\
"The use of \"extra-params\" is highly discouraged for production machines. Tunning of the config.h is preferable.\n\n"


//Number of MBUFs per pool (per CPU socket)
unsigned int mbuf_pool_size = DEFAULT_NB_MBUF;

//Fake argv for eal
static const char* argv_fake[] = {"xdpd", "-c", NULL, "-n", XSTR(RTE_MEM_CHANNELS), NULL};

#define MAX_COREMASK_LEN 64
static char coremask[MAX_COREMASK_LEN];

/*
* @name    hal_driver_init
* @brief   Initializes driver. Before using the HAL_DRIVER routines, higher layers must allow driver to initialize itself
* @ingroup driver_management
*/

static void parse_extra_params(const std::string& params){

	std::istringstream ss(params);
	std::string t, r;

	//Asign the coremask pointer
	strncpy(coremask, XSTR(DEFAULT_RTE_CORE_MASK), MAX_COREMASK_LEN);
	argv_fake[2] = coremask;

	//First split
	while(std::getline(ss, t, ';')) {
		std::istringstream ss_(t);

		//Recover parameter
		std::getline(ss_, r, '=');
		r.erase(std::remove_if( r.begin(), r.end(), ::isspace ),
								r.end() );

		if(r.compare(DRIVER_EXTRA_COREMASK) == 0){
			std::getline(ss_, r, '=');
			r.erase(std::remove_if( r.begin(), r.end(),
						::isspace ), r.end() );

			strncpy(coremask, r.c_str(), MAX_COREMASK_LEN);
			ROFL_DEBUG(DRIVER_NAME" Overriding default coremask(%s) with %s\n", XSTR(DEFAULT_RTE_CORE_MASK), coremask);
		}else if(r.compare(DRIVER_EXTRA_POOL_SIZE) == 0){
			std::getline(ss_, r, '=');
			r.erase(std::remove_if( r.begin(), r.end(),
						::isspace ), r.end() );

			std::istringstream ss__(r);
			unsigned int mbufs;
			ss__ >> mbufs;
			mbuf_pool_size = mbufs;
			ROFL_DEBUG(DRIVER_NAME" Overriding default #mbufs per pool(%u) with %u\n", DEFAULT_NB_MBUF, mbufs);
		}else{
			t.erase(std::remove_if( t.begin(), t.end(),
						::isspace ), t.end() );

			//Unknown or unparsable parameter
			ROFL_WARN(DRIVER_NAME" WARNING: could not understand extra-param '%s'. Ignoring it...\n",
							t.c_str());
		}
	}
}


hal_result_t hal_driver_init(hal_extension_ops_t* extensions, const char* extra_params){

	int ret;
	const int EAL_ARGS = sizeof(argv_fake)/sizeof(*argv_fake);

	ROFL_INFO(DRIVER_NAME" Initializing...\n");

	//Parse extra parameters
	parse_extra_params(std::string(extra_params));

	//Show a nice trace
	ROFL_INFO(DRIVER_NAME" Initializing EAL with coremask: %s, memchannels: %s\n", argv_fake[2], argv_fake[4]);

	// init EAL library
	optind=1;
	ret = rte_eal_init(EAL_ARGS-1, (char**)argv_fake);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "rte_eal_init failed");
	optind=1;

	//Make sure lcore count > 2. Core 0 left for mgmt
	if( rte_lcore_count() < 2 ){
		ROFL_ERR(DRIVER_NAME"ERROR: the system must have at last 2 cores. Aborting\n");
		return HAL_FAILURE;
	}

	//Init PKT_IN bufferpool
	bufferpool::init();

	//Initialize pipeline
	if(physical_switch_init() != ROFL_SUCCESS)
		return HAL_FAILURE;

	//Discover and initialize rofl-pipeline state
	if(iface_manager_discover_system_ports() != ROFL_SUCCESS) {
		ROFL_ERR(DRIVER_NAME"ERROR: Failed to discover system ports - Aborting\n");
		return HAL_FAILURE;
	}

	//Initialize processing
	if(processing_init() != ROFL_SUCCESS)
		return HAL_FAILURE;

	//Initialize PKT_IN
	if(pktin_dispatcher_init() != ROFL_SUCCESS)
		return HAL_FAILURE;

	//Initialize Background Tasks Manager
	if(launch_background_tasks_manager() != ROFL_SUCCESS){
		return HAL_FAILURE;
	}

#ifdef GNU_LINUX_DPDK_ENABLE_NF
	//Add extensions
	memset(extensions, 0, sizeof(hal_extension_ops_t));
	extensions->nf_ports.create_nf_port = hal_driver_dpdk_nf_create_nf_port;
	extensions->nf_ports.destroy_nf_port = hal_driver_dpdk_nf_destroy_nf_port;
#endif

	return HAL_SUCCESS; 
}

/**
* @name    hal_driver_get_info
* @brief   Get the information of the driver (code-name, version, usage...)
* @ingroup driver_management
*/
void hal_driver_get_info(driver_info_t* info){
	//Fill-in hal_driver_info_t
	strncpy(info->code_name, GNU_LINUX_DPDK_CODE_NAME, DRIVER_CODE_NAME_MAX_LEN);
	strncpy(info->version, GNU_LINUX_DPDK_VERSION, DRIVER_VERSION_MAX_LEN);
	strncpy(info->description, GNU_LINUX_DPDK_DESC, DRIVER_DESCRIPTION_MAX_LEN);
	strncpy(info->usage, GNU_LINUX_DPDK_USAGE, DRIVER_USAGE_MAX_LEN);
	strncpy(info->extra_params, GNU_LINUX_DPDK_EXTRA_PARAMS, DRIVER_EXTRA_PARAMS_MAX_LEN);
}

/*
* @name    hal_driver_destroy
* @brief   Destroy driver state. Allows platform state to be properly released. 
* @ingroup driver_management
*/
hal_result_t hal_driver_destroy(){

	ROFL_INFO(DRIVER_NAME" Destroying...\n");
	
	//Cleanup processing. This must be the first thing to do
	processing_destroy();

	//Initialize PKT_IN
	pktin_dispatcher_destroy();

	//Stop the bg manager
	stop_background_tasks_manager();

	//Destroy pipeline platform state
	physical_switch_destroy();

	// destroy bufferpool
	bufferpool::destroy();
	
	//Shutdown ports
	iface_manager_destroy();

	return HAL_SUCCESS; 
}

/*
* Switch management functions
*/
/**
* @brief   Checks if an LSI with the specified dpid exists 
* @ingroup logical_switch_management
*/
bool hal_driver_switch_exists(uint64_t dpid){
	return physical_switch_get_logical_switch_by_dpid(dpid) != NULL;
}

/**
* @brief   Retrieve the list of LSIs dpids
* @ingroup logical_switch_management
* @retval  List of available dpids, which MUST be deleted using dpid_list_destroy().
*/
dpid_list_t* hal_driver_get_all_lsi_dpids(void){
	return physical_switch_get_all_lsi_dpids();  
}

/**
 * @name hal_driver_get_switch_snapshot_by_dpid 
 * @brief Retrieves a snapshot of the current state of a switch port, if the port name is found. The snapshot MUST be deleted using switch_port_destroy_snapshot()
 * @ingroup logical_switch_management
 * @retval  Pointer to of_switch_snapshot_t instance or NULL 
 */
of_switch_snapshot_t* hal_driver_get_switch_snapshot_by_dpid(uint64_t dpid){
	return physical_switch_get_logical_switch_snapshot(dpid);
}


/*
* @name    hal_driver_create_switch 
* @brief   Instruct driver to create an OF logical switch 
* @ingroup logical_switch_management
* @retval  Pointer to of_switch_t instance 
*/
hal_result_t hal_driver_create_switch(char* name, uint64_t dpid, of_version_t of_version, unsigned int num_of_tables, int* ma_list){
	
	of_switch_t* sw;
	
	ROFL_INFO(DRIVER_NAME" Creating switch. Name: %s, number of tables: %d\n",name, num_of_tables);
	
	sw = (of_switch_t*)of1x_init_switch(name, of_version, dpid, num_of_tables, (enum of1x_matching_algorithm_available*) ma_list);

	if(!sw)
		return HAL_FAILURE;

	//If you would fully use ROFL-pipeline you woudl call then
	physical_switch_add_logical_switch(sw);
	
	return HAL_SUCCESS;
}


/*
* @name    hal_driver_destroy_switch_by_dpid 
* @brief   Instructs the driver to destroy the switch with the specified dpid 
* @ingroup logical_switch_management
*/
hal_result_t hal_driver_destroy_switch_by_dpid(const uint64_t dpid){

	unsigned int i;
	
	//Try to retrieve the switch
	of_switch_t* sw = physical_switch_get_logical_switch_by_dpid(dpid);
	
	if(!sw)
		return HAL_FAILURE;

	//Desechedule all ports. Do not feed more packets to the switch
	for(i=0;i<sw->max_ports;i++){

		if(sw->logical_ports[i].attachment_state == LOGICAL_PORT_STATE_ATTACHED && sw->logical_ports[i].port){
			hal_driver_detach_port_from_switch(dpid, sw->logical_ports[i].port->name);
		}
	}	


	//Note: There is no need to drain PKT_INs here. Will be done in the destroy hook of the pipeline

	//Detach ports from switch. 
	if(physical_switch_detach_all_ports_from_logical_switch(sw)!=ROFL_SUCCESS)
		return HAL_FAILURE;

	//Remove switch from the switch bank
	if(physical_switch_remove_logical_switch(sw)!=ROFL_SUCCESS)
		return HAL_FAILURE;
	
	return HAL_SUCCESS;
}

/*
* Port management 
*/

/**
* @brief   Checks if a port with the specified name exists 
* @ingroup port_management 
*/
bool hal_driver_port_exists(const char *name){
	return physical_switch_get_port_by_name(name) != NULL; 
}

/**
* @brief   Retrieve the list of names of the available ports of the platform. You may want to 
* 	   call hal_driver_get_port_snapshot_by_name(name) to get more information of the port 
* @ingroup port_management
* @retval  List of available port names, which MUST be deleted using switch_port_name_list_destroy().
*/
switch_port_name_list_t* hal_driver_get_all_port_names(void){
	return physical_switch_get_all_port_names(); 
}

/**
 * @name hal_driver_get_port_by_name
 * @brief Retrieves a snapshot of the current state of a switch port, if the port name is found. The snapshot MUST be deleted using switch_port_destroy_snapshot()
 * @ingroup port_management
 */
switch_port_snapshot_t* hal_driver_get_port_snapshot_by_name(const char *name){
	return physical_switch_get_port_snapshot(name); 
}

/**
 * @name hal_driver_get_port_by_num
 * @brief Retrieves a snapshot of the current state of the port of the Logical Switch Instance with dpid at port_num, if exists. The snapshot MUST be deleted using switch_port_destroy_snapshot()
 * @ingroup port_management
 * @param dpid DatapathID 
 * @param port_num Port number
 */
switch_port_snapshot_t* hal_driver_get_port_snapshot_by_num(uint64_t dpid, unsigned int port_num){
	
	of_switch_t* lsw;
	
	lsw = physical_switch_get_logical_switch_by_dpid(dpid);
	if(!lsw)
		return NULL; 

	//Check if the port does exist.
	if(!port_num || port_num >= LOGICAL_SWITCH_MAX_LOG_PORTS || !lsw->logical_ports[port_num].port)
		return NULL;

	return physical_switch_get_port_snapshot(lsw->logical_ports[port_num].port->name); 
}


/*
* @name    hal_driver_attach_physical_port_to_switch
* @brief   Attemps to attach a system's port to switch, at of_port_num if defined, otherwise in the first empty OF port number.
* @ingroup management
*
* @param dpid Datapath ID of the switch to attach the ports to
* @param name Port name (system's name)
* @param of_port_num If *of_port_num is non-zero, try to attach to of_port_num of the logical switch, otherwise try to attach to the first available port and return the result in of_port_num
*/
hal_result_t hal_driver_attach_port_to_switch(uint64_t dpid, const char* name, unsigned int* of_port_num){

	switch_port_t* port;
	of_switch_t* lsw;

	//Check switch existance
	lsw = physical_switch_get_logical_switch_by_dpid(dpid);
	if(!lsw){
		return HAL_FAILURE;
	}
	
	//Check if the port does exist
	port = physical_switch_get_port_by_name(name);
	if(!port)
		return HAL_FAILURE;

	ROFL_INFO(DRIVER_NAME" Trying to attach port %s. to switch %s (0x%llx)\n", port->name, lsw->name, (long long unsigned int)lsw->dpid);
	
	//Update pipeline state
	if(*of_port_num == 0){
		//no port specified, we assign the first available
		if(physical_switch_attach_port_to_logical_switch(port,lsw,of_port_num) == ROFL_FAILURE){
			assert(0);
			return HAL_FAILURE;
		}
	}else{
		if(physical_switch_attach_port_to_logical_switch_at_port_num(port,lsw,*of_port_num) == ROFL_FAILURE){
			assert(0);
			return HAL_FAILURE;
		}
	}
	
	if(port->type == PORT_TYPE_VIRTUAL){
		/*
		* Virtual link
		*/
		//Do nothing
	}else{
		//Schedule the port in the I/O subsystem
		if(processing_schedule_port(port) != ROFL_SUCCESS){
			assert(0);
			return HAL_FAILURE;
		}
	}		

	return HAL_SUCCESS;
}

/**
* @name    hal_driver_connect_switches
* @brief   Attemps to connect two logical switches via a virtual port. Forwarding module may or may not support this functionality. 
* @ingroup management
*
* @param dpid_lsi1 Datapath ID of the LSI1
* @param dpid_lsi2 Datapath ID of the LSI2 
*/
hal_result_t hal_driver_connect_switches(uint64_t dpid_lsi1, unsigned int* port_num1, switch_port_snapshot_t** port1, uint64_t dpid_lsi2, unsigned int* port_num2, switch_port_snapshot_t** port2) {

	of_switch_t *lsw1, *lsw2;
	switch_port_snapshot_t *port1_not, *port2_not;
	switch_port_t *vport1, *vport2;

	//Check existance of the dpid
	lsw1 = physical_switch_get_logical_switch_by_dpid(dpid_lsi1);
	lsw2 = physical_switch_get_logical_switch_by_dpid(dpid_lsi2);

	if(!lsw1 || !lsw2){
		assert(0);
		return HAL_FAILURE;
	}
	
	//Create virtual port pair
	if(iface_manager_create_virtual_port_pair(lsw1, &vport1, lsw2, &vport2) != ROFL_SUCCESS){
		assert(0);
		return HAL_FAILURE;
	}

	//Attach both ports
	if(hal_driver_attach_port_to_switch(dpid_lsi1, vport1->name, port_num1) != HAL_SUCCESS){
		assert(0);
		return HAL_FAILURE;
	}
	if(hal_driver_attach_port_to_switch(dpid_lsi2, vport2->name, port_num2) != HAL_SUCCESS){
		assert(0);
		return HAL_FAILURE;
	}

	//Notify port add as requested by the API
	port1_not = physical_switch_get_port_snapshot(vport1->name);
	port2_not = physical_switch_get_port_snapshot(vport2->name);
	if(!port1_not || !port2_not){
		assert(0);
		return HAL_FAILURE;
	}
	
	hal_cmm_notify_port_add(port1_not);
	hal_cmm_notify_port_add(port2_not);


	//Enable interfaces (start packet transmission)
	if(hal_driver_bring_port_up(vport1->name) != HAL_SUCCESS || hal_driver_bring_port_up(vport2->name) != HAL_SUCCESS){
		ROFL_ERR(DRIVER_NAME" ERROR: unable to bring up vlink ports.\n");
		assert(0);
		return HAL_FAILURE;
	}

	//Set switch ports and return
	*port1 = physical_switch_get_port_snapshot(vport1->name);
	*port2 = physical_switch_get_port_snapshot(vport2->name);
	
	assert(*port1 != NULL);
	assert(*port2 != NULL);

	return HAL_SUCCESS; 
}

/*
* @name    hal_driver_detach_port_from_switch
* @brief   Detaches a port from the switch 
* @ingroup port_management
*
* @param dpid Datapath ID of the switch to detach the ports
* @param name Port name (system's name)
*/
hal_result_t hal_driver_detach_port_from_switch(uint64_t dpid, const char* name){
	of_switch_t* lsw;
	switch_port_t *port, *port_pair;
	switch_port_snapshot_t *port_snapshot=NULL, *port_pair_snapshot=NULL;	
	
	lsw = physical_switch_get_logical_switch_by_dpid(dpid);
	if(!lsw)
		return HAL_FAILURE;

	port = physical_switch_get_port_by_name(name);

	//Check if the port does exist and is really attached to the dpid
	if( !port || !port->attached_sw || port->attached_sw->dpid != dpid)
		return HAL_FAILURE;

	//Detach it
	if(physical_switch_detach_port_from_logical_switch(port,lsw) != ROFL_SUCCESS){
		ROFL_ERR(DRIVER_NAME" Error detaching port %s.\n",port->name);
		assert(0);
		goto DRIVER_DETACH_ERROR;	
	}
	
	if(port->type == PORT_TYPE_VIRTUAL){
		/*
		* Virtual link
		*/
		
		//Snapshoting the port *before* it is detached 
		port_snapshot = physical_switch_get_port_snapshot(port->name); 

		port_pair = (switch_port_t*)port->platform_port_state;
		port_pair_snapshot = physical_switch_get_port_snapshot(port_pair->name); 
		
		//Notify removal of both ports
		hal_cmm_notify_port_delete(port_snapshot);
		hal_cmm_notify_port_delete(port_pair_snapshot);
	
		//Detach pair
		if(physical_switch_detach_port_from_logical_switch(port_pair, port_pair->attached_sw) != ROFL_SUCCESS){
			ROFL_ERR(DRIVER_NAME" Error detaching port %s.\n",port_pair->name);
			assert(0);
			goto DRIVER_DETACH_ERROR;	
		}

		//Remove from the pipeline and delete
		if(physical_switch_remove_port(port->name) != ROFL_SUCCESS){
			ROFL_ERR(DRIVER_NAME" Error removing port from the physical_switch. The port may become unusable...\n");
			assert(0);
			return HAL_FAILURE;
			
		}
		
		if(physical_switch_remove_port(port_pair->name) != ROFL_SUCCESS){
			ROFL_ERR(DRIVER_NAME" Error removing port from the physical_switch. The port may become unusable...\n");
			assert(0);
			goto DRIVER_DETACH_ERROR;
			
		}
	}else{
		//Deschedule port from processing (physical or NF port)
		processing_deschedule_port(port);
	}


	
	return HAL_SUCCESS; 

DRIVER_DETACH_ERROR:
	if(port_snapshot)
		switch_port_destroy_snapshot(port_snapshot);	
	if(port_pair_snapshot)
		switch_port_destroy_snapshot(port_pair_snapshot);	

	return HAL_FAILURE; 
}

/*
* @name    hal_driver_detach_port_from_switch_at_port_num
* @brief   Detaches port_num of the logical switch identified with dpid 
* @ingroup port_management
*
* @param dpid Datapath ID of the switch to detach the ports
* @param of_port_num Number of the port (OF number) 
*/
hal_result_t hal_driver_detach_port_from_switch_at_port_num(uint64_t dpid, const unsigned int of_port_num){

	of_switch_t* lsw;
	
	lsw = physical_switch_get_logical_switch_by_dpid(dpid);
	if(!lsw)
		return HAL_FAILURE;

	//Check if the port does exist.
	if(!of_port_num || of_port_num >= LOGICAL_SWITCH_MAX_LOG_PORTS || !lsw->logical_ports[of_port_num].port)
		return HAL_FAILURE;

	return hal_driver_detach_port_from_switch(dpid, lsw->logical_ports[of_port_num].port->name);
}


//Port admin up/down stuff

/*
* Port administrative management actions (ifconfig up/down like)
*/

/*
* @name    hal_driver_bring_port_up
* @brief   Brings up a system port. If the port is attached to an OF logical switch, this also schedules port for I/O and triggers PORTMOD message. 
* @ingroup port_management
*
* @param name Port system name 
*/
hal_result_t hal_driver_bring_port_up(const char* name){

	switch_port_t* port;
	switch_port_snapshot_t* port_snapshot;

	//Check if the port does exist
	port = physical_switch_get_port_by_name(name);

	if(!port || !port->platform_port_state)
		return HAL_FAILURE;

	//Bring it up
	iface_manager_bring_up(port);

	port_snapshot = physical_switch_get_port_snapshot(port->name); 
	hal_cmm_notify_port_status_changed(port_snapshot);
	
	return HAL_SUCCESS;
}


/*
* @name    hal_driver_bring_port_down
* @brief   Shutdowns (brings down) a system port. If the port is attached to an OF logical switch, this also de-schedules port and triggers PORTMOD message. 
* @ingroup port_management
*
* @param name Port system name 
*/
hal_result_t hal_driver_bring_port_down(const char* name){

	switch_port_t* port;
	switch_port_snapshot_t* port_snapshot;
	
	//Check if the port does exist
	port = physical_switch_get_port_by_name(name);
	if(!port || !port->platform_port_state)
		return HAL_FAILURE;

	//Bring it down
	iface_manager_bring_down(port);

	port_snapshot = physical_switch_get_port_snapshot(port->name); 
	hal_cmm_notify_port_status_changed(port_snapshot);
	
	return HAL_SUCCESS;
}

/*
* @name    hal_driver_bring_port_up_by_num
* @brief   Brings up a port from an OF logical switch (and the underlying physical interface). This function also triggers the PORTMOD message 
* @ingroup port_management
*
* @param dpid DatapathID 
* @param port_num OF port number
*/
hal_result_t hal_driver_bring_port_up_by_num(uint64_t dpid, unsigned int port_num){

	of_switch_t* lsw;
	
	lsw = physical_switch_get_logical_switch_by_dpid(dpid);
	if(!lsw)
		return HAL_FAILURE;

	//Check if the port does exist and is really attached to the dpid
	if( !lsw->logical_ports[port_num].port || lsw->logical_ports[port_num].attachment_state != LOGICAL_PORT_STATE_ATTACHED || lsw->logical_ports[port_num].port->attached_sw->dpid != dpid)
		return HAL_FAILURE;

	return hal_driver_bring_port_up(lsw->logical_ports[port_num].port->name);
}

/*
* @name    hal_driver_bring_port_down_by_num
* @brief   Brings down a port from an OF logical switch (and the underlying physical interface). This also triggers the PORTMOD message.
* @ingroup port_management
*
* @param dpid DatapathID 
* @param port_num OF port number
*/
hal_result_t hal_driver_bring_port_down_by_num(uint64_t dpid, unsigned int port_num){

	of_switch_t* lsw;
	
	lsw = physical_switch_get_logical_switch_by_dpid(dpid);
	if(!lsw)
		return HAL_FAILURE;

	//Check if the port does exist and is really attached to the dpid
	if( !lsw->logical_ports[port_num].port || lsw->logical_ports[port_num].attachment_state != LOGICAL_PORT_STATE_ATTACHED || lsw->logical_ports[port_num].port->attached_sw->dpid != dpid)
		return HAL_FAILURE;

	return hal_driver_bring_port_down(lsw->logical_ports[port_num].port->name);
}

/**
 * @brief Retrieve a snapshot of the monitoring state. If rev is 0, or the current monitoring 
 * has changed (monitoring->rev != rev), a new snapshot of the monitoring state is made. Warning: this 
 * is expensive.
 * @ingroup driver_management
 *
 * @param rev Last seen revision. Set to 0 to always get a new snapshot 
 * @return A snapshot of the monitoring state that MUST be destroyed using monitoring_destroy_snapshot() or NULL if there have been no changes (same rev)
 */ 
monitoring_snapshot_state_t* hal_driver_get_monitoring_snapshot(uint64_t rev){

	monitoring_state_t* mon = physical_switch_get_monitoring();

	if( rev == 0 || monitoring_has_changed(mon, &rev) ) 
		return monitoring_get_snapshot(mon);

	return NULL;
}

/**
 * @brief get a list of available matching algorithms
 * @ingroup driver_management
 *
 * @param of_version
 * @param name_list
 * @param count
 * @return
 */
hal_result_t hal_driver_list_matching_algorithms(of_version_t of_version, const char * const** name_list, int *count){
	return (hal_result_t)of_get_switch_matching_algorithms(of_version, name_list, count);
}

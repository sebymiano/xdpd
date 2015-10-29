/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _BACKGROUND_TASK_MANAGER_
#define _BACKGROUND_TASK_MANAGER_

#include <pthread.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl_datapath.h>

#define MAX_NL_MESSAGE_HEADER 4096
#define MAX_EPOLL_EVENTS 128


/*time between timers being checked, define together with OF12_TIMER_SLOT_MS*/
#define LSW_TIMER_SLOT_MS 200
#define LSW_TIMER_BUFFER_POOL_MS 5000 /*time to check for expired buffers in the pool*/

//C++ extern C
ROFL_BEGIN_DECLS

/**
 * launches the main thread
 */

void check_port_status(void);

void update_misc_stats(void);

int process_timeouts(void);

rofl_result_t launch_background_tasks_manager(void);

rofl_result_t stop_background_tasks_manager(void);

//C++ extern C
ROFL_END_DECLS

#endif

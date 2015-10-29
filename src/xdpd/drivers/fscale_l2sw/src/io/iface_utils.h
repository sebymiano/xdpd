/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _PLATFORM_PORT_MGMT_
#define _PLATFORM_PORT_MGMT_

#include <rofl_datapath.h>
#include <rofl/datapath/hal/driver.h>


//C++ extern C
ROFL_BEGIN_DECLS

/*
 * @name update_port_status
 */
rofl_result_t update_port_status(char * name, bool up);


//C++ extern C
ROFL_END_DECLS

#endif //PLATFORM_PORT_MGMT

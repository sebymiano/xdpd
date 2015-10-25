/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef _VTSS_L2SW_H_
#define _VTSS_L2SW_H_

#include <rofl_datapath.h>

#define FSCALE_L2SW_INTERFACE_BASE_NAME "fsw"
#define FSCALE_L2SW_INTERFACE_NAME_LEN 5 //fswX\0

ROFL_BEGIN_DECLS

rofl_result_t vtss_l2sw_init(void);

rofl_result_t vtss_l2sw_destroy(void);

ROFL_END_DECLS


#endif //_VTSS_L2SW_H_

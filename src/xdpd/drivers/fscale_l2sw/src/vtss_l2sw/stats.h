/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _VTSS_L2SW_STATS_H_
#define _VTSS_L2SW_STATS_H_

#include <inttypes.h>
#include "../util/compiler_assert.h"
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>

//C++ extern C
ROFL_BEGIN_DECLS

rofl_result_t vtss_l2sw_update_entry_stats(of1x_flow_entry_t* entry);

void dump_l2sw_flow_acl_entries();

//C++ extern C
ROFL_END_DECLS

#endif //NETFPGA_STATS

#ifndef _VTSS_L2SW_FLOW_ENTRY_H_
#define _VTSS_L2SW_FLOW_ENTRY_H_

#include <inttypes.h>
#include <stdbool.h>
#include <rofl_datapath.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>

#include "../util/compiler_assert.h"
#include <vtss_api/vtss_api.h>
#include <vtss_api/vtss_security_api.h>

//C++ extern C
ROFL_BEGIN_DECLS

vtss_rc vtss_l2sw_generate_acl_entry_matches(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry);

vtss_rc vtss_l2sw_generate_acl_entry_actions(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry);

//C++ extern C
ROFL_END_DECLS


#endif //_VTSS_L2SW_FLOW_ENTRY_H_

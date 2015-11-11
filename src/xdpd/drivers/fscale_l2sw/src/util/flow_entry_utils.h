#ifndef _VTSS_L2SW_FLOW_ENTRY_UTILS_H_
#define _VTSS_L2SW_FLOW_ENTRY_UTILS_H_

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

bool add_or_update_match_in_port(vtss_ace_t* acl_entry, of1x_match_type_t type, of1x_match_t* match);

bool add_or_update_eth_dst(vtss_ace_t* acl_entry, of1x_match_type_t type, of1x_match_t* match);

bool add_or_update_eth_src(vtss_ace_t* acl_entry, of1x_match_type_t type, of1x_match_t* match);

//C++ extern C
ROFL_END_DECLS

#endif //_VTSS_L2SW_FLOW_ENTRY_UTILS_H_

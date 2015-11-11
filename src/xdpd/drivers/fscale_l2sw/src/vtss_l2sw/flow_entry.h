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

extern int aclId;

#define ACL_INVALID_ID -1

//Entry
typedef struct vtss_l2sw_flow_entry{
	//ACL ID
	unsigned int acl_id;
}vtss_l2sw_flow_entry_t;

//C++ extern C
ROFL_BEGIN_DECLS

vtss_l2sw_flow_entry_t* vtss_l2sw_init_vtss_flow_entry(void);

void vtss_l2sw_destroy_vtss_flow_entry(vtss_l2sw_flow_entry_t* entry);

vtss_rc vtss_l2sw_generate_acl_entry_matches(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry);

vtss_rc vtss_l2sw_generate_acl_entry_actions(vtss_ace_t* acl_entry, of1x_flow_entry_t* of1x_entry);

//C++ extern C
ROFL_END_DECLS

#endif //_VTSS_L2SW_FLOW_ENTRY_H_

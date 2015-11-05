#ifndef L2SWITCH_UTILS_H
#define L2SWITCH_UTILS_H

#include <rofl_datapath.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_action.h>

//Extern C
ROFL_BEGIN_DECLS

bool is_l2_entry(of1x_flow_entry_t *entry);

bool actions_are_only_l2(of1x_action_group_t* action_group);

//Extern C
ROFL_END_DECLS


#endif /* L2SWITCH_UTILS_H */

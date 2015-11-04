#ifndef L2SWITCH_UTILS_H
#define L2SWITCH_UTILS_H

#include <rofl_datapath.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h>

//Extern C
ROFL_BEGIN_DECLS

bool is_l2_entry(const of1x_flow_entry_t * const entry);

//Extern C
ROFL_END_DECLS


#endif /* L2SWITCH_UTILS_H */

#include "l2switch_utils.h"
#include <stdio.h>
#include <unistd.h>

//Prototypes
#include <rofl/common/utils/c_logger.h>
#include <rofl/datapath/pipeline/platform/memory.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_match.h>
#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.h>
#include <rofl/datapath/pipeline/common/datapacket.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/hal/cmm.h>

bool is_l2_entry(of1x_flow_entry_t * entry) {
	bitmap128_t no_l2_bitmap;
	bitmap128_t and_matches_bitmap;
	bitmap128_t and_wildcard_bitmap;

	bitmap128_set_all(&no_l2_bitmap);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_IN_PORT);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_IN_PHY_PORT);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_DST);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_SRC);
	//bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_VLAN_VID);

	and_matches_bitmap = bitmap128_and(&entry->matches.match_bm, &no_l2_bitmap);
	and_wildcard_bitmap = bitmap128_and(&entry->matches.wildcard_bm,
			&no_l2_bitmap);
	if (bitmap128_is_empty(&and_matches_bitmap)
			&& bitmap128_is_empty(&and_wildcard_bitmap)) {
		//TODO: Here I'm sure that there are no entries different from l2, but I'm not sure that there are still l2 entries

		/*if(bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_IN_PORT)
		 || bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_IN_PHY_PORT)
		 || bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_ETH_DST)
		 || bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_ETH_SRC)
		 || bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_VLAN_VID)){
		 return true;
		 }*/

		if (actions_are_only_l2(entry->inst_grp.instructions,
				entry->inst_grp.num_of_instructions)) {
			return true;

		}
	}

	return false;
}

bool actions_are_only_l2(of1x_instruction_t* instruction_list, int num_instructions){
	int i;
	bitmap128_t no_l2_bitmap;
	bitmap128_t and_actions_bitmap;
	of1x_instruction_t instr;

	bitmap128_set_all(&no_l2_bitmap);
	bitmap128_unset(&no_l2_bitmap, OF1X_AT_OUTPUT);
	bitmap128_unset(&no_l2_bitmap, OF1X_AT_NO_ACTION);

	for(i = 0; i < num_instructions; i++){
		instr = instruction_list[i];

		and_actions_bitmap = bitmap128_and(&instr.apply_actions->bitmap, &no_l2_bitmap);
		if(!bitmap128_is_empty(&and_actions_bitmap)){
			//If this result is empty means that there are no actions not supported
			return false;
		}

		//Here I'm not sure if the instruction contains our supported action
		if(!bitmap128_is_bit_set(&instr.apply_actions->bitmap, OF1X_AT_OUTPUT)
				&& !bitmap128_is_bit_set(&instr.apply_actions->bitmap, OF1X_AT_NO_ACTION)){
			//This instruction is empty because is doesn't contain action output
			return false;
		}

	}

	return true;
}

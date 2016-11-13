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
#include "../config.h"

bool is_l2_entry(of1x_flow_entry_t * entry) {
	bitmap128_t no_l2_bitmap;
	bitmap128_t and_matches_bitmap;
	bitmap128_t and_wildcard_bitmap;

	bitmap128_set_all(&no_l2_bitmap);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_IN_PORT);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_IN_PHY_PORT);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_DST);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_SRC);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_TYPE);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_VLAN_VID);

	ROFL_INFO("["DRIVER_NAME"] %s(): set bitmap for no l2 matches\n", __FUNCTION__);

	and_matches_bitmap = bitmap128_and(&entry->matches.match_bm, &no_l2_bitmap);
	and_wildcard_bitmap = bitmap128_and(&entry->matches.wildcard_bm, &no_l2_bitmap);
	if (!bitmap128_is_empty(&and_matches_bitmap) || !bitmap128_is_empty(&and_wildcard_bitmap)) {
		//TODO: Here I'm sure that there are no entries different from l2, but I'm not sure that there are still l2 entries

		ROFL_INFO("["DRIVER_NAME"] %s(): there are matches different from l2\n", __FUNCTION__);
		return false;
	}

	/*if (!bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_IN_PORT)
	 && !bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_IN_PHY_PORT)
	 && !bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_ETH_DST)
	 && !bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_ETH_SRC)) {
	 ROFL_INFO("["DRIVER_NAME"] %s(): there aren't matches different from l2 but there are no l2 matches also\n",
	 __FUNCTION__);
	 //return false;
	 }*/

	if (!actions_are_only_l2(entry->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions)) {
		ROFL_INFO("["DRIVER_NAME"] %s(): there are some actions not compatible with this driver\n", __FUNCTION__);
		return false;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): fine :) all actions and entries are l2\n", __FUNCTION__);

	return true;
}

bool is_valid_entry(of1x_flow_entry_t * entry) {
	bitmap128_t no_l2_bitmap;
	bitmap128_t and_matches_bitmap;
	bitmap128_t and_wildcard_bitmap;

	bitmap128_set_all(&no_l2_bitmap);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_IN_PORT);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_IN_PHY_PORT);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_DST);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_SRC);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_TYPE);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_VLAN_VID);

	ROFL_INFO("["DRIVER_NAME"] %s(): checking if the entry is valid for the offload\n", __FUNCTION__);

	and_matches_bitmap = bitmap128_and(&entry->matches.match_bm, &no_l2_bitmap);
	and_wildcard_bitmap = bitmap128_and(&entry->matches.wildcard_bm, &no_l2_bitmap);
	if (!bitmap128_is_empty(&and_matches_bitmap) || !bitmap128_is_empty(&and_wildcard_bitmap)) {
		//TODO: Here I'm sure that there are no entries different from l2, but I'm not sure that there are still l2 entries

		ROFL_INFO("["DRIVER_NAME"] %s(): there are matches different from l2\n", __FUNCTION__);
		return false;
	}

	if (!actions_are_only_l2(entry->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions)) {
		ROFL_INFO("["DRIVER_NAME"] %s(): there are some actions not compatible with this driver\n", __FUNCTION__);
		return false;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): Great!! All actions and entries are l2. Let's offload them :)\n", __FUNCTION__);

	return true;
}

bool actions_are_only_l2(of1x_action_group_t* action_group) {
	bitmap128_t no_l2_bitmap;
	bitmap128_t and_actions_bitmap;

	bitmap128_set_all(&no_l2_bitmap);
	bitmap128_unset(&no_l2_bitmap, OF1X_AT_OUTPUT);
	bitmap128_unset(&no_l2_bitmap, OF1X_AT_NO_ACTION);

	ROFL_INFO("["DRIVER_NAME"] %s(): set bitmap for no l2 matches\n", __FUNCTION__);

	and_actions_bitmap = bitmap128_and(&action_group->bitmap, &no_l2_bitmap);
	if (!bitmap128_is_empty(&and_actions_bitmap)) {
		//If this result is not empty means that there are actions not supported
		ROFL_INFO("["DRIVER_NAME"] %s(): there are actions different from l2\n", __FUNCTION__);
		return false;
	}

	//Here I'm not sure if the instruction contains our supported action
	if (!bitmap128_is_bit_set(&action_group->bitmap, OF1X_AT_OUTPUT)
			&& !bitmap128_is_bit_set(&action_group->bitmap, OF1X_AT_NO_ACTION)) {
		//This instruction is empty because is doesn't contain action output
		ROFL_INFO("["DRIVER_NAME"] %s(): there aren't actions different from l2 but there are no l2 actions also\n",
				__FUNCTION__);
		return false;
	}

	return true;
}

bool is_entry_for_mac_table(of1x_flow_entry_t * entry) {
	bitmap128_t no_l2_bitmap;
	bitmap128_t and_matches_bitmap;

	bitmap128_set_all(&no_l2_bitmap);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_SRC);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_ETH_DST);
	bitmap128_unset(&no_l2_bitmap, OF1X_MATCH_VLAN_VID);

	ROFL_INFO("["DRIVER_NAME"] %s(): set bitmap for mac l2 matches\n", __FUNCTION__);

	and_matches_bitmap = bitmap128_and(&entry->matches.match_bm, &no_l2_bitmap);

	if (!bitmap128_is_empty(&and_matches_bitmap) || !bitmap128_is_empty(&entry->matches.wildcard_bm)) {
		//TODO: Here I'm sure that there are no entries different from l2, but I'm not sure that there are still l2 entries

		ROFL_INFO("["DRIVER_NAME"] %s(): there are matches different from l2 or wildcard\n", __FUNCTION__);
		return false;
	}

	if (!actions_are_only_l2(entry->inst_grp.instructions[OF1X_IT_APPLY_ACTIONS].apply_actions)) {
		ROFL_INFO("["DRIVER_NAME"] %s(): there are some actions not compatible with this driver\n", __FUNCTION__);
		return false;
	}

	if (bitmap128_is_bit_set(&entry->matches.match_bm, OF1X_MATCH_ETH_DST)) {

		ROFL_INFO(
				"["DRIVER_NAME"] %s(): fine :) all actions and entries are l2 and compatible with MAC Table Offloading\n",
				__FUNCTION__);
		return true;
	}

	ROFL_INFO("["DRIVER_NAME"] %s(): for MAC Table Offloading the entry should contains at least eth_dst \n",
			__FUNCTION__);

	return false;
}

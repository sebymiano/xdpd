#include "acl_util.h"
#include <algorithm>

namespace xdpd{
namespace fscale_l2sw {

acl_id_storage::acl_id_storage() : acl_storage(ACL_MAX_ID){
	IncGenerator g(1); //start number of ACL ID
	std::generate(acl_storage.begin(), acl_storage.end(), g);
}

int acl_id_storage::getAclID(){
	std::vector<int>::iterator it;
	int aclId;
	it = std::min_element(acl_storage.begin(), acl_storage.end());
	if(acl_storage.end() == it){
		//Empty array
		return ACL_INVALID_ID;
	}

	aclId = *it;
	//Delete element from array
	acl_storage.erase(it);
	return aclId;
}

bool acl_id_storage::releaseAclID(int id) {
	if(0 < id < ACL_MAX_ID){
		acl_storage.push_back(id);
		return true;
	}

	return false;
}

}// namespace xdpd::fscale_l2sw
}// namespace xdpd

int getAclID_C(){
	return xdpd::fscale_l2sw::acl_id_storage::get_instance().getAclID();
}

bool releaseAclID_C(int id){
	return xdpd::fscale_l2sw::acl_id_storage::get_instance().releaseAclID(id);
}

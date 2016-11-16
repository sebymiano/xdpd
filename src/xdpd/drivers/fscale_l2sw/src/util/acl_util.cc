#include "acl_util.h"
#include "acl_id_storage.h"

int getAclID_C(){
	return xdpd::fscale_l2sw::acl_id_storage::get_instance().getAclID();
}

bool releaseAclID_C(int id){
	return xdpd::fscale_l2sw::acl_id_storage::get_instance().releaseAclID(id);
}

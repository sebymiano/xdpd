#ifndef _ACL_UTIL_
#define _ACL_UTIL_

#include <stdint.h>
#include <rofl_datapath.h>
#include <vector>
#include <numeric>

#define ACL_INVALID_ID -1
#define ACL_MAX_ID 4096

namespace xdpd {
namespace fscale_l2sw {

class acl_id_storage {

private:
	std::vector<int> acl_storage;

	struct IncGenerator {
	    int current_;
	    IncGenerator (int start) : current_(start) {}
	    int operator() () { return current_++; }
	};

    // Private constructor, singleton instance
	acl_id_storage();
	acl_id_storage(const acl_id_storage&);
    void operator=(const acl_id_storage&);

public:
	static acl_id_storage& get_instance()
        {
		static acl_id_storage instance;
		return instance;
	}
	int getAclID();
	bool releaseAclID(int id);
};

}// namespace xdpd::fscale_l2sw
}// namespace xdpd

//C++ extern C
ROFL_BEGIN_DECLS

int getAclID_C();
bool releaseAclID_C(int id);

//C++ extern C
ROFL_END_DECLS

#endif //_ACL_UTIL_


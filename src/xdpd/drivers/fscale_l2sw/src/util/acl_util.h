#ifndef _ACL_UTIL_
#define _ACL_UTIL_

#include <stdint.h>
#include <rofl_datapath.h>

//C++ extern C
ROFL_BEGIN_DECLS

int getAclID_C();
bool releaseAclID_C(int id);

//C++ extern C
ROFL_END_DECLS

#endif //_ACL_UTIL_


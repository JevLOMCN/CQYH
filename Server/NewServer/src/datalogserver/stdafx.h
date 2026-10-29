#ifndef _STD_AFX_H_
#define _STD_AFX_H_

#include <winsock2.h>
#include <windows.h>
#include "std_type.h"

#include "baselib/asm.h"
#define _SCL_SECURE_INVALID_PARAMETER(expr) INT3()
#include "process_error.h"
#include "string_ex.h"
#include "error_def.h"
#include "gpkitsvr.h"

#include "linux_compatible.h"

#include <list>
#include <map>
#include <set>
#include <stack>
#include "read_data_helper.h"
#include "baselib/simple_list.h"

#endif  // _STD_AFX_H_
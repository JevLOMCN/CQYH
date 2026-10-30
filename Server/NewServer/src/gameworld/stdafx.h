#ifndef _ATD_AFX_H_
#define _ATD_AFX_H_

#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include "std_type.h"

#ifndef _ROBOT
    #include "gpkitsvr.h"
#endif

#include "process_error.h"
#include "log.h"
#include "string_ex.h"
#include "math_ex.h"
#include "linux_compatible.h"
#include "baselib/asm.h"
#include "baselib/simple_list.h"

#define _SCL_SECURE_INVALID_PARAMETER(expr) INT3()
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <list>
#include <set>
#include <stack>
#include <string.h>

#endif	// _ATD_AFX_H_

#ifndef _STDAFX_H_
#define _STDAFX_H_

#define _CRT_RAND_S
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "std_type.h"

#include "asm.h"
#define _SCL_SECURE_INVALID_PARAMETER(expr) INT3()

#include <list>
#include <map>
#include <set>
#include <vector>

#include "process_error.h"
#include "string_ex.h"

#endif // _STDAFX_H_
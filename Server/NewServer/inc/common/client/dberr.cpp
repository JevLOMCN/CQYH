// this cpp source file is generated automatically by tools, please do not manually modify.
#include "stdafx.h"
#include "dberr.h"

// @brief
// You may see this phenomenon :
// variables with "STR_" as their prifix, maybe have a suffix
// the suffix means which format output symbols have appeared in function like printf
// Sample :  "%s" turns to be "_S"
namespace  ERR_CREATEROLE_DB
{
    const INT32 ID_NOERROR = 0;
    const TCHAR* STR_NOERROR = _SDTSource2T("GBK", "Operation successful");

    const INT32 ID_TOOMUCHROLE = 1;
    const TCHAR* STR_TOOMUCHROLE = _SDTSource2T("GBK", "Too many characters created");

    const INT32 ID_PARAMERROR = 2;
    const TCHAR* STR_PARAMERROR = _SDTSource2T("GBK", "Incorrect parameter");

    const INT32 ID_UNKNOWERROR = 3;
    const TCHAR* STR_UNKNOWERROR = _SDTSource2T("GBK", "Duplicate character name or database error");
}


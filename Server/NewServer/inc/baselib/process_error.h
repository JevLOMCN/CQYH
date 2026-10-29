#ifndef _PROCESS_ERROR_H_
#define _PROCESS_ERROR_H_

#define PROCESS_ERROR(__condition__)								\
	do																\
	{																\
		if (__condition__)											\
		{															\
																	\
		}															\
		else														\
		{															\
			goto Exit0;												\
		}															\
	} while (0)

#include "log.h"
#include "asm.h"

//#if defined(_DEBUG) & defined(WIN32)
//    #define _CRTDBG_MAP_ALLOC
//    #include <stdlib.h>
//    #include <crtdbg.h>
//    #include <xtree>
//    #define _DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//    #define new _DEBUG_NEW
//#endif  // _DEBUG & WIN32

#define PROCESS_SUCCESS(__condition__)								\
	do																\
	{																\
		if (__condition__)											\
		{															\
			goto Exit1;												\
		}															\
	} while (0)

#define LOG_PROCESS_ERROR(__condition__)							\
	do																\
	{																\
		if (__condition__)											\
		{															\
																	\
		}															\
		else														\
		{															\
            ERR("LOG_PROCESS_ERROR(%s) at line %d in function %s() file \"%s\"", \
                #__condition__, __LINE__, __FUNCTION__, __FILE__);                  \
			goto Exit0;												\
		}															\
	} while (0)

#define LOG_CHECK_ERROR(__condition__)								\
	do																\
	{																\
		if (__condition__)											\
		{															\
																	\
		}															\
		else														\
		{															\
            ERR("LOG_CHECK_ERROR(%s) at line %d in function %s() file \"%s\"", \
                #__condition__, __LINE__, __FUNCTION__, __FILE__);                  \
		}															\
	} while (0)

#define LOG_CHECK_ERROR_BREAK(__condition__)						\
	do																\
	{																\
		if (!(__condition__))										\
		{															\
            ERR("LOG_CHECK_ERROR(%s) at line %d in function %s() file \"%s\"", \
                #__condition__, __LINE__, __FUNCTION__, __FILE__);                  \
            INT3();                                                 \
		}															\
	} while (0)

#define PROCESS_ERROR_RET_CODE(__condition__, __result__)			\
	do																\
	{																\
		if (__condition__)											\
		{															\
																	\
		}															\
		else														\
		{															\
			nResult = __result__;									\
			goto Exit0;												\
		}															\
	} while (0)

#define LOG_PROCESS_ERROR_RET_CODE(__condition__, __result__)		\
	do																\
	{																\
		if (__condition__)											\
		{															\
																	\
		}															\
		else														\
		{															\
			nResult = __result__;									\
            ERR("LOG_PROCESS_ERROR_RET_CODE(%s) at line %d in function %s() file \"%s\"", \
                #__condition__, __LINE__, __FUNCTION__, __FILE__);                  \
			goto Exit0;												\
		}															\
	} while (0)

#define SAFE_FREE(__pointer__)				    \
	do											\
	{											\
		if (__pointer__)						\
		{										\
			free(__pointer__);				    \
			(__pointer__) = NULL;				\
		}										\
	} while (0);

#define SAFE_DELETE(__pointer__)				\
	do											\
	{											\
		if (__pointer__)						\
		{										\
			delete (__pointer__);				\
			(__pointer__) = NULL;				\
		}										\
	} while (0);

#define SAFE_DELETE_ARRAY(__pointer__)			\
	do											\
	{											\
		if (__pointer__)						\
		{										\
			delete [] (__pointer__);			\
			(__pointer__) = NULL;				\
		}										\
	} while (0);


#ifndef ASSERT
    #define ASSERT LOG_CHECK_ERROR
#endif  // ASSERT

#endif	// _PROCESS_ERROR_H_

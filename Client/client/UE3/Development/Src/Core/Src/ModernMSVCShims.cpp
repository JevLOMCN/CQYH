/*=============================================================================
	ModernMSVCShims.cpp: Link-time shims that let the prebuilt Visual Studio 2010 era third party libraries link
	against the Visual Studio 2015+ runtime.

	The old libraries import a few C/C++ runtime entry points that no longer exist under those names. Each is
	redirected (via /alternatename) to an equivalent implemented here. Win32 and Win64.
=============================================================================*/

#include "CorePrivate.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && (defined(_M_IX86) || defined(_M_X64))

#include <stdio.h>
#include <new>

#if defined(_M_IX86)
#define UE3_SHIM_EDX , void* /*Unused EDX*/
#else
#define UE3_SHIM_EDX
#endif
#include <exception>

/*-----------------------------------------------------------------------------
	__iob_func: replaced by __acrt_iob_func in the universal CRT.
-----------------------------------------------------------------------------*/

namespace
{
	/** Code built with the old headers addresses stdout/stderr as __iob_func() + n, with sizeof(FILE) == 32 there. */
	struct FOldStyleFileSlot
	{
		FILE	File;
		char	Padding[(sizeof(void*) == 8 ? 48 : 32) - sizeof(FILE)];
	};

	FOldStyleFileSlot GOldStyleIob[3];

	FILE* __cdecl ShimIobFunc()
	{
		static bool bInitialized = false;
		if( !bInitialized )
		{
			for( unsigned int Index = 0; Index < 3; Index++ )
			{
				GOldStyleIob[Index].File = *__acrt_iob_func( Index );
			}
			bInitialized = true;
		}
		return &GOldStyleIob[0].File;
	}
}

extern "C" void* ue3_imp_iob_func = (void*)&ShimIobFunc;
#if defined(_M_X64)
#pragma comment(linker, "/alternatename:__imp___iob_func=ue3_imp_iob_func")
#else
#pragma comment(linker, "/alternatename:__imp____iob_func=_ue3_imp_iob_func")
#endif

/*-----------------------------------------------------------------------------
	std::exception(const char* const&): Microsoft extension that the current STL no longer exports.
-----------------------------------------------------------------------------*/

namespace
{
	std::exception* __fastcall ShimExceptionConstruct( std::exception* Self UE3_SHIM_EDX, const char* const* Message )
	{
		return ::new( (void*)Self ) std::exception( *Message );
	}
}

extern "C" void* ue3_imp_exception_ctor = (void*)&ShimExceptionConstruct;
#if defined(_M_X64)
#pragma comment(linker, "/alternatename:__imp_??0exception@std@@QEAA@AEBQEBD@Z=ue3_imp_exception_ctor")
#else
#pragma comment(linker, "/alternatename:__imp_??0exception@std@@QAE@ABQBD@Z=_ue3_imp_exception_ctor")
#endif

/*-----------------------------------------------------------------------------
	std::_Container_base12: checked iterator bookkeeping, used by the debug builds (_ITERATOR_DEBUG_LEVEL 2) of the old libraries.
-----------------------------------------------------------------------------*/

#if _ITERATOR_DEBUG_LEVEL == 2

#include <xmemory>

namespace
{
	void __fastcall ShimContainerOrphanAll( std::_Container_base12* Self UE3_SHIM_EDX )
	{
		Self->_Orphan_all();
	}

	void __fastcall ShimContainerDestruct( std::_Container_base12* Self UE3_SHIM_EDX )
	{
		Self->_Orphan_all();
	}

	std::_Iterator_base12** __fastcall ShimContainerGetPFirst( const std::_Container_base12* Self UE3_SHIM_EDX )
	{
		return Self->_Myproxy == NULL ? NULL : &Self->_Myproxy->_Myfirstiter;
	}
}

extern "C" void* ue3_imp_container_orphan_all = (void*)&ShimContainerOrphanAll;
extern "C" void* ue3_imp_container_dtor = (void*)&ShimContainerDestruct;
extern "C" void* ue3_imp_container_getpfirst = (void*)&ShimContainerGetPFirst;
#if defined(_M_X64)
#pragma comment(linker, "/alternatename:__imp_?_Orphan_all@_Container_base12@std@@QEAAXXZ=ue3_imp_container_orphan_all")
#pragma comment(linker, "/alternatename:__imp_??1_Container_base12@std@@QEAA@XZ=ue3_imp_container_dtor")
#pragma comment(linker, "/alternatename:__imp_?_Getpfirst@_Container_base12@std@@QEBAPEAPEAU_Iterator_base12@2@XZ=ue3_imp_container_getpfirst")
#else
#pragma comment(linker, "/alternatename:__imp_?_Orphan_all@_Container_base12@std@@QAEXXZ=_ue3_imp_container_orphan_all")
#pragma comment(linker, "/alternatename:__imp_??1_Container_base12@std@@QAE@XZ=_ue3_imp_container_dtor")
#pragma comment(linker, "/alternatename:__imp_?_Getpfirst@_Container_base12@std@@QBEPAPAU_Iterator_base12@2@XZ=_ue3_imp_container_getpfirst")

#endif // _M_X64
#endif // _ITERATOR_DEBUG_LEVEL == 2

#endif // modern MSVC on Win32 or Win64

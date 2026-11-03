/*=============================================================================
	WxImageShims.cpp: The prebuilt wxWidgets DLL was built before C++11 and has no wxImage move constructor or move
	assignment, but code compiled with a newer compiler asks for them (implicitly declared, imported from the DLL).
	Both are redirected here to the copy operations the DLL does provide. Win32 and Win64.
=============================================================================*/

#include "UnrealEd.h"

#if UE3_WX32
FString::FString(const wxString& In) : FString(In.wc_str()) {}
FString::FString(const wxCStrData& In) : FString(In.AsWChar()) {}
FString& FString::operator=(const wxString& In) { return *this = In.wc_str(); }
FString& FString::operator=(const wxCStrData& In) { return *this = In.AsWChar(); }
FString& FString::operator+=(const wxString& In) { return *this += In.wc_str(); }
FString& FString::operator+=(const wxCStrData& In) { return *this += In.AsWChar(); }
INT FString::InStr(const wxString& In, UBOOL bFromEnd, UBOOL bIgnoreCase, INT Start) const { return InStr(In.wc_str(), bFromEnd, bIgnoreCase, Start); }
INT FString::InStr(const wxCStrData& In, UBOOL bFromEnd, UBOOL bIgnoreCase, INT Start) const { return InStr(In.AsWChar(), bFromEnd, bIgnoreCase, Start); }
FFilename::FFilename(const wxString& In) : FString(In) {}
FFilename::FFilename(const wxCStrData& In) : FString(In) {}
#endif

#if !UE3_WX32 && defined(_MSC_VER) && (_MSC_VER >= 1900) && (defined(_M_IX86) || defined(_M_X64))

#include <new>

#if defined(_M_IX86)
#define UE3_SHIM_EDX , void* /*Unused EDX*/
#else
#define UE3_SHIM_EDX
#endif

namespace
{
	wxImage* __fastcall ShimWxImageMoveConstruct( wxImage* Self UE3_SHIM_EDX, wxImage* Other )
	{
		return ::new( (void*)Self ) wxImage( *(const wxImage*)Other );
	}

	wxImage* __fastcall ShimWxImageMoveAssign( wxImage* Self UE3_SHIM_EDX, wxImage* Other )
	{
		*Self = *(const wxImage*)Other;
		return Self;
	}
}

extern "C" void* ue3_imp_wximage_move_ctor = (void*)&ShimWxImageMoveConstruct;
extern "C" void* ue3_imp_wximage_move_assign = (void*)&ShimWxImageMoveAssign;
#if defined(_M_X64)
#pragma comment(linker, "/alternatename:__imp_??0wxImage@@QEAA@$$QEAV0@@Z=ue3_imp_wximage_move_ctor")
#pragma comment(linker, "/alternatename:__imp_??4wxImage@@QEAAAEAV0@$$QEAV0@@Z=ue3_imp_wximage_move_assign")
#else
#pragma comment(linker, "/alternatename:__imp_??0wxImage@@QAE@$$QAV0@@Z=_ue3_imp_wximage_move_ctor")
#pragma comment(linker, "/alternatename:__imp_??4wxImage@@QAEAAV0@$$QAV0@@Z=_ue3_imp_wximage_move_assign")

#endif // _M_X64
#endif // modern MSVC on Win32 or Win64

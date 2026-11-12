// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "../Core/Inc/ForceTry.h"

#include "MMOAny.h"
#include "MMOSingleton.h"
#include "MMOLogicCommand.h"
//#include "IUDKActor.h"

#include "MMOPrerequisites.h"

#include "scene_kismet_def.h"

#include "MMOGameClasses.h"
#include "MMOGameUIClasses.h"
#include "MMOFacade.h"


//macro to release self-allocated memory hemu@snda
#define SAFEDELETE(a)\
{\
	if( a )\
  {\
  delete a;\
  a = NULL;\
  }\
}

//macro to release self-allocated memory arrays hemu@snda
#define SAFEDELETEARRAY(a)\
{\
	if( a )\
  {\
  delete[] a;\
  a = NULL;\
  }\
}

//#include "targetver.h"

//#include <stdio.h>
//#include <tchar.h>
//#include <list>
//#include <string>
////#include <WTypes.h>
////#include <WinDef.h>
//#include <limits>
//#include <deque>
//#include <cassert>
//#include <map>

// TODO: reference additional headers your program requires here

#define M_TEMP_ENABLE_UI_CODE 0
#define M_TEMP_ENABLE_UI_Skill_CODE 1

#include "CommonGameEvents.h"

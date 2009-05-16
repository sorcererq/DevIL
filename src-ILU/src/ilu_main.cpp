//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 05/15/2009
//
// Filename: src-ILU/src/ilu_main.cpp
//
// Description: Startup functions
//
//-----------------------------------------------------------------------------


#include "ilu_internal.h"
#include "ilu_states.h"
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif//_WIN32

#ifdef _WIN32
	#if (defined(IL_USE_PRAGMA_LIBS))
		#if defined(_MSC_VER) || defined(__BORLANDC__)
			#pragma comment(lib, "DevIL.lib")
		#endif
	#endif
#endif

/* Only needed for MSVC++ unless extended to actually do something =) */
#if defined(_WIN32) && defined(_MSC_VER)

#ifndef IL_STATIC_LIB
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	hModule;  ul_reason_for_call;  lpReserved;
	
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		//iluInit();
	}

	return TRUE;
}

#endif
#endif

void ILAPIENTRY iluInit()
{
	// Used mostly for rotations
	//IL_PI = 4 * atan(1);		// precomputed value of pi
	//IL_DEGCONV = IL_PI / 180;	// division is slow on some computers

	iluSetLanguage(ILU_ENGLISH);
	return;
}


//#ifndef _WIN32_WCE
ILimage* ILAPIENTRY iluLoadImage(ILconst_string FileName, ILstate *State)
{
	ILimage *Image = ilGenImage(State);
	if (Image == NULL)
		return NULL;
	if (!ilLoadImage(Image, FileName, State)) {
		ilCloseImage(Image);
		return NULL;
	}
	return Image;
}
//#endif//_WIN32_WCE

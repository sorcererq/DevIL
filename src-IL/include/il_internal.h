//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2008 by Denton Woods
// Last modified: 12/14/2008
//
// Filename: src-IL/include/il_internal.h
//
// Description: Internal stuff for DevIL
//
//-----------------------------------------------------------------------------
#ifndef INTERNAL_H
#define INTERNAL_H
#define _IL_BUILD_LIBRARY


// Local headers
/*#if (defined(_WIN32) || defined(_WIN64)) && !defined(HAVE_CONFIG_H)
	#define HAVE_CONFIG_H
#endif*/
#ifdef HAVE_CONFIG_H //if we use autotools, we have HAVE_CONFIG_H defined and we have to look for it like that
#include <config.h>
#else // if we don't use autotools, we have to point to (possibly different) config.h than in the opposite case
#include <IL/config.h>
#endif

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <IL/il.h>
#include <IL/devil_internal_exports.h>
#include "il_files.h"
#include "il_endian.h"

// Windows-specific
#ifdef _WIN32
	#ifdef _MSC_VER
		#if _MSC_VER > 1000
			#pragma once
			#pragma intrinsic(memcpy)
			#pragma intrinsic(memset)
			#pragma intrinsic(strcmp)
			#pragma intrinsic(strlen)
			#pragma intrinsic(strcpy)
			
			#if _MSC_VER >= 1300
				#pragma warning(disable : 4996)  // MSVC++ 8/9 deprecation warnings
			#endif
			//pragma comment(linker, "/NODEFAULTLIB:libc")
			//pragma comment(linker, "/NODEFAULTLIB:libcd")
			//pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
			//#ifdef _DEBUG
			//	pragma comment(linker, "/NODEFAULTLIB:libcmtd")
			//	pragma comment(linker, "/NODEFAULTLIB:msvcrt.lib")
			//#endif // _DEBUG
		#endif // _MSC_VER > 1000
	#endif
	#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
#endif
// Windows has a TEXT macro defined in WinNT.h that makes string Unicode if UNICODE is defined.
/*#ifndef _WIN32
	#define IL_TEXT(s) s
#endif*/
/*#ifdef _WIN32_WCE
	#define IL_TEXT(s) ((char*)TEXT(s))
#elif _WIN32
	#define IL_TEXT(s) (s)
#else
	#define IL_TEXT(s) (s)
	#define TEXT(s) (s)
#endif*/

#ifdef _UNICODE
	#define IL_TEXT(s) L##s
	#ifndef _WIN32  // At least in Linux, fopen works fine, and wcsicmp is not defined.
		#define wcsicmp wcsncasecmp
		#define _wcsicmp wcsncasecmp
		#define _wfopen fopen
	#endif
	#define iStrCpy wcscpy
#else
	#define IL_TEXT(s) (s)
	#define iStrCpy strcpy
#endif

#ifdef IL_INLINE_ASM
	#if (defined (_MSC_VER) && defined(_WIN32))  // MSVC++ only
		#define USE_WIN32_ASM
	#endif

	#ifdef _WIN64
		#undef USE_WIN32_ASM
	//@TODO: Windows 64 compiler cannot use inline ASM, so we need to
	//  generate some MASM code at some point.
	#endif
#endif
extern ILimage *iCurImage;
#define BIT_0	0x00000001
#define BIT_1	0x00000002
#define BIT_2	0x00000004
#define BIT_3	0x00000008
#define BIT_4	0x00000010
#define BIT_5	0x00000020
#define BIT_6	0x00000040
#define BIT_7	0x00000080
#define BIT_8	0x00000100
#define BIT_9	0x00000200
#define BIT_10	0x00000400
#define BIT_11	0x00000800
#define BIT_12	0x00001000
#define BIT_13	0x00002000
#define BIT_14	0x00004000
#define BIT_15	0x00008000
#define BIT_16	0x00010000
#define BIT_17	0x00020000
#define BIT_18	0x00040000
#define BIT_19	0x00080000
#define BIT_20	0x00100000
#define BIT_21	0x00200000
#define BIT_22	0x00400000
#define BIT_23	0x00800000
#define BIT_24	0x01000000
#define BIT_25	0x02000000
#define BIT_26	0x04000000
#define BIT_27	0x08000000
#define BIT_28	0x10000000
#define BIT_29	0x20000000
#define BIT_30	0x40000000
#define BIT_31	0x80000000
#define NUL '\0'  // Easier to type and ?portable?
#if !_WIN32 || _WIN32_WCE
	int stricmp(const char *src1, const char *src2);
	int strnicmp(const char *src1, const char *src2, size_t max);
#endif//_WIN32
int iStrCmp(ILconst_string src1, ILconst_string src2);

//
// Some math functions
//
// A fast integer squareroot, completely accurate for x < 289.
// Taken from http://atoms.org.uk/sqrt/
// There is also a version that is accurate for all integers
// < 2^31, if we should need it
int iSqrt(int x);
//
// Useful miscellaneous functions
//
ILboolean	iCheckExtension(ILconst_string Arg, ILconst_string Ext);
ILbyte*		iFgets(char *buffer, ILuint maxlen);
ILboolean	iFileExists(ILconst_string FileName);
ILstring	iGetExtension(ILconst_string FileName);
ILstring	ilStrDup(ILconst_string Str);
ILuint		ilStrLen(ILconst_string Str);
ILuint		ilCharStrLen(const char *Str);
// Miscellaneous functions
void					ilDefaultStates(void);
ILenum					iGetHint(ILenum Target);
ILint					iGetInt(ILenum Mode);
void					ilRemoveRegistered(void);
ILAPI void ILAPIENTRY	ilSetCurImage(ILimage *Image);
//
// Rle compression
//
#define		IL_TGACOMP 0x01
#define		IL_PCXCOMP 0x02
#define		IL_SGICOMP 0x03
#define     IL_BMPCOMP 0x04
ILboolean	ilRleCompressLine(ILubyte *ScanLine, ILuint Width, ILubyte Bpp, ILubyte *Dest, ILuint *DestWidth, ILenum CompressMode);
ILuint		ilRleCompress(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp, ILubyte *Dest, ILenum CompressMode, ILuint *ScanTable);
void		iSetImage0(void);
// DXTC compression
ILuint		ilNVidiaCompressDXTFile(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILenum DxtType);
// Conversion functions
ILboolean	ilAddAlpha(void);
ILboolean	ilAddAlphaKey(ILimage *Image);
ILboolean	iFastConvert(ILenum DestFormat);
ILboolean	ilFixImage(void);
ILboolean	ilRemoveAlpha(void);
ILboolean	ilSwapColours(void);
// Miscellaneous functions
char*		iGetString(ILenum StringName);  // Internal version of ilGetString
// Library usage
#if _MSC_VER && !_WIN32_WCE
	#ifndef IL_NO_JPG
		#ifdef IL_USE_IJL
			//pragma comment(lib, "ijl15.lib")
		#else
			#ifndef IL_DEBUG
				//pragma comment(lib, "libjpeg.lib")
			#else
				//pragma comment(lib, "debug/libjpeg.lib")
			#endif
		#endif
	#endif
	#ifndef IL_NO_MNG
		#ifndef IL_DEBUG
			//pragma comment(lib, "libmng.lib")
			//pragma comment(lib, "libjpeg.lib")  // For JNG support.
		#else
			//pragma comment(lib, "debug/libmng.lib")
			//pragma comment(lib, "debug/libjpeg.lib")  // For JNG support.
		#endif
	#endif
	#ifndef IL_NO_PNG
		#ifndef IL_DEBUG
			//pragma comment(lib, "libpng.lib")
		#else
			//pragma comment(lib, "debug/libpng.lib")
		#endif
	#endif
	#ifndef IL_NO_TIF
		#ifndef IL_DEBUG
			//pragma comment(lib, "libtiff.lib")
		#else
			//pragma comment(lib, "debug/libtiff.lib")
		#endif
	#endif
	#if !defined(IL_NO_MNG) || !defined(IL_NO_PNG)
		#ifndef IL_DEBUG
			//pragma comment(lib, "zlib.lib")
		#else
			//pragma comment(lib, "debug/zlib.lib")
		#endif
	#endif
#endif
//
// Image loading/saving functions
//
ILboolean ilIsValidBmp(ILconst_string CONST_RESTRICT FileName);
ILboolean ilIsValidBmpF(ILHANDLE File);
ILboolean ilIsValidBmpL(const void *Lump, const ILuint Size);
ILboolean ilLoadBmp(ILconst_string FileName);
ILboolean ilLoadBmpF(ILHANDLE File);
ILboolean ilLoadBmpL(const void *Lump, const ILuint Size);
ILboolean ilSaveBmp(ILconst_string FileName);
ILboolean ilSaveBmpF(ILHANDLE File);
ILboolean ilSaveBmpL(void *Lump, ILuint Size);
ILboolean ilSaveCHeader(ILconst_string FileName, char *InternalName);
ILboolean ilLoadCut(ILconst_string FileName);
ILboolean ilLoadCutF(ILHANDLE File);
ILboolean ilLoadCutL(const void *Lump, ILuint Size);
ILboolean ilIsValidDcx(ILconst_string FileName);
ILboolean ilIsValidDcxF(ILHANDLE File);
ILboolean ilIsValidDcxL(const void *Lump, ILuint Size);
ILboolean ilLoadDcx(ILconst_string FileName);
ILboolean ilLoadDcxF(ILHANDLE File);
ILboolean ilLoadDcxL(const void *Lump, ILuint Size);
ILboolean ilIsValidDds(ILconst_string FileName);
ILboolean ilIsValidDdsF(ILHANDLE File);
ILboolean ilIsValidDdsL(const void *Lump, ILuint Size);
ILboolean ilLoadDds(ILconst_string FileName);
ILboolean ilLoadDdsF(ILHANDLE File);
ILboolean ilLoadDdsL(const void *Lump, ILuint Size);
ILboolean ilSaveDds(ILconst_string FileName);
ILboolean ilSaveDdsF(ILHANDLE File);
ILboolean ilSaveDdsL(void *Lump, ILuint Size);
ILboolean ilLoadDoom(ILconst_string FileName);
ILboolean ilLoadDoomF(ILHANDLE File);
ILboolean ilLoadDoomL(const void *Lump, ILuint Size);
ILboolean ilLoadDoomFlat(ILconst_string FileName);
ILboolean ilLoadDoomFlatF(ILHANDLE File);
ILboolean ilLoadDoomFlatL(const void *Lump, ILuint Size);
ILboolean ilIsValidExr(ILconst_string FileName);
ILboolean ilIsValidExrF(ILHANDLE File);
ILboolean ilIsValidExrL(const void *Lump, ILuint Size);
ILboolean ilLoadExr(ILconst_string FileName);
ILboolean ilLoadExrF(ILHANDLE File);
ILboolean ilLoadExrL(const void *Lump, ILuint Size);
ILboolean ilIsValidGif(ILconst_string FileName);
ILboolean ilIsValidGifF(ILHANDLE File);
ILboolean ilIsValidGifL(const void *Lump, ILuint Size);
ILboolean ilLoadGif(ILconst_string FileName);
ILboolean ilLoadGifF(ILHANDLE File);
ILboolean ilLoadGifL(const void *Lump, ILuint Size);
ILboolean ilIsValidHdr(ILconst_string FileName);
ILboolean ilIsValidHdrF(ILHANDLE File);
ILboolean ilIsValidHdrL(const void *Lump, ILuint Size);
ILboolean ilLoadHdr(ILconst_string FileName);
ILboolean ilLoadHdrF(ILHANDLE File);
ILboolean ilLoadHdrL(const void *Lump, ILuint Size);
ILboolean ilSaveHdr(ILconst_string FileName);
ILboolean ilSaveHdrF(ILHANDLE File);
ILboolean ilSaveHdrL(void *Lump, ILuint Size);
ILboolean ilLoadIcon(ILconst_string FileName);
ILboolean ilLoadIconF(ILHANDLE File);
ILboolean ilLoadIconL(const void *Lump, ILuint Size);
ILboolean ilLoadIcns(ILconst_string FileName);
ILboolean ilLoadIcnsF(ILHANDLE File);
ILboolean ilLoadIcnsL(const void *Lump, ILuint Size);
ILboolean ilIsValidJpg(ILconst_string FileName);
ILboolean ilIsValidJpgF(ILHANDLE File);
ILboolean ilIsValidJpgL(const void *Lump, ILuint Size);
ILboolean ilLoadJp2(ILconst_string FileName);
ILboolean ilLoadJp2F(ILHANDLE File);
ILboolean ilLoadJp2L(const void *Lump, ILuint Size);
ILboolean ilLoadJp2LInternal(const void *Lump, ILuint Size, ILimage *Image);
ILboolean ilLoadJpeg(ILconst_string FileName);
ILboolean ilLoadJpegF(ILHANDLE File);
ILboolean ilLoadJpegL(const void *Lump, ILuint Size);
ILboolean ilSaveJpeg(ILconst_string FileName);
ILboolean ilSaveJpegF(ILHANDLE File);
ILboolean ilSaveJpegL(void *Lump, ILuint Size);
ILboolean ilIsValidLif(ILconst_string FileName);
ILboolean ilIsValidLifF(ILHANDLE File);
ILboolean ilIsValidLifL(const void *Lump, ILuint Size);
ILboolean ilLoadLif(ILconst_string FileName);
ILboolean ilLoadLifF(ILHANDLE File);
ILboolean ilLoadLifL(const void *Lump, ILuint Size);
ILboolean ilLoadMdl(ILconst_string FileName);
ILboolean ilLoadMdlF(ILHANDLE File);
ILboolean ilLoadMdlL(const void *Lump, ILuint Size);
ILboolean ilLoadMng(ILconst_string FileName);
ILboolean ilLoadMngF(ILHANDLE File);
ILboolean ilLoadMngL(const void *Lump, ILuint Size);
ILboolean ilSaveMng(ILconst_string FileName);
ILboolean ilSaveMngF(ILHANDLE File);
ILboolean ilSaveMngL(void *Lump, ILuint Size);
ILboolean ilLoadPcd(ILconst_string FileName);
ILboolean ilLoadPcdF(ILHANDLE File);
ILboolean ilLoadPcdL(const void *Lump, ILuint Size);
ILboolean ilIsValidPcx(ILconst_string FileName);
ILboolean ilIsValidPcxF(ILHANDLE File);
ILboolean ilIsValidPcxL(const void *Lump, ILuint Size);
ILboolean ilLoadPcx(ILconst_string FileName);
ILboolean ilLoadPcxF(ILHANDLE File);
ILboolean ilLoadPcxL(const void *Lump, ILuint Size);
ILboolean ilSavePcx(ILconst_string FileName);
ILboolean ilSavePcxF(ILHANDLE File);
ILboolean ilSavePcxL(void *Lump, ILuint Size);
ILboolean ilIsValidPic(ILconst_string FileName);
ILboolean ilIsValidPicF(ILHANDLE File);
ILboolean ilIsValidPicL(const void *Lump, ILuint Size);
ILboolean ilLoadPic(ILconst_string FileName);
ILboolean ilLoadPicF(ILHANDLE File);
ILboolean ilLoadPicL(const void *Lump, ILuint Size);
ILboolean ilLoadPix(ILconst_string FileName);
ILboolean ilLoadPixF(ILHANDLE File);
ILboolean ilLoadPixL(const void *Lump, ILuint Size);
ILboolean ilIsValidPng(ILconst_string FileName);
ILboolean ilIsValidPngF(ILHANDLE File);
ILboolean ilIsValidPngL(const void *Lump, ILuint Size);
ILboolean ilLoadPng(ILconst_string FileName);
ILboolean ilLoadPngF(ILHANDLE File);
ILboolean ilLoadPngL(const void *Lump, ILuint Size);
ILboolean ilSavePng(ILconst_string FileName);
ILboolean ilSavePngF(ILHANDLE File);
ILboolean ilSavePngL(void *Lump, ILuint Size);
ILboolean ilIsValidPnm(ILconst_string FileName);
ILboolean ilIsValidPnmF(ILHANDLE File);
ILboolean ilIsValidPnmL(const void *Lump, ILuint Size);
ILboolean ilLoadPnm(ILconst_string FileName);
ILboolean ilLoadPnmF(ILHANDLE File);
ILboolean ilLoadPnmL(const void *Lump, ILuint Size);
ILboolean ilSavePnm(ILconst_string FileName);
ILboolean ilSavePnmF(ILHANDLE File);
ILboolean ilSavePnmL(void *Lump, ILuint Size);
ILboolean ilIsValidPsd(ILconst_string FileName);
ILboolean ilIsValidPsdF(ILHANDLE File);
ILboolean ilIsValidPsdL(const void *Lump, ILuint Size);
ILboolean ilLoadPsd(ILconst_string FileName);
ILboolean ilLoadPsdF(ILHANDLE File);
ILboolean ilLoadPsdL(const void *Lump, ILuint Size);
ILboolean ilSavePsd(ILconst_string FileName);
ILboolean ilSavePsdF(ILHANDLE File);
ILboolean ilSavePsdL(void *Lump, ILuint Size);
ILboolean ilIsValidPsp(ILconst_string FileName);
ILboolean ilIsValidPspF(ILHANDLE File);
ILboolean ilIsValidPspL(const void *Lump, ILuint Size);
ILboolean ilLoadPsp(ILconst_string FileName);
ILboolean ilLoadPspF(ILHANDLE File);
ILboolean ilLoadPspL(const void *Lump, ILuint Size);
ILboolean ilLoadPxr(ILconst_string FileName);
ILboolean ilLoadPxrF(ILHANDLE File);
ILboolean ilLoadPxrL(const void *Lump, ILuint Size);
ILboolean ilLoadRaw(ILconst_string FileName);
ILboolean ilLoadRawF(ILHANDLE File);
ILboolean ilLoadRawL(const void *Lump, ILuint Size);
ILboolean ilSaveRaw(ILconst_string FileName);
ILboolean ilSaveRawF(ILHANDLE File);
ILboolean ilSaveRawL(void *Lump, ILuint Size);
ILboolean ilIsValidSgi(ILconst_string FileName);
ILboolean ilIsValidSgiF(ILHANDLE File);
ILboolean ilIsValidSgiL(const void *Lump, ILuint Size);
ILboolean ilLoadSgi(ILconst_string FileName);
ILboolean ilLoadSgiF(ILHANDLE File);
ILboolean ilLoadSgiL(const void *Lump, ILuint Size);
ILboolean ilSaveSgi(ILconst_string FileName);
ILboolean ilSaveSgiF(ILHANDLE File);
ILboolean ilSaveSgiL(void *Lump, ILuint Size);
ILboolean ilIsValidSun(ILconst_string FileName);
ILboolean ilIsValidSunF(ILHANDLE File);
ILboolean ilIsValidSunL(const void *Lump, ILuint Size);
ILboolean ilLoadSun(ILconst_string FileName);
ILboolean ilLoadSunF(ILHANDLE File);
ILboolean ilLoadSunL(const void *Lump, ILuint Size);
ILboolean ilIsValidTga(ILconst_string FileName);
ILboolean ilIsValidTgaF(ILHANDLE File);
ILboolean ilIsValidTgaL(const void *Lump, ILuint Size);
ILboolean ilLoadTarga(ILconst_string FileName);
ILboolean ilLoadTargaF(ILHANDLE File);
ILboolean ilLoadTargaL(const void *Lump, ILuint Size);
ILboolean ilSaveTarga(ILconst_string FileName);
ILboolean ilSaveTargaF(ILHANDLE File);
ILboolean ilSaveTargaL(void *Lump, ILuint Size);
ILboolean ilIsValidTiff(ILconst_string FileName);
ILboolean ilIsValidTiffF(ILHANDLE File);
ILboolean ilIsValidTiffL(const void *Lump, ILuint Size);
ILboolean ilLoadTiff(ILconst_string FileName);
ILboolean ilLoadTiffF(ILHANDLE File);
ILboolean ilLoadTiffL(const void *Lump, ILuint Size);
ILboolean ilSaveTiff(ILconst_string FileName);
ILboolean ilSaveTiffF(ILHANDLE File);
ILboolean ilSaveTiffL(const void *Lump, ILuint Size);
ILboolean ilIsValidVtf(ILconst_string FileName);
ILboolean ilIsValidVtfF(ILHANDLE File);
ILboolean ilIsValidVtfL(const void *Lump, ILuint Size);
ILboolean ilLoadVtf(ILconst_string FileName);
ILboolean ilLoadVtfF(ILHANDLE File);
ILboolean ilLoadVtfL(const void *Lump, ILuint Size);
ILboolean ilLoadWal(ILconst_string FileName);
ILboolean ilLoadWalF(ILHANDLE File);
ILboolean ilLoadWalL(const void *Lump, ILuint Size);
ILboolean ilLoadWbmp(ILconst_string FileName);
ILboolean ilLoadWbmpF(ILHANDLE File);
ILboolean ilLoadWbmpL(const void *Lump, ILuint Size);
ILboolean ilSaveWbmp(ILconst_string FileName);
ILboolean ilSaveWbmpF(ILHANDLE File);
ILboolean ilSaveWbmpL(const void *Lump, ILuint Size);
ILboolean ilLoadWdp(ILconst_string FileName);
ILboolean ilLoadWdpF(ILHANDLE File);
ILboolean ilLoadWdpL(const void *Lump, ILuint Size);
ILboolean ilIsValidWdp(ILconst_string FileName);
ILboolean ilIsValidWdpF(ILHANDLE File);
ILboolean ilIsValidWdpL(const void *Lump, ILuint Size);
ILboolean ilLoadXpm(ILconst_string FileName);
ILboolean ilLoadXpmF(ILHANDLE File);
ILboolean ilLoadXpmL(const void *Lump, ILuint Size);


// OpenEXR is written in C++, so we have to wrap this to avoid linker errors.
/*#ifndef IL_NO_EXR
	#ifdef __cplusplus
	extern "C" {
	#endif
		ILboolean ilLoadExr(ILconst_string FileName);
	#ifdef __cplusplus
	}
	#endif
#endif*/

//ILboolean ilLoadExr(ILconst_string FileName);


#ifdef __cplusplus
}
#endif

#endif//INTERNAL_H

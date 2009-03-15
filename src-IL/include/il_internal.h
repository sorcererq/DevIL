//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 01/24/2009
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
#else // If we do not use autotools, we have to point to (possibly different) config.h than in the opposite case
	#include <IL/config.h>
#endif

// Standard headers
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef BUILD_MODULES
#include <ltdl.h>
#endif /* BUILD_MODULES */

#ifdef __cplusplus
extern "C" {
#endif

#include <IL/il.h>
#include <IL/devil_internal_exports.h>
#include "il_files.h"
#include "il_endian.h"

#ifndef _WIN32
	// The Microsoft HD Photo Device Porting Kit has not been ported to anything other
	//  than Windows yet, so we disable this if Windows is not the current platform.
	#define IL_NO_WDP
#endif//_WIN32

// If we do not want support for game image formats, this define removes them all.
#ifdef IL_NO_GAMES
	#define IL_NO_BLP
	#define IL_NO_DOOM
	#define IL_NO_FTX
	#define IL_NO_IWI
	#define IL_NO_LIF
	#define IL_NO_MDL
	#define IL_NO_ROT
	#define IL_NO_TPL
	#define IL_NO_WAL
#endif//IL_NO_GAMES

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
		#endif // _MSC_VER > 1000
	#endif
	#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
#endif//_WIN32

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

	#ifdef _WIN32_WCE  // Cannot use our inline ASM in Windows Mobile.
		#undef USE_WIN32_ASM
	#endif
#endif

struct modules
{
	char ** Module_names;	/**<  */
	char ** Module_formats;	/**<  */
	int Num_modules;	/**<  */

	lt_dlhandle * Module_handles;	/**<  */
};
typedef struct modules Modules;

Modules * create_modules();
void destroy_modules(Modules * modules);

struct format_functions
{
	/*@{*/ 
	/** The "Is valid" callbacks */
	int (* ilIsValid)(const char * filename);
	int (* ilIsValidF)(const char * filename);
	int (* ilIsValidL)(const void * lump, ILuint size);
	/*@}*/ 

	/*@{*/ 
	/** The "Load stuff" callbacks */
	int (* ilLoad)(const char * filename);
	int (* ilLoadF)(const char * filename);
	int (* ilLoadL)(const void * lump, ILuint size);
	/*@}*/ 

	/*@{*/ 
	/** The "Save stuff" callbacks */
	int (* ilSave)(const char * filename);
	int (* ilSaveF)(const char * filename);
	int (* ilSaveL)(const void * lump, ILuint size);
	/*@}*/ 
};
typedef struct format_functions Format_functions;

struct format
{
	Format_functions Callbacks;	/**< The callbacks stuff */
	char ** Extensions;	/**< The NULL-pointer-terminated lowercase extension strings, like {"jpg", "jpeg", NULL} */
	char * Name;	/**< Format name, like "JPEG" */
};
typedef struct format Format;

void load_callbacks(const Modules * modules, Format_functions * callbacks, const char * name);
void Set_format(const Modules * modules, Format * format, const char * format_name, const char * format_extensions);

static Format Formats[IL_FORMATS_COUNT];

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
#ifdef _WIN32_WCE
	char *strdup(const char *src);
#endif
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
ILuint					ilDetermineSize(ILenum Type);
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
ILuint			ilNVidiaCompressDXTFile(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILenum DxtType);
ILAPI ILubyte*	ILAPIENTRY ilNVidiaCompressDXT(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILenum DxtFormat, ILuint *DxtSize);
ILAPI ILubyte*	ILAPIENTRY ilSquishCompressDXT(ILubyte *Data, ILuint Width, ILuint Height, ILuint Depth, ILenum DxtFormat, ILuint *DxtSize);

// Conversion functions
ILboolean	ilAddAlpha(void);
ILboolean	ilAddAlphaKey(ILimage *Image);
ILboolean	iFastConvert(ILenum DestFormat);
ILboolean	ilFixCur(void);
ILboolean	ilFixImage(void);
ILboolean	ilRemoveAlpha(void);
ILboolean	ilSwapColours(void);
// Palette functions
ILboolean	iCopyPalette(ILpal *Dest, ILpal *Src);
// Miscellaneous functions
char*		iGetString(ILenum StringName);  // Internal version of ilGetString

//
// Image loading/saving functions
//
ILboolean ilSaveImage(ILconst_string filename);
ILboolean ilIsValid_BLP(ILconst_string FileName);
ILboolean ilIsValidF_BLP(ILHANDLE File);
ILboolean ilIsValidL_BLP(const void *Lump, ILuint Size);
ILboolean ilLoad_BLP(ILconst_string FileName);
ILboolean ilLoadF_BLP(ILHANDLE File);
ILboolean ilLoadL_BLP(const void *Lump, ILuint Size);
ILboolean ilIsValid_BMP(ILconst_string CONST_RESTRICT FileName);
ILboolean ilIsValidF_BMP(ILHANDLE File);
ILboolean ilIsValidL_BMP(const void *Lump, ILuint Size);
ILboolean ilLoad_BMP(ILconst_string FileName);
ILboolean ilLoadF_BMP(ILHANDLE File);
ILboolean ilLoadL_BMP(const void *Lump, ILuint Size);
ILboolean ilSave_BMP(ILconst_string FileName);
ILuint    ilSaveF_BMP(ILHANDLE File);
ILuint    ilSaveL_BMP(void *Lump, ILuint Size);
ILboolean ilSaveCHeader(ILconst_string FileName, char *InternalName);

ILboolean ilLoad_CUT(ILconst_string FileName);
ILboolean ilLoadF_CUT(ILHANDLE File);
ILboolean ilLoadL_CUT(const void *Lump, ILuint Size);
ILboolean ilIsValid_DCX(ILconst_string FileName);
ILboolean ilIsValidF_DCX(ILHANDLE File);
ILboolean ilIsValidL_DCX(const void *Lump, ILuint Size);
ILboolean ilLoad_DCX(ILconst_string FileName);
ILboolean ilLoadF_DCX(ILHANDLE File);
ILboolean ilLoadL_DCX(const void *Lump, ILuint Size);
ILboolean ilIsValid_DDS(ILconst_string FileName);
ILboolean ilIsValidF_DDS(ILHANDLE File);
ILboolean ilIsValidL_DDS(const void *Lump, ILuint Size);
ILboolean ilLoad_DDS(ILconst_string FileName);
ILboolean ilLoadF_DDS(ILHANDLE File);
ILboolean ilLoadL_DDS(const void *Lump, ILuint Size);
ILboolean ilSave_DDS(ILconst_string FileName);
ILuint    ilSaveF_DDS(ILHANDLE File);
ILuint    ilSaveL_DDS(void *Lump, ILuint Size);
ILboolean ilIsValid_DICOM(ILconst_string FileName);
ILboolean ilIsValidF_DICOM(ILHANDLE File);
ILboolean ilIsValidL_DICOM(const void *Lump, ILuint Size);
ILboolean ilLoad_DICOM(ILconst_string FileName);
ILboolean ilLoadF_DICOM(ILHANDLE File);
ILboolean ilLoadL_DICOM(const void *Lump, ILuint Size);
ILboolean ilLoad_DOOM(ILconst_string FileName);
ILboolean ilLoadF_DOOM(ILHANDLE File);
ILboolean ilLoadL_DOOM(const void *Lump, ILuint Size);
ILboolean ilLoad_DOOM_FLAT(ILconst_string FileName);
ILboolean ilLoadF_DOOM_FLAT(ILHANDLE File);
ILboolean ilLoadL_DOOM_FLAT(const void *Lump, ILuint Size);
ILboolean ilIsValid_EXR(ILconst_string FileName);
ILboolean ilIsValidF_EXR(ILHANDLE File);
ILboolean ilIsValidL_EXR(const void *Lump, ILuint Size);
ILboolean ilLoad_EXR(ILconst_string FileName);
ILboolean ilLoadF_EXR(ILHANDLE File);
ILboolean ilLoadL_EXR(const void *Lump, ILuint Size);
ILboolean ilSave_EXR(ILconst_string FileName);
ILuint    ilSaveF_EXR(ILHANDLE File);
ILuint    ilSaveL_EXR(void *Lump, ILuint Size);
ILboolean ilIsValid_FITS(ILconst_string FileName);
ILboolean ilIsValidF_FITS(ILHANDLE File);
ILboolean ilIsValidL_FITS(const void *Lump, ILuint Size);
ILboolean ilLoad_FITS(ILconst_string FileName);
ILboolean ilLoadF_FITS(ILHANDLE File);
ILboolean ilLoadL_FITS(const void *Lump, ILuint Size);
ILboolean ilLoad_FTX(ILconst_string FileName);
ILboolean ilLoadF_FTX(ILHANDLE File);
ILboolean ilLoadL_FTX(const void *Lump, ILuint Size);
ILboolean ilIsValid_GIF(ILconst_string FileName);
ILboolean ilIsValidF_GIF(ILHANDLE File);
ILboolean ilIsValidL_GIF(const void *Lump, ILuint Size);
ILboolean ilLoad_GIF(ILconst_string FileName);
ILboolean ilLoadF_GIF(ILHANDLE File);
ILboolean ilLoadL_GIF(const void *Lump, ILuint Size);
ILboolean ilIsValid_HDR(ILconst_string FileName);
ILboolean ilIsValidF_HDR(ILHANDLE File);
ILboolean ilIsValidL_HDR(const void *Lump, ILuint Size);
ILboolean ilLoad_HDR(ILconst_string FileName);
ILboolean ilLoadF_HDR(ILHANDLE File);
ILboolean ilLoadL_HDR(const void *Lump, ILuint Size);
ILboolean ilSave_HDR(ILconst_string FileName);
ILuint    ilSaveF_HDR(ILHANDLE File);
ILuint    ilSaveL_HDR(void *Lump, ILuint Size);
ILboolean ilLoad_ICON(ILconst_string FileName);
ILboolean ilLoadF_ICON(ILHANDLE File);
ILboolean ilLoadL_ICON(const void *Lump, ILuint Size);
ILboolean ilIsValid_ICNS(ILconst_string FileName);
ILboolean ilIsValidF_ICNS(ILHANDLE File);
ILboolean ilIsValidL_ICNS(const void *Lump, ILuint Size);
ILboolean ilLoad_ICNS(ILconst_string FileName);
ILboolean ilLoadF_ICNS(ILHANDLE File);
ILboolean ilLoadL_ICNS(const void *Lump, ILuint Size);
ILboolean ilLoad_IFF(ILconst_string FileName);
ILboolean ilLoadF_IFF(ILHANDLE File);
ILboolean ilLoadL_IFF(const void *Lump, ILuint Size);
ILboolean ilIsValid_IWI(ILconst_string FileName);
ILboolean ilIsValidF_IWI(ILHANDLE File);
ILboolean ilIsValidL_IWI(const void *Lump, ILuint Size);
ILboolean ilLoad_IWI(ILconst_string FileName);
ILboolean ilLoadF_IWI(ILHANDLE File);
ILboolean ilLoadL_IWI(const void *Lump, ILuint Size);
ILboolean ilIsValid_JP2(ILconst_string FileName);
ILboolean ilIsValidF_JP2(ILHANDLE File);
ILboolean ilIsValidL_JP2(const void *Lump, ILuint Size);
ILboolean ilLoad_JP2(ILconst_string FileName);
ILboolean ilLoadF_JP2(ILHANDLE File);
ILboolean ilLoadL_JP2(const void *Lump, ILuint Size);
ILboolean ilLoadLInternal_JP2(const void *Lump, ILuint Size, ILimage *Image);
ILboolean ilSave_JP2(ILconst_string FileName);
ILuint    ilSaveF_JP2(ILHANDLE File);
ILuint    ilSaveL_JP2(void *Lump, ILuint Size);
ILboolean ilIsValid_JPEG(ILconst_string FileName);
ILboolean ilIsValidF_JPEG(ILHANDLE File);
ILboolean ilIsValidL_JPEG(const void *Lump, ILuint Size);
ILboolean ilLoad_JPEG(ILconst_string FileName);
ILboolean ilLoadF_JPEG(ILHANDLE File);
ILboolean ilLoadL_JPEG(const void *Lump, ILuint Size);
ILboolean ilSave_JPEG(ILconst_string FileName);
ILuint    ilSaveF_JPEG(ILHANDLE File);
ILuint    ilSaveL_JPEG(void *Lump, ILuint Size);
ILboolean ilIsValid_LIF(ILconst_string FileName);
ILboolean ilIsValidF_LIF(ILHANDLE File);
ILboolean ilIsValidL_LIF(const void *Lump, ILuint Size);
ILboolean ilLoad_LIF(ILconst_string FileName);
ILboolean ilLoadF_LIF(ILHANDLE File);
ILboolean ilLoadL_LIF(const void *Lump, ILuint Size);
ILboolean ilIsValid_MDL(ILconst_string FileName);
ILboolean ilIsValidF_MDL(ILHANDLE File);
ILboolean ilIsValidL_MDL(const void *Lump, ILuint Size);
ILboolean ilLoad_MDL(ILconst_string FileName);
ILboolean ilLoadF_MDL(ILHANDLE File);
ILboolean ilLoadL_MDL(const void *Lump, ILuint Size);
ILboolean ilLoad_MNG(ILconst_string FileName);
ILboolean ilLoadF_MNG(ILHANDLE File);
ILboolean ilLoadL_MNG(const void *Lump, ILuint Size);
ILboolean ilSave_MNG(ILconst_string FileName);
ILuint    ilSaveF_MNG(ILHANDLE File);
ILuint    ilSaveL_MNG(void *Lump, ILuint Size);
ILboolean ilLoad_PCD(ILconst_string FileName);
ILboolean ilLoadF_PCD(ILHANDLE File);
ILboolean ilLoadL_PCD(const void *Lump, ILuint Size);
ILboolean ilIsValid_PCX(ILconst_string FileName);
ILboolean ilIsValidF_PCX(ILHANDLE File);
ILboolean ilIsValidL_PCX(const void *Lump, ILuint Size);
ILboolean ilLoad_PCX(ILconst_string FileName);
ILboolean ilLoadF_PCX(ILHANDLE File);
ILboolean ilLoadL_PCX(const void *Lump, ILuint Size);
ILboolean ilSave_PCX(ILconst_string FileName);
ILuint    ilSaveF_PCX(ILHANDLE File);
ILuint    ilSaveL_PCX(void *Lump, ILuint Size);
ILboolean ilIsValid_PIC(ILconst_string FileName);
ILboolean ilIsValidF_PIC(ILHANDLE File);
ILboolean ilIsValidL_PIC(const void *Lump, ILuint Size);
ILboolean ilLoad_PIC(ILconst_string FileName);
ILboolean ilLoadF_PIC(ILHANDLE File);
ILboolean ilLoadL_PIC(const void *Lump, ILuint Size);
ILboolean ilLoad_PIX(ILconst_string FileName);
ILboolean ilLoadF_PIX(ILHANDLE File);
ILboolean ilLoadL_PIX(const void *Lump, ILuint Size);
ILboolean ilIsValid_PNG(ILconst_string FileName);
ILboolean ilIsValidF_PNG(ILHANDLE File);
ILboolean ilIsValidL_PNG(const void *Lump, ILuint Size);
ILboolean ilLoad_PNG(ILconst_string FileName);
ILboolean ilLoadF_PNG(ILHANDLE File);
ILboolean ilLoadL_PNG(const void *Lump, ILuint Size);
ILboolean ilSave_PNG(ILconst_string FileName);
ILuint    ilSaveF_PNG(ILHANDLE File);
ILuint    ilSaveL_PNG(void *Lump, ILuint Size);
ILboolean ilIsValid_PNM(ILconst_string FileName);
ILboolean ilIsValidF_PNM(ILHANDLE File);
ILboolean ilIsValidL_PNM(const void *Lump, ILuint Size);
ILboolean ilLoad_PNM(ILconst_string FileName);
ILboolean ilLoadF_PNM(ILHANDLE File);
ILboolean ilLoadL_PNM(const void *Lump, ILuint Size);
ILboolean ilSave_PNM(ILconst_string FileName);
ILuint    ilSaveF_PNM(ILHANDLE File);
ILuint    ilSaveL_PNM(void *Lump, ILuint Size);
ILboolean ilIsValid_PSD(ILconst_string FileName);
ILboolean ilIsValidF_PSD(ILHANDLE File);
ILboolean ilIsValidL_PSD(const void *Lump, ILuint Size);
ILboolean ilLoad_PSD(ILconst_string FileName);
ILboolean ilLoadF_PSD(ILHANDLE File);
ILboolean ilLoadL_PSD(const void *Lump, ILuint Size);
ILboolean ilSave_PSD(ILconst_string FileName);
ILuint    ilSaveF_PSD(ILHANDLE File);
ILuint    ilSaveL_PSD(void *Lump, ILuint Size);
ILboolean ilIsValid_PSP(ILconst_string FileName);
ILboolean ilIsValidF_PSP(ILHANDLE File);
ILboolean ilIsValidL_PSP(const void *Lump, ILuint Size);
ILboolean ilLoad_PSP(ILconst_string FileName);
ILboolean ilLoadF_PSP(ILHANDLE File);
ILboolean ilLoadL_PSP(const void *Lump, ILuint Size);
ILboolean ilLoad_PXR(ILconst_string FileName);
ILboolean ilLoadF_PXR(ILHANDLE File);
ILboolean ilLoadL_PXR(const void *Lump, ILuint Size);
ILboolean ilLoad_RAW(ILconst_string FileName);
ILboolean ilLoadF_RAW(ILHANDLE File);
ILboolean ilLoadL_RAW(const void *Lump, ILuint Size);
ILboolean ilSave_RAW(ILconst_string FileName);
ILuint    ilSaveF_RAW(ILHANDLE File);
ILuint    ilSaveL_RAW(void *Lump, ILuint Size);
ILboolean ilLoad_ROT(ILconst_string FileName);
ILboolean ilLoadF_ROT(ILHANDLE File);
ILboolean ilLoadL_ROT(const void *Lump, ILuint Size);
ILboolean ilIsValid_ROT(ILconst_string FileName);
ILboolean ilIsValidF_ROT(ILHANDLE File);
ILboolean ilIsValidL_ROT(const void *Lump, ILuint Size);
ILboolean ilIsValid_SGI(ILconst_string FileName);
ILboolean ilIsValidF_SGI(ILHANDLE File);
ILboolean ilIsValidL_SGI(const void *Lump, ILuint Size);
ILboolean ilLoad_SGI(ILconst_string FileName);
ILboolean ilLoadF_SGI(ILHANDLE File);
ILboolean ilLoadL_SGI(const void *Lump, ILuint Size);
ILboolean ilSave_SGI(ILconst_string FileName);
ILuint    ilSaveF_SGI(ILHANDLE File);
ILuint    ilSaveL_SGI(void *Lump, ILuint Size);
ILboolean ilIsValid_SUN(ILconst_string FileName);
ILboolean ilIsValidF_SUN(ILHANDLE File);
ILboolean ilIsValidL_SUN(const void *Lump, ILuint Size);
ILboolean ilLoad_SUN(ILconst_string FileName);
ILboolean ilLoadF_SUN(ILHANDLE File);
ILboolean ilLoadL_SUN(const void *Lump, ILuint Size);
ILboolean ilIsValid_TGA(ILconst_string FileName);
ILboolean ilIsValidF_TGA(ILHANDLE File);
ILboolean ilIsValidL_TGA(const void *Lump, ILuint Size);
ILboolean ilLoad_TARGA(ILconst_string FileName);
ILboolean ilLoadF_TARGA(ILHANDLE File);
ILboolean ilLoadL_TARGA(const void *Lump, ILuint Size);
ILboolean ilSave_TARGA(ILconst_string FileName);
ILuint    ilSaveF_TARGA(ILHANDLE File);
ILuint    ilSaveL_TARGA(void *Lump, ILuint Size);
ILboolean ilLoad_TEXTURE(ILconst_string FileName);
ILboolean ilLoadF_TEXTURE(ILHANDLE File);
ILboolean ilLoadL_TEXTURE(const void *Lump, ILuint Size);
ILboolean ilIsValid_TIFF(ILconst_string FileName);
ILboolean ilIsValidF_TIFF(ILHANDLE File);
ILboolean ilIsValidL_TIFF(const void *Lump, ILuint Size);
ILboolean ilLoad_TIFF(ILconst_string FileName);
ILboolean ilLoadF_TIFF(ILHANDLE File);
ILboolean ilLoadL_TIFF(const void *Lump, ILuint Size);
ILboolean ilSave_TIFF(ILconst_string FileName);
ILuint    ilSaveF_TIFF(ILHANDLE File);
ILuint    ilSaveL_TIFF(void *Lump, ILuint Size);
ILboolean ilIsValid_TPL(ILconst_string FileName);
ILboolean ilIsValidF_TPL(ILHANDLE File);
ILboolean ilIsValidL_TPL(const void *Lump, ILuint Size);
ILboolean ilLoad_TPL(ILconst_string FileName);
ILboolean ilLoadF_TPL(ILHANDLE File);
ILboolean ilLoadL_TPL(const void *Lump, ILuint Size);
ILboolean ilIsValid_VTF(ILconst_string FileName);
ILboolean ilIsValidF_VTF(ILHANDLE File);
ILboolean ilIsValidL_VTF(const void *Lump, ILuint Size);
ILboolean ilLoad_VTF(ILconst_string FileName);
ILboolean ilLoadF_VTF(ILHANDLE File);
ILboolean ilLoadL_VTF(const void *Lump, ILuint Size);
ILboolean ilLoad_WAL(ILconst_string FileName);
ILboolean ilLoadF_WAL(ILHANDLE File);
ILboolean ilLoadL_WAL(const void *Lump, ILuint Size);
ILboolean ilLoad_WBMP(ILconst_string FileName);
ILboolean ilLoadF_WBMP(ILHANDLE File);
ILboolean ilLoadL_WBMP(const void *Lump, ILuint Size);
ILboolean ilSave_WBMP(ILconst_string FileName);
ILuint    ilSaveF_WBMP(ILHANDLE File);
ILuint    ilSaveL_WBMP(void *Lump, ILuint Size);
ILboolean ilIsValid_WDP(ILconst_string FileName);
ILboolean ilIsValidF_WDP(ILHANDLE File);
ILboolean ilIsValidL_WDP(const void *Lump, ILuint Size);
ILboolean ilLoad_WDP(ILconst_string FileName);
ILboolean ilLoadF_WDP(ILHANDLE File);
ILboolean ilLoadL_WDP(const void *Lump, ILuint Size);
ILboolean ilIsValid_XPM(ILconst_string FileName);
ILboolean ilIsValidF_XPM(ILHANDLE File);
ILboolean ilIsValidL_XPM(const void *Lump, ILuint Size);
ILboolean ilLoad_XPM(ILconst_string FileName);
ILboolean ilLoadF_XPM(ILHANDLE File);
ILboolean ilLoadL_XPM(const void *Lump, ILuint Size);

// OpenEXR is written in C++, so we have to wrap this to avoid linker errors.
/*#ifndef IL_NO_EXR
	#ifdef __cplusplus
	extern "C" {
	#endif
		ILboolean ilLoad_EXR(ILconst_string FileName);
	#ifdef __cplusplus
	}
	#endif
#endif*/

//ILboolean ilLoad_EXR(ILconst_string FileName);


#ifdef __cplusplus
}
#endif

#endif//INTERNAL_H

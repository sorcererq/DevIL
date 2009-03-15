//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified: 03/13/2009
//
// Filename: src-IL/src/il_targa.cpp
//
// Description: Reads from and writes to a Targa (.tga) file.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_TGA
#include "il_targa.h"
//#include <time.h>  // for ilMakeString()
#include <string.h>
#include "il_manip.h"
#include "il_bits.h"

#ifdef DJGPP
#include <dos.h>
#endif


//! Checks if the file specified in FileName is a valid Targa file.
ILboolean ilIsValidTga(ILconst_string FileName)
{
	ILHANDLE	TargaFile;
	ILboolean	bTarga = IL_FALSE;
	
	if (!iCheckExtension(FileName, IL_TEXT("tga")) &&
		!iCheckExtension(FileName, IL_TEXT("vda")) &&
		!iCheckExtension(FileName, IL_TEXT("icb")) &&
		!iCheckExtension(FileName, IL_TEXT("vst"))) {
		ilSetError(IL_INVALID_EXTENSION);
		return bTarga;
	}
	
	TargaFile = iopenr(FileName);
	if (TargaFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bTarga;
	}
	
	bTarga = ilIsValidTgaF(TargaFile);
	icloser(TargaFile);
	
	return bTarga;
}


//! Checks if the ILHANDLE contains a valid Targa file at the current position.
ILboolean ilIsValidTgaF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iIsValidTarga();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Checks if Lump is a valid Targa lump.
ILboolean ilIsValidTgaL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iIsValidTarga();
}


// Internal function used to get the Targa header from the current file.
ILboolean iGetTgaHead(TARGAHEAD *Header)
{
	Header->IDLen = (ILubyte)igetc();
	Header->ColMapPresent = (ILubyte)igetc();
	Header->ImageType = (ILubyte)igetc();
	Header->FirstEntry = GetLittleShort();
	Header->ColMapLen = GetLittleShort();
	Header->ColMapEntSize = (ILubyte)igetc();

	Header->OriginX = GetLittleShort();
	Header->OriginY = GetLittleShort();
	Header->Width = GetLittleUShort();
	Header->Height = GetLittleUShort();
	Header->Bpp = (ILubyte)igetc();
	Header->ImageDesc = (ILubyte)igetc();
	
	return IL_TRUE;
}


// Internal function to get the header and check it.
ILboolean iIsValidTarga()
{
	TARGAHEAD	Head;
	
	if (!iGetTgaHead(&Head))
		return IL_FALSE;
	iseek(-(ILint)sizeof(TARGAHEAD), IL_SEEK_CUR);
	
	return iCheckTarga(&Head);
}


// Internal function used to check if the HEADER is a valid Targa header.
ILboolean iCheckTarga(TARGAHEAD *Header)
{
	if (Header->Width == 0 || Header->Height == 0)
		return IL_FALSE;
	if (Header->Bpp != 8 && Header->Bpp != 15 && Header->Bpp != 16 
		&& Header->Bpp != 24 && Header->Bpp != 32)
		return IL_FALSE;
	if (Header->ImageDesc & BIT_4)	// Supposed to be set to 0
		return IL_FALSE;
	
	// check type (added 20040218)
	if (Header->ImageType   != TGA_NO_DATA
	   && Header->ImageType != TGA_COLMAP_UNCOMP
	   && Header->ImageType != TGA_UNMAP_UNCOMP
	   && Header->ImageType != TGA_BW_UNCOMP
	   && Header->ImageType != TGA_COLMAP_COMP
	   && Header->ImageType != TGA_UNMAP_COMP
	   && Header->ImageType != TGA_BW_COMP)
		return IL_FALSE;
	
	// Doesn't work well with the bitshift so change it.
	if (Header->Bpp == 15)
		Header->Bpp = 16;
	
	return IL_TRUE;
}


//! Reads a Targa file
ILimage *ilLoadTarga(ILconst_string FileName)
{
	ILHANDLE	TargaFile;
	ILimage		*Image;
	
	TargaFile = iopenr(FileName);
	if (TargaFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return NULL;
	}

	Image = ilLoadTargaF(TargaFile);
	icloser(TargaFile);

	return Image;
}


//! Reads an already-opened Targa file
ILimage *ilLoadTargaF(ILHANDLE File)
{
	ILuint	FirstPos;
	ILimage	*Image;
	
	iSetInputFile(File);
	FirstPos = itell();
	Image = iLoadTargaInternal();
	iseek(FirstPos, IL_SEEK_SET);
	
	return Image;
}


//! Reads from a memory "lump" that contains a Targa
ILimage *ilLoadTargaL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadTargaInternal();
}


// Internal function used to load the Targa.
ILimage *iLoadTargaInternal()
{
	TARGAHEAD	Header;
	ILenum		iOrigin;
	ILimage		*Image;

	if (!iGetTgaHead(&Header))
		return IL_FALSE;
	if (!iCheckTarga(&Header)) {
		ilSetError(IL_INVALID_FILE_HEADER);
		return IL_FALSE;
	}

	switch (Header.ImageType)
	{
		case TGA_NO_DATA:
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			return NULL;
		case TGA_COLMAP_UNCOMP:
		case TGA_COLMAP_COMP:
			Image = iReadColMapTga(&Header);
			break;
		case TGA_UNMAP_UNCOMP:
		case TGA_UNMAP_COMP:
			Image = iReadUnmapTga(&Header);
			break;
		case TGA_BW_UNCOMP:
		case TGA_BW_COMP:
			Image = iReadBwTga(&Header);
			break;
		default:
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			return NULL;
	}

	if (Image == NULL)
		return NULL;

	// @JASON Extra Code to manipulate the image depending on
	// the Image Descriptor's origin bits.
	iOrigin = Header.ImageDesc & IMAGEDESC_ORIGIN_MASK;
	
	switch (iOrigin)
	{
		case IMAGEDESC_TOPLEFT:
			Image->Origin = IL_ORIGIN_UPPER_LEFT;
			break;

		case IMAGEDESC_TOPRIGHT:
			Image->Origin = IL_ORIGIN_UPPER_LEFT;
			iMirror(Image);
			break;

		case IMAGEDESC_BOTLEFT:
			Image->Origin = IL_ORIGIN_LOWER_LEFT;
			break;

		case IMAGEDESC_BOTRIGHT:
			Image->Origin = IL_ORIGIN_LOWER_LEFT;
			iMirror(Image);
			break;
	}

	if (!ilFixImage(Image)) {
		ilCloseImage(Image);
		return NULL;
	}
	return Image;
}


ILimage *iReadColMapTga(TARGAHEAD *Header)
{
	ILimage		*Image;
	char		ID[255];
	ILuint		i;
	ILushort	Pixel;

	if (iread(ID, 1, Header->IDLen) != Header->IDLen)
		return IL_FALSE;

	Image = ilNewImage(Header->Width, Header->Height, 1, ilGetFormatBpp(Header->Bpp >> 3), IL_UNSIGNED_BYTE, NULL);
	if (Image == NULL)
		return NULL;
	if (Image->Pal.Palette && Image->Pal.PalSize)
		ifree(Image->Pal.Palette);

	Image->Format = IL_COLOUR_INDEX;
	Image->Pal.PalSize = Header->ColMapLen * (Header->ColMapEntSize >> 3);

	switch (Header->ColMapEntSize)
	{
		case 16:
			Image->Pal.PalType = IL_PAL_BGRA32;
			Image->Pal.PalSize = Header->ColMapLen * 4;
			break;
		case 24:
			Image->Pal.PalType = IL_PAL_BGR24;
			break;
		case 32:
			Image->Pal.PalType = IL_PAL_BGRA32;
			break;
		default:
			// Should *never* reach here
			ilSetError(IL_ILLEGAL_FILE_VALUE);
			ilCloseImage(Image);
			return NULL;
	}

	Image->Pal.Palette = (ILubyte*)ialloc(Image->Pal.PalSize);
	if (Image->Pal.Palette == NULL) {
		ilCloseImage(Image);
		return NULL;
	}

	// Do we need to do something with FirstEntry?	Like maybe:
	//	iread(Image->Pal + Targa->FirstEntry, 1, Image->Pal.PalSize);  ??
	if (Header->ColMapEntSize != 16)
	{
		if (iread(Image->Pal.Palette, 1, Image->Pal.PalSize) != Image->Pal.PalSize) {
			ilCloseImage(Image);
			return NULL;
		}
	}
	else {
		// 16 bit palette, so we have to break it up.
		for (i = 0; i < Image->Pal.PalSize; i += 4)
		{
			Pixel = GetBigUShort();
			if (ieof())
				return IL_FALSE;
			Image->Pal.Palette[3] = (Pixel & 0x8000) >> 12;
			Image->Pal.Palette[0] = (Pixel & 0xFC00) >> 7;
			Image->Pal.Palette[1] = (Pixel & 0x03E0) >> 2;
			Image->Pal.Palette[2] = (Pixel & 0x001F) << 3;
		}
	}

	if (Header->ImageType == TGA_COLMAP_COMP) {
		if (!iUncompressTgaData(Image)) {
			ilCloseImage(Image);
			return NULL;
		}
	}
	else {
		if (iread(Image->Data, 1, Image->SizeOfData) != Image->SizeOfData) {
			ilCloseImage(Image);
			return NULL;
		}
	}

	return Image;
}


ILimage *iReadUnmapTga(TARGAHEAD *Header)
{
	ILimage	*Image;
	ILubyte Bpp;
	char	ID[255];

	if (iread(ID, 1, Header->IDLen) != Header->IDLen)
		return IL_FALSE;
	
	/*if (Header->Bpp == 16)
		Bpp = 3;
	else*/
	Bpp = (ILubyte)(Header->Bpp >> 3);

	Image = ilNewImage(Header->Width, Header->Height, 1, ilGetFormatBpp(Bpp), IL_UNSIGNED_BYTE, NULL);
	if (Image == NULL)
		return NULL;
	
	switch (Bpp)
	{
		case 1:
			Image->Format = IL_COLOUR_INDEX;  // wtf?  How is this possible?
			break;
		case 2:  // 16-bit is not supported directly!
				 //Image->Format = IL_RGB5_A1;
			/*Image->Format = IL_RGBA;
			Image->Type = IL_UNSIGNED_SHORT_5_5_5_1_EXT;*/
			//Image->Type = IL_UNSIGNED_SHORT_5_6_5_REV;
			
			// Remove?
			//ilCloseImage(Image);
			//ilSetError(IL_FORMAT_NOT_SUPPORTED);
			//return IL_FALSE;
			
			/*Image->Bpp = 4;
			Image->Format = IL_BGRA;
			Image->Type = IL_UNSIGNED_SHORT_1_5_5_5_REV;*/
			
			Image->Format = IL_BGR;
			
			break;
		case 3:
			Image->Format = IL_BGR;
			break;
		case 4:
			Image->Format = IL_BGRA;
			break;
		default:
			ilSetError(IL_INVALID_VALUE);
			return IL_FALSE;
	}


	// @TODO:  Determine this:
	// We assume that no palette is present, but it's possible...
	//	Should we mess with it or not?


	if (Header->ImageType == TGA_UNMAP_COMP) {
		if (!iUncompressTgaData(Image)) {
			ilCloseImage(Image);
			return NULL;
		}
	}
	else {
		if (iread(Image->Data, 1, Image->SizeOfData) != Image->SizeOfData) {
			ilCloseImage(Image);
			return NULL;
		}
	}

	// Go ahead and expand it to 24-bit.
	if (Header->Bpp == 16) {
		if (!i16BitTarga(Image))
			return IL_FALSE;
		return Image;
	}
	
	return Image;
}


ILimage *iReadBwTga(TARGAHEAD *Header)
{
	ILimage	*Image;
	char	ID[255];

	if (iread(ID, 1, Header->IDLen) != Header->IDLen)
		return NULL;

	// We assume that no palette is present, but it's possible...
	//	Should we mess with it or not?

	Image = ilNewImage(Header->Width, Header->Height, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, NULL);
	if (Image == NULL)
		return NULL;

	if (Header->ImageType == TGA_BW_COMP) {
		if (!iUncompressTgaData(Image)) {
			ilCloseImage(Image);
			return NULL;
		}
	}
	else {
		if (iread(Image->Data, 1, Image->SizeOfData) != Image->SizeOfData) {
			ilCloseImage(Image);
			return NULL;
		}
	}

	return Image;
}


ILboolean iUncompressTgaData(ILimage *Image)
{
	ILuint	BytesRead = 0, Size, RunLen, i, ToRead;
	ILubyte Header, Color[4];
	ILint	c;
	
	Size = Image->Width * Image->Height * Image->Depth * Image->Bpp;
	
	if (iGetHint(IL_MEM_SPEED_HINT) == IL_FASTEST)
		iPreCache(Image->SizeOfData / 2);
	
	while (BytesRead < Size) {
		Header = (ILubyte)igetc();
		if (Header & BIT_7) {
			ClearBits(Header, BIT_7);
			if (iread(Color, 1, Image->Bpp) != Image->Bpp) {
				iUnCache();
				return IL_FALSE;
			}
			RunLen = (Header+1) * Image->Bpp;
			for (i = 0; i < RunLen; i += Image->Bpp) {
				// Read the color in, but we check to make sure that we do not go past the end of the image.
				for (c = 0; c < Image->Bpp && BytesRead+i+c < Size; c++) {
					Image->Data[BytesRead+i+c] = Color[c];
				}
			}
			BytesRead += RunLen;
		}
		else {
			RunLen = (Header+1) * Image->Bpp;
			// We have to check that we do not go past the end of the image data.
			if (BytesRead + RunLen > Size)
				ToRead = Size - BytesRead;
			else
				ToRead = RunLen;
			if (iread(Image->Data + BytesRead, 1, ToRead) != ToRead) {
				iUnCache();  //@TODO: Error needed here?
				return IL_FALSE;
			}
			BytesRead += RunLen;

			if (BytesRead + RunLen > Size)
				iseek(RunLen - ToRead, IL_SEEK_CUR);
		}
	}
	
	iUnCache();
	
	return IL_TRUE;
}


// Pretty damn unoptimized
ILboolean i16BitTarga(ILimage *Image)
{
	ILushort	*Temp1;
	ILubyte 	*Data, *Temp2;
	ILuint		x, PixSize = Image->Width * Image->Height;
	
	Data = (ILubyte*)ialloc(Image->Width * Image->Height * 3);
	Temp1 = (ILushort*)Image->Data;
	Temp2 = Data;
	
	if (Data == NULL)
		return IL_FALSE;
	
	for (x = 0; x < PixSize; x++) {
		*Temp2++ = (*Temp1 & 0x001F) << 3;	// Blue
		*Temp2++ = (*Temp1 & 0x03E0) >> 2;	// Green
		*Temp2++ = (*Temp1 & 0x7C00) >> 7;	// Red
		
		Temp1++;
		
		
		/*s = *Temp;
		s = SwapShort(s);
		a = !!(s & BIT_15);
		
		s = s << 1;
		
		//if (a) {
		SetBits(s, BIT_0);
		//}
		
		//SetBits(s, BIT_15);
		
		*Temp++ = s;*/
	}
	
	if (!ilTexImage(Image, Image->Width, Image->Height, 1, IL_BGR, IL_UNSIGNED_BYTE, Data)) {
		ifree(Data);
		return IL_FALSE;
	}
	
	ifree(Data);
	
	return IL_TRUE;
}


//! Writes a Targa file
ILboolean ilSaveTarga(ILimage *Image, const ILstring FileName)
{
	ILHANDLE	TargaFile;
	ILuint		TargaSize;

	if (ilGetBoolean(IL_FILE_MODE) == IL_FALSE) {
		if (iFileExists(FileName)) {
			ilSetError(IL_FILE_ALREADY_EXISTS);
			return IL_FALSE;
		}
	}

	TargaFile = iopenw(FileName);
	if (TargaFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return IL_FALSE;
	}

	TargaSize = ilSaveTargaF(Image, TargaFile);
	iclosew(TargaFile);

	if (TargaSize == 0)
		return IL_FALSE;
	return IL_TRUE;
}


//! Writes a Targa to an already-opened file
ILuint ilSaveTargaF(ILimage *Image, ILHANDLE File)
{
	ILuint Pos;
	iSetOutputFile(File);
	Pos = itellw();
	if (iSaveTargaInternal(Image) == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


//! Writes a Targa to a memory "lump"
ILuint ilSaveTargaL(ILimage *Image, void *Lump, ILuint Size)
{
	ILuint Pos = itellw();
	iSetOutputLump(Lump, Size);
	if (iSaveTargaInternal(Image) == IL_FALSE)
		return 0;  // Error occurred
	return itellw() - Pos;  // Return the number of bytes written.
}


// Internal function used to save the Targa.
ILboolean iSaveTargaInternal(ILimage *Image)
{
	const char	*ID = iGetString(IL_TGA_ID_STRING);
	const char	*AuthName = iGetString(IL_TGA_AUTHNAME_STRING);
	const char	*AuthComment = iGetString(IL_TGA_AUTHCOMMENT_STRING);
	ILubyte 	IDLen = 0, UsePal, Type, PalEntSize;
	ILshort 	ColMapStart = 0, PalSize;
	ILubyte		Temp;
	ILenum		Format;
	ILboolean	Compress;
	ILuint		RleLen;
	ILubyte 	*Rle;
	ILpal		*TempPal = NULL;
	ILimage 	*TempImage = NULL;
	ILuint		ExtOffset, i;
	char		*Footer = "TRUEVISION-XFILE.\0";
	char		*idString = "Developer's Image Library (DevIL)";
	ILuint		Day, Month, Year, Hour, Minute, Second;
	char		*TempData;

	if (Image == NULL) {
		ilSetError(IL_ILLEGAL_OPERATION);
		return IL_FALSE;
	}
	
	if (iGetInt(IL_TGA_RLE) == IL_TRUE)
		Compress = IL_TRUE;
	else
		Compress = IL_FALSE;
	
	if (ID)
		IDLen = (ILubyte)ilCharStrLen(ID);
	
	if (Image->Pal.Palette && Image->Pal.PalSize && Image->Pal.PalType != IL_PAL_NONE)
		UsePal = IL_TRUE;
	else
		UsePal = IL_FALSE;
	
	iwrite(&IDLen, sizeof(ILubyte), 1);
	iwrite(&UsePal, sizeof(ILubyte), 1);

	Format = Image->Format;
	switch (Format) {
		case IL_COLOUR_INDEX:
			if (Compress)
				Type = 9;
			else
				Type = 1;
			break;
		case IL_BGR:
		case IL_BGRA:
			if (Compress)
				Type = 10;
			else
				Type = 2;
			break;
		case IL_RGB:
		case IL_RGBA:
			ilSwapColours(Image);
			if (Compress)
				Type = 10;
			else
				Type = 2;
			break;
		case IL_LUMINANCE:
			if (Compress)
				Type = 11;
			else
				Type = 3;
			break;
		default:
			// Should convert the types here...
			ilSetError(IL_INVALID_VALUE);
			ifree(ID);
			ifree(AuthName);
			ifree(AuthComment);
			return IL_FALSE;
	}
	
	iwrite(&Type, sizeof(ILubyte), 1);
	SaveLittleShort(ColMapStart);
	
	switch (Image->Pal.PalType)
	{
		case IL_PAL_NONE:
			PalSize = 0;
			PalEntSize = 0;
			break;
		case IL_PAL_BGR24:
			PalSize = (ILshort)(Image->Pal.PalSize / 3);
			PalEntSize = 24;
			TempPal = &Image->Pal;
			break;
			
		case IL_PAL_RGB24:
		case IL_PAL_RGB32:
		case IL_PAL_RGBA32:
		case IL_PAL_BGR32:
		case IL_PAL_BGRA32:
			TempPal = iConvertPal(&Image->Pal, IL_PAL_BGR24);
			if (TempPal == NULL)
				return IL_FALSE;
				PalSize = (ILshort)(TempPal->PalSize / 3);
			PalEntSize = 24;
			break;
		default:
			ilSetError(IL_INVALID_VALUE);
			ifree(ID);
			ifree(AuthName);
			ifree(AuthComment);
			PalSize = 0;
			PalEntSize = 0;
			return IL_FALSE;
	}
	SaveLittleShort(PalSize);
	iwrite(&PalEntSize, sizeof(ILubyte), 1);
	
	if (Image->Bpc > 1) {
		TempImage = iConvertImage(Image, Image->Format, IL_UNSIGNED_BYTE);
		if (TempImage == NULL) {
			ifree(ID);
			ifree(AuthName);
			ifree(AuthComment);
			return IL_FALSE;
		}
	}
	else {
		TempImage = Image;
	}
	
	if (TempImage->Origin != IL_ORIGIN_LOWER_LEFT)
		TempData = (char*)iGetFlipped(TempImage);
	else
		TempData = (char*)TempImage->Data;
	
	// Write out the origin stuff.
	Temp = 0;
	iwrite(&Temp, sizeof(ILshort), 1);
	iwrite(&Temp, sizeof(ILshort), 1);
	
	Temp = Image->Bpp << 3;  // Changes to bits per pixel
	SaveLittleUShort((ILushort)Image->Width);
	SaveLittleUShort((ILushort)Image->Height);
	iwrite(&Temp, sizeof(ILubyte), 1);
	
	// Still don't know what exactly this is for...
	Temp = 0;
	iwrite(&Temp, sizeof(ILubyte), 1);
	iwrite(ID, sizeof(char), IDLen);
	ifree(ID);
	//iwrite(ID, sizeof(ILbyte), IDLen - sizeof(ILuint));
	//iwrite(&Image->Depth, sizeof(ILuint), 1);
	
	// Write out the colormap
	if (UsePal)
		iwrite(TempPal->Palette, sizeof(ILubyte), TempPal->PalSize);
	// else do nothing
	
	if (!Compress)
		iwrite(TempData, sizeof(ILubyte), TempImage->SizeOfData);
	else {
		Rle = (ILubyte*)ialloc(TempImage->SizeOfData + TempImage->SizeOfData / 2 + 1);	// max
		if (Rle == NULL) {
			ifree(AuthName);
			ifree(AuthComment);
			return IL_FALSE;
		}
		RleLen = ilRleCompress((unsigned char*)TempData, TempImage->Width, TempImage->Height,
		                       TempImage->Depth, TempImage->Bpp, Rle, IL_TGACOMP, NULL);
		
		iwrite(Rle, 1, RleLen);
		ifree(Rle);
	}
	
	// Write the extension area.
	ExtOffset = itellw();
	SaveLittleUShort(495);	// Number of bytes in the extension area (TGA 2.0 spec)
	iwrite(AuthName, 1, ilCharStrLen(AuthName));
	ipad(41 - ilCharStrLen(AuthName));
	iwrite(AuthComment, 1, ilCharStrLen(AuthComment));
	ipad(324 - ilCharStrLen(AuthComment));
	ifree(AuthName);
	ifree(AuthComment);
	
	// Write time/date
	iGetDateTime(&Month, &Day, &Year, &Hour, &Minute, &Second);
	SaveLittleUShort((ILushort)Month);
	SaveLittleUShort((ILushort)Day);
	SaveLittleUShort((ILushort)Year);
	SaveLittleUShort((ILushort)Hour);
	SaveLittleUShort((ILushort)Minute);
	SaveLittleUShort((ILushort)Second);
	
	for (i = 0; i < 6; i++) {  // Time created
		SaveLittleUShort(0);
	}
	for (i = 0; i < 41; i++) {	// Job name/ID
		iputc(0);
	}
	for (i = 0; i < 3; i++) {  // Job time
		SaveLittleUShort(0);
	}
	
	iwrite(idString, 1, ilCharStrLen(idString));	// Software ID
	for (i = 0; i < 41 - ilCharStrLen(idString); i++) {
		iputc(0);
	}
	SaveLittleUShort(IL_VERSION);  // Software version
	iputc(' ');  // Release letter (not beta anymore, so use a space)
	
	SaveLittleUInt(0);	// Key colour
	SaveLittleUInt(0);	// Pixel aspect ratio
	SaveLittleUInt(0);	// Gamma correction offset
	SaveLittleUInt(0);	// Colour correction offset
	SaveLittleUInt(0);	// Postage stamp offset
	SaveLittleUInt(0);	// Scan line offset
	iputc(3);  // Attributes type
	
	// Write the footer.
	SaveLittleUInt(ExtOffset);	// No extension area
	SaveLittleUInt(0);	// No developer directory
	iwrite(Footer, 1, ilCharStrLen(Footer));
	
	if (TempImage->Origin != IL_ORIGIN_LOWER_LEFT) {
		ifree(TempData);
	}
	if (Format == IL_RGB || Format == IL_RGBA) {
		ilSwapColours(Image);
	}
	
	if (TempPal != &Image->Pal && TempPal != NULL) {
		ifree(TempPal->Palette);
		ifree(TempPal);
	}
	
	if (TempImage != Image)
		ilCloseImage(TempImage);

	return IL_TRUE;
}


// Only to be called by ilDetermineSize.  Returns the buffer size needed to save the
//  current image as a Targa file.
ILuint iTargaSize(ILimage *Image)
{
	ILuint	Size, Bpp;
	ILubyte	IDLen = 0;
	const char	*ID = iGetString(IL_TGA_ID_STRING);
	const char	*AuthName = iGetString(IL_TGA_AUTHNAME_STRING);
	const char	*AuthComment = iGetString(IL_TGA_AUTHCOMMENT_STRING);

	//@TODO: Support color indexed images.
	if (iGetInt(IL_TGA_RLE) == IL_TRUE || Image->Format == IL_COLOUR_INDEX) {
		// Use the slower method, since we are using compression.  We do a "fake" write.
		ilSaveTargaL(Image, NULL, 0);
	}

	if (ID)
		IDLen = (ILubyte)ilCharStrLen(ID);

	Size = 18 + IDLen;  // Header + ID

	// Bpp may not be Image->Bpp.
	switch (Image->Format)
	{
		case IL_BGR:
		case IL_RGB:
			Bpp = 3;
			break;
		case IL_BGRA:
		case IL_RGBA:
			Bpp = 4;
			break;
		case IL_LUMINANCE:
			Bpp = 1;
			break;
		default:  //@TODO: Do not know what to do with the others yet.
			return 0;
	}

	Size += Image->Width * Image->Height * Bpp;
	Size += 532;  // Size of the extension area

	return Size;
}


/*// Makes a neat string to go into the id field of the .tga
void iMakeString(char *Str)
{
	char		*PSG = "Generated by Developer's Image Library: ";
	char		TimeStr[255];
	
	time_t		Time;
	struct tm	*CurTime;
	
	time(&Time);
#ifdef _WIN32
	_tzset();
#endif
	CurTime = localtime(&Time);
	
	strftime(TimeStr, 255 - ilCharStrLen(PSG), "%#c (%z)", CurTime);
	//strftime(TimeStr, 255 - ilCharStrLen(PSG), "%C (%Z)", CurTime);
	sprintf(Str, "%s%s", PSG, TimeStr);
	
	return;
}*/


//changed name to iGetDateTime on 20031221 to fix bug 830196
void iGetDateTime(ILuint *Month, ILuint *Day, ILuint *Yr, ILuint *Hr, ILuint *Min, ILuint *Sec)
{
#ifdef DJGPP	
	struct date day;
	struct time curtime;
	
	gettime(&curtime);
	getdate(&day);
	
	*Month = day.da_mon;
	*Day = day.da_day;
	*Yr = day.da_year;
	
	*Hr = curtime.ti_hour;
	*Min = curtime.ti_min;
	*Sec = curtime.ti_sec;
	
	return;
#else
	
#ifdef _WIN32
	SYSTEMTIME Time;
	
	GetSystemTime(&Time);
	
	*Month = Time.wMonth;
	*Day = Time.wDay;
	*Yr = Time.wYear;
	
	*Hr = Time.wHour;
	*Min = Time.wMinute;
	*Sec = Time.wSecond;
	
	return;
#else
	
	*Month = 0;
	*Day = 0;
	*Yr = 0;
	
	*Hr = 0;
	*Min = 0;
	*Sec = 0;
	
	return;
#endif
#endif
}


#endif//IL_NO_TGA
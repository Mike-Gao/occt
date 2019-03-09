// Copyright (c) 2019 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifdef HAVE_FREEIMAGE
  #include <FreeImage.h>

  #ifdef _MSC_VER
    #pragma comment( lib, "FreeImage.lib" )
  #endif
#endif

#include <Image_FreeImage.hxx>

#include <gp.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_OpenFile.hxx>
#include <fstream>
#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(Image_FreeImage, Image_AlienPixMapI)

namespace
{
#ifdef HAVE_FREEIMAGE
  static Image_Format convertFromFreeFormat (FREE_IMAGE_TYPE       theFormatFI,
                                             FREE_IMAGE_COLOR_TYPE theColorTypeFI,
                                             unsigned              theBitsPerPixel)
  {
    switch (theFormatFI)
    {
      case FIT_RGBF:   return Image_Format_RGBF;
      case FIT_RGBAF:  return Image_Format_RGBAF;
      case FIT_FLOAT:  return Image_Format_GrayF;
      case FIT_BITMAP:
      {
        switch (theColorTypeFI)
        {
          case FIC_MINISBLACK:
          {
            return Image_Format_Gray;
          }
          case FIC_RGB:
          {
            if (Image_PixMap::IsBigEndianHost())
            {
              return (theBitsPerPixel == 32) ? Image_Format_RGB32 : Image_Format_RGB;
            }
            else
            {
              return (theBitsPerPixel == 32) ? Image_Format_BGR32 : Image_Format_BGR;
            }
          }
          case FIC_RGBALPHA:
          {
            return Image_PixMap::IsBigEndianHost() ? Image_Format_RGBA : Image_Format_BGRA;
          }
          default:
            return Image_Format_UNKNOWN;
        }
      }
      default:
        return Image_Format_UNKNOWN;
    }
  }

  static FREE_IMAGE_TYPE convertToFreeFormat (Image_Format theFormat)
  {
    switch (theFormat)
    {
      case Image_Format_GrayF:
      case Image_Format_AlphaF:
        return FIT_FLOAT;
      case Image_Format_RGBAF:
        return FIT_RGBAF;
      case Image_Format_RGBF:
        return FIT_RGBF;
      case Image_Format_RGBA:
      case Image_Format_BGRA:
      case Image_Format_RGB32:
      case Image_Format_BGR32:
      case Image_Format_RGB:
      case Image_Format_BGR:
      case Image_Format_Gray:
      case Image_Format_Alpha:
        return FIT_BITMAP;
      default:
        return FIT_UNKNOWN;
    }
  }

  //! Wrapper for accessing C++ stream from FreeImage.
  class Image_FreeImageStream
  {
  public:
    //! Construct wrapper over input stream.
    Image_FreeImageStream (std::istream& theStream)
    : myIStream (&theStream), myOStream (NULL), myInitPos (theStream.tellg()) {}

    //! Get io object.
    FreeImageIO GetFiIO() const
    {
      FreeImageIO anIo;
      memset (&anIo, 0, sizeof(anIo));
      if (myIStream != NULL)
      {
        anIo.read_proc = readProc;
        anIo.seek_proc = seekProc;
        anIo.tell_proc = tellProc;
      }
      if (myOStream != NULL)
      {
        anIo.write_proc = writeProc;
      }
      return anIo;
    }
  public:
    //! Simulate fread().
    static unsigned int DLL_CALLCONV readProc (void* theBuffer, unsigned int theSize, unsigned int theCount, fi_handle theHandle)
    {
      Image_FreeImageStream* aThis = (Image_FreeImageStream* )theHandle;
      if (aThis->myIStream == NULL)
      {
        return 0;
      }

      if (!aThis->myIStream->read ((char* )theBuffer, std::streamsize(theSize) * std::streamsize(theCount)))
      {
        //aThis->myIStream->clear();
      }
      const std::streamsize aNbRead = aThis->myIStream->gcount();
      return (unsigned int )(aNbRead / theSize);
    }

    //! Simulate fwrite().
    static unsigned int DLL_CALLCONV writeProc (void* theBuffer, unsigned int theSize, unsigned int theCount, fi_handle theHandle)
    {
      Image_FreeImageStream* aThis = (Image_FreeImageStream* )theHandle;
      if (aThis->myOStream != NULL
       && aThis->myOStream->write ((const char* )theBuffer, std::streamsize(theSize) * std::streamsize(theCount)))
      {
        return theCount;
      }
      return 0;
    }

    //! Simulate fseek().
    static int DLL_CALLCONV seekProc (fi_handle theHandle, long theOffset, int theOrigin)
    {
      Image_FreeImageStream* aThis = (Image_FreeImageStream* )theHandle;
      if (aThis->myIStream == NULL)
      {
        return -1;
      }

      bool isSeekDone = false;
      switch (theOrigin)
      {
        case SEEK_SET:
          if (aThis->myIStream->seekg ((std::streamoff )aThis->myInitPos + theOffset, std::ios::beg))
          {
            isSeekDone = true;
          }
          break;
        case SEEK_CUR:
          if (aThis->myIStream->seekg (theOffset, std::ios::cur))
          {
            isSeekDone = true;
          }
          break;
        case SEEK_END:
          if (aThis->myIStream->seekg (theOffset, std::ios::end))
          {
            isSeekDone = true;
          }
          break;
      }
      return isSeekDone ? 0 : -1;
    }

    //! Simulate ftell().
    static long DLL_CALLCONV tellProc (fi_handle theHandle)
    {
      Image_FreeImageStream* aThis = (Image_FreeImageStream* )theHandle;
      const long aPos = aThis->myIStream != NULL ? (long )(aThis->myIStream->tellg() - aThis->myInitPos) : 0;
      return aPos;
    }
  private:
    std::istream*  myIStream;
    std::ostream*  myOStream;
    std::streampos myInitPos;
  };
#endif
}

// =======================================================================
// function : IsAvailable
// purpose  :
// =======================================================================
bool Image_FreeImage::IsAvailable()
{
#if defined(HAVE_FREEIMAGE)
  return true;
#else
  return false;
#endif
}

// =======================================================================
// function : Image_FreeImage
// purpose  :
// =======================================================================
Image_FreeImage::Image_FreeImage()
: myLibImage (NULL)
{
  SetTopDown (false);
}

// =======================================================================
// function : ~Image_FreeImage
// purpose  :
// =======================================================================
Image_FreeImage::~Image_FreeImage()
{
  Clear();
}

// =======================================================================
// function : InitTrash
// purpose  :
// =======================================================================
bool Image_FreeImage::InitTrash (Image_Format        thePixelFormat,
                                 const Standard_Size theSizeX,
                                 const Standard_Size theSizeY,
                                 const Standard_Size theSizeRowBytes)
{
  Clear();
#ifdef HAVE_FREEIMAGE
  (void )theSizeRowBytes;
  FREE_IMAGE_TYPE aFormatFI = convertToFreeFormat (thePixelFormat);
  int aBitsPerPixel = (int )Image_PixMap::SizePixelBytes (thePixelFormat) * 8;
  if (aFormatFI == FIT_UNKNOWN)
  {
    aFormatFI     = FIT_BITMAP;
    aBitsPerPixel = 24;
  }

  FIBITMAP* anImage = FreeImage_AllocateT (aFormatFI, (int )theSizeX, (int )theSizeY, aBitsPerPixel);
  Image_Format aFormat = convertFromFreeFormat (FreeImage_GetImageType(anImage),
                                                FreeImage_GetColorType(anImage),
                                                FreeImage_GetBPP      (anImage));
  if (thePixelFormat == Image_Format_BGR32
   || thePixelFormat == Image_Format_RGB32)
  {
    //FreeImage_SetTransparent (anImage, FALSE);
    aFormat = (aFormat == Image_Format_BGRA) ? Image_Format_BGR32 : Image_Format_RGB32;
  }

  initWrapper (aFormat, FreeImage_GetBits (anImage),
               FreeImage_GetWidth (anImage), FreeImage_GetHeight (anImage), FreeImage_GetPitch (anImage));
  SetTopDown (false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
#else
  return base_type::InitTrash (thePixelFormat, theSizeX, theSizeY, theSizeRowBytes);
#endif
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Image_FreeImage::Clear()
{
  base_type::Clear();
#ifdef HAVE_FREEIMAGE
  if (myLibImage != NULL)
  {
    FreeImage_Unload (myLibImage);
    myLibImage = NULL;
  }
#endif
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
bool Image_FreeImage::Load (const Standard_Byte* theData,
                            Standard_Size theLength,
                            const TCollection_AsciiString& theImagePath)
{
  Clear();
#ifdef HAVE_FREEIMAGE
#ifdef _WIN32
  const TCollection_ExtendedString aFileNameW (theImagePath);
#endif
  FREE_IMAGE_FORMAT aFIF = FIF_UNKNOWN;
  FIMEMORY* aFiMem = NULL;
  if (theData != NULL)
  {
    aFiMem = FreeImage_OpenMemory ((BYTE* )theData, (DWORD )theLength);
    aFIF = FreeImage_GetFileTypeFromMemory (aFiMem, 0);
  }
  else
  {
  #ifdef _WIN32
    aFIF = FreeImage_GetFileTypeU (aFileNameW.ToWideString(), 0);
  #else
    aFIF = FreeImage_GetFileType (theImagePath.ToCString(), 0);
  #endif
  }
  if (aFIF == FIF_UNKNOWN)
  {
    // no signature? try to guess the file format from the file extension
    aFIF = FreeImage_GetFIFFromFilename (theImagePath.ToCString());
  }
  if ((aFIF == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading (aFIF))
  {
    ::Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: image '") + theImagePath + "' has unsupported file format.",
                                         Message_Fail);
    if (aFiMem != NULL)
    {
      FreeImage_CloseMemory (aFiMem);
    }
    return false;
  }

  int aLoadFlags = 0;
  if (aFIF == FIF_GIF)
  {
    // 'Play' the GIF to generate each frame (as 32bpp) instead of returning raw frame data when loading
    aLoadFlags = GIF_PLAYBACK;
  }
  else if (aFIF == FIF_ICO)
  {
    // convert to 32bpp and create an alpha channel from the AND-mask when loading
    aLoadFlags = ICO_MAKEALPHA;
  }

  FIBITMAP* anImage = NULL;
  if (theData != NULL)
  {
    anImage = FreeImage_LoadFromMemory (aFIF, aFiMem, aLoadFlags);
    FreeImage_CloseMemory (aFiMem);
    aFiMem = NULL;
  }
  else
  {
  #ifdef _WIN32
    anImage = FreeImage_LoadU (aFIF, aFileNameW.ToWideString(), aLoadFlags);
  #else
    anImage = FreeImage_Load  (aFIF, theImagePath.ToCString(), aLoadFlags);
  #endif
  }
  if (anImage == NULL)
  {
    TCollection_AsciiString aMessage = "Error: image file '";
    aMessage.AssignCat (theImagePath);
    aMessage.AssignCat ("' is missing or invalid.");
    ::Message::DefaultMessenger()->Send (aMessage, Message_Fail);
    return false;
  }

  Image_Format aFormat = convertFromFreeFormat (FreeImage_GetImageType(anImage),
                                                FreeImage_GetColorType(anImage),
                                                FreeImage_GetBPP      (anImage));
  if (aFormat == Image_Format_UNKNOWN)
  {
    //anImage = FreeImage_ConvertTo24Bits (anImage);
    ::Message::DefaultMessenger()->Send (    TCollection_AsciiString ("Error: image '") + theImagePath + "' has unsupported pixel format.",
                                         Message_Fail);
    return false;
  }

  initWrapper (aFormat, FreeImage_GetBits (anImage),
               FreeImage_GetWidth (anImage), FreeImage_GetHeight (anImage), FreeImage_GetPitch (anImage));
  SetTopDown (false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
#else
  (void )theData;
  (void )theLength;
  (void )theImagePath;
  Message::DefaultMessenger()->Send ("Error: FreeImage library is unavailable", Message_Fail);
  return false;
#endif
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
bool Image_FreeImage::Load (std::istream& theStream,
                            const TCollection_AsciiString& theFileName)
{
  Clear();
#ifdef HAVE_FREEIMAGE
  Image_FreeImageStream aStream (theStream);
  FreeImageIO aFiIO = aStream.GetFiIO();

  FREE_IMAGE_FORMAT aFIF = FreeImage_GetFileTypeFromHandle (&aFiIO, &aStream, 0);
  if (aFIF == FIF_UNKNOWN)
  {
    // no signature? try to guess the file format from the file extension
    aFIF = FreeImage_GetFIFFromFilename (theFileName.ToCString());
  }
  if ((aFIF == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading (aFIF))
  {
    ::Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: image stream '") + theFileName + "' has unsupported file format.",
                                         Message_Fail);
    return false;
  }

  int aLoadFlags = 0;
  if (aFIF == FIF_GIF)
  {
    // 'Play' the GIF to generate each frame (as 32bpp) instead of returning raw frame data when loading
    aLoadFlags = GIF_PLAYBACK;
  }
  else if (aFIF == FIF_ICO)
  {
    // convert to 32bpp and create an alpha channel from the AND-mask when loading
    aLoadFlags = ICO_MAKEALPHA;
  }

  FIBITMAP* anImage = FreeImage_LoadFromHandle (aFIF, &aFiIO, &aStream, aLoadFlags);
  if (anImage == NULL)
  {
    ::Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: image stream '") + theFileName + "' is missing or invalid.",
                                         Message_Fail);
    return false;
  }

  Image_Format aFormat = convertFromFreeFormat (FreeImage_GetImageType(anImage),
                                                FreeImage_GetColorType(anImage),
                                                FreeImage_GetBPP      (anImage));
  if (aFormat == Image_Format_UNKNOWN)
  {
    ::Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: image stream '") + theFileName + "' has unsupported pixel format.",
                                         Message_Fail);
    return false;
  }

  initWrapper (aFormat, FreeImage_GetBits (anImage),
               FreeImage_GetWidth (anImage), FreeImage_GetHeight (anImage), FreeImage_GetPitch (anImage));
  SetTopDown (false);

  // assign image after wrapper initialization (virtual Clear() called inside)
  myLibImage = anImage;
  return true;
#else
  (void )theStream;
  (void )theFileName;
  Message::DefaultMessenger()->Send ("Error: FreeImage library is unavailable", Message_Fail);
  return false;
#endif
}

// =======================================================================
// function : Save
// purpose  :
// =======================================================================
bool Image_FreeImage::Save (const TCollection_AsciiString& theFileName)
{
#ifdef HAVE_FREEIMAGE
  if (myLibImage == NULL)
  {
    return false;
  }

#ifdef _WIN32
  const TCollection_ExtendedString aFileNameW (theFileName.ToCString(), Standard_True);
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilenameU (aFileNameW.ToWideString());
#else
  FREE_IMAGE_FORMAT anImageFormat = FreeImage_GetFIFFromFilename (theFileName.ToCString());
#endif
  if (anImageFormat == FIF_UNKNOWN)
  {
#ifdef OCCT_DEBUG
    std::cerr << "Image_FreeImage, image format doesn't supported!\n";
#endif
    return false;
  }

  if (IsTopDown())
  {
    FreeImage_FlipVertical (myLibImage);
    SetTopDown (false);
  }

  // FreeImage doesn't provide flexible format conversion API
  // so we should perform multiple conversions in some cases!
  FIBITMAP* anImageToDump = myLibImage;
  switch (anImageFormat)
  {
    case FIF_PNG:
    case FIF_BMP:
    {
      if (Format() == Image_Format_BGR32
       || Format() == Image_Format_RGB32)
      {
        // stupid FreeImage treats reserved byte as alpha if some bytes not set to 0xFF
        for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
        {
          for (Standard_Size aCol = 0; aCol < SizeX(); ++aCol)
          {
            myData.ChangeValue (aRow, aCol)[3] = 0xFF;
          }
        }
      }
      else if (FreeImage_GetImageType (myLibImage) != FIT_BITMAP)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_BITMAP);
      }
      break;
    }
    case FIF_GIF:
    {
      FIBITMAP* aTmpBitmap = myLibImage;
      if (FreeImage_GetImageType (myLibImage) != FIT_BITMAP)
      {
        aTmpBitmap = FreeImage_ConvertToType (myLibImage, FIT_BITMAP);
        if (aTmpBitmap == NULL)
        {
          return false;
        }
      }

      if (FreeImage_GetBPP (aTmpBitmap) != 24)
      {
        FIBITMAP* aTmpBitmap24 = FreeImage_ConvertTo24Bits (aTmpBitmap);
        if (aTmpBitmap != myLibImage)
        {
          FreeImage_Unload (aTmpBitmap);
        }
        if (aTmpBitmap24 == NULL)
        {
          return false;
        }
        aTmpBitmap = aTmpBitmap24;
      }

      // need conversion to image with palette (requires 24bit bitmap)
      anImageToDump = FreeImage_ColorQuantize (aTmpBitmap, FIQ_NNQUANT);
      if (aTmpBitmap != myLibImage)
      {
        FreeImage_Unload (aTmpBitmap);
      }
      break;
    }
    case FIF_HDR:
    case FIF_EXR:
    {
      if (Format() == Image_Format_Gray
       || Format() == Image_Format_Alpha)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_FLOAT);
      }
      else if (Format() == Image_Format_RGBA
            || Format() == Image_Format_BGRA)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_RGBAF);
      }
      else
      {
        FREE_IMAGE_TYPE aImgTypeFI = FreeImage_GetImageType (myLibImage);
        if (aImgTypeFI != FIT_RGBF
         && aImgTypeFI != FIT_RGBAF
         && aImgTypeFI != FIT_FLOAT)
        {
          anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_RGBF);
        }
      }
      break;
    }
    default:
    {
      if (FreeImage_GetImageType (myLibImage) != FIT_BITMAP)
      {
        anImageToDump = FreeImage_ConvertToType (myLibImage, FIT_BITMAP);
        if (anImageToDump == NULL)
        {
          return false;
        }
      }

      if (FreeImage_GetBPP (anImageToDump) != 24)
      {
        FIBITMAP* aTmpBitmap24 = FreeImage_ConvertTo24Bits (anImageToDump);
        if (anImageToDump != myLibImage)
        {
          FreeImage_Unload (anImageToDump);
        }
        if (aTmpBitmap24 == NULL)
        {
          return false;
        }
        anImageToDump = aTmpBitmap24;
      }
      break;
    }
  }

  if (anImageToDump == NULL)
  {
    return false;
  }

#ifdef _WIN32
  bool isSaved = (FreeImage_SaveU (anImageFormat, anImageToDump, aFileNameW.ToWideString()) != FALSE);
#else
  bool isSaved = (FreeImage_Save  (anImageFormat, anImageToDump, theFileName.ToCString()) != FALSE);
#endif
  if (anImageToDump != myLibImage)
  {
    FreeImage_Unload (anImageToDump);
  }
  return isSaved;
#else
  (void )theFileName;
  Message::DefaultMessenger()->Send ("Error: FreeImage library is unavailable", Message_Fail);
  return false;
#endif
}

// =======================================================================
// function : AdjustGamma
// purpose  :
// =======================================================================
bool Image_FreeImage::AdjustGamma (const Standard_Real theGammaCorr)
{
#ifdef HAVE_FREEIMAGE
  return FreeImage_AdjustGamma (myLibImage, theGammaCorr) != FALSE;
#else
  (void )theGammaCorr;
  return false;
#endif
}

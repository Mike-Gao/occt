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

#if !defined(HAVE_FREEIMAGE) && defined(_WIN32)
  #define HAVE_WINCODEC
#endif

#if defined(HAVE_WINCODEC)
  //#include <initguid.h>
  #include <wincodec.h>
  #undef min
  #undef max

  #ifdef _MSC_VER
    #pragma comment( lib, "Ole32.lib" )
  #endif
#endif

#include <Image_WinCodec.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_OpenFile.hxx>
#include <fstream>
#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(Image_WinCodec, Image_AlienPixMapI)

namespace
{
#if defined(HAVE_WINCODEC)

  //! Return a zero GUID
  static GUID getNullGuid()
  {
    GUID aGuid = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
    return aGuid;
  }

  //! Sentry over IUnknown pointer.
  template<class T> class Image_ComPtr
  {
  public:
    //! Empty constructor.
    Image_ComPtr()
    : myPtr (NULL) {}

    //! Destructor.
    ~Image_ComPtr()
    {
      Nullify();
    }

    //! Return TRUE if pointer is NULL.
    bool IsNull() const { return myPtr == NULL; }

    //! Release the pointer.
    void Nullify()
    {
      if (myPtr != NULL)
      {
        myPtr->Release();
        myPtr = NULL;
      }
    }

    //! Return pointer for initialization.
    T*& ChangePtr()
    {
      Standard_ASSERT_RAISE (myPtr == NULL, "Pointer cannot be initialized twice!");
      return myPtr;
    }

    //! Return pointer.
    T* get() { return myPtr; }

    //! Return pointer.
    T* operator->() { return get(); }

    //! Cast handle to contained type
    T& operator*() { return *get(); }

  private:
    T* myPtr;
  };

  //! Convert WIC GUID to Image_Format.
  static Image_Format convertFromWicFormat (const WICPixelFormatGUID& theFormat)
  {
    if (theFormat == GUID_WICPixelFormat32bppBGRA)
    {
      return Image_Format_BGRA;
    }
    else if (theFormat == GUID_WICPixelFormat32bppBGR)
    {
      return Image_Format_BGR32;
    }
    else if (theFormat == GUID_WICPixelFormat24bppRGB)
    {
      return Image_Format_RGB;
    }
    else if (theFormat == GUID_WICPixelFormat24bppBGR)
    {
      return Image_Format_BGR;
    }
    else if (theFormat == GUID_WICPixelFormat8bppGray)
    {
      return Image_Format_Gray;
    }
    return Image_Format_UNKNOWN;
  }

  //! Convert Image_Format to WIC GUID.
  static WICPixelFormatGUID convertToWicFormat (Image_Format theFormat)
  {
    switch (theFormat)
    {
      case Image_Format_BGRA:   return GUID_WICPixelFormat32bppBGRA;
      case Image_Format_BGR32:  return GUID_WICPixelFormat32bppBGR;
      case Image_Format_RGB:    return GUID_WICPixelFormat24bppRGB;
      case Image_Format_BGR:    return GUID_WICPixelFormat24bppBGR;
      case Image_Format_Gray:   return GUID_WICPixelFormat8bppGray;
      case Image_Format_Alpha:  return GUID_WICPixelFormat8bppGray; // GUID_WICPixelFormat8bppAlpha
      case Image_Format_GrayF:  // GUID_WICPixelFormat32bppGrayFloat
      case Image_Format_AlphaF:
      case Image_Format_RGBAF:  // GUID_WICPixelFormat128bppRGBAFloat
      case Image_Format_RGBF:   // GUID_WICPixelFormat96bppRGBFloat
      case Image_Format_RGBA:   // GUID_WICPixelFormat32bppRGBA
      case Image_Format_RGB32:  // GUID_WICPixelFormat32bppRGB
      default:
        return getNullGuid();
    }
  }

#endif
}

// =======================================================================
// function : IsAvailable
// purpose  :
// =======================================================================
bool Image_WinCodec::IsAvailable()
{
#if defined(HAVE_WINCODEC)
  return true;
#else
  return false;
#endif
}

// =======================================================================
// function : Image_WinCodec
// purpose  :
// =======================================================================
Image_WinCodec::Image_WinCodec()
{
  SetTopDown (true);
}

// =======================================================================
// function : ~Image_WinCodec
// purpose  :
// =======================================================================
Image_WinCodec::~Image_WinCodec()
{
  Clear();
}

// =======================================================================
// function : InitTrash
// purpose  :
// =======================================================================
bool Image_WinCodec::InitTrash (Image_Format        thePixelFormat,
                                const Standard_Size theSizeX,
                                const Standard_Size theSizeY,
                                const Standard_Size theSizeRowBytes)
{
  Clear();

  Image_Format aFormat = thePixelFormat;
  switch (aFormat)
  {
    case Image_Format_RGB:
      aFormat = Image_Format_BGR;
      break;
    case Image_Format_RGB32:
      aFormat = Image_Format_BGR32;
      break;
    case Image_Format_RGBA:
      aFormat = Image_Format_BGRA;
      break;
    default:
      break;
  }

  if (!base_type::InitTrash (aFormat, theSizeX, theSizeY, theSizeRowBytes))
  {
    return false;
  }
  SetTopDown (true);
  return true;
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
bool Image_WinCodec::Load (const Standard_Byte* theData,
                           Standard_Size theLength,
                           const TCollection_AsciiString& theFileName)
{
  Clear();
#if defined(HAVE_WINCODEC)
  Image_ComPtr<IWICImagingFactory> aWicImgFactory;
  CoInitializeEx (NULL, COINIT_MULTITHREADED);
  if (CoCreateInstance (CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&aWicImgFactory.ChangePtr())) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot initialize WIC Imaging Factory", Message_Fail);
    return false;
  }

  Image_ComPtr<IWICBitmapDecoder> aWicDecoder;
  Image_ComPtr<IWICStream> aWicStream;
  if (theData != NULL)
  {
    if (aWicImgFactory->CreateStream (&aWicStream.ChangePtr()) != S_OK
     || aWicStream->InitializeFromMemory ((BYTE* )theData, (DWORD )theLength) != S_OK)
    {
      Message::DefaultMessenger()->Send ("Error: cannot initialize WIC Stream", Message_Fail);
      return false;
    }
    if (aWicImgFactory->CreateDecoderFromStream (aWicStream.get(), NULL, WICDecodeMetadataCacheOnDemand, &aWicDecoder.ChangePtr()) != S_OK)
    {
      Message::DefaultMessenger()->Send ("Error: cannot create WIC Image Decoder", Message_Fail);
      return false;
    }
  }
  else
  {
    const TCollection_ExtendedString aFileNameW (theFileName);
    if (aWicImgFactory->CreateDecoderFromFilename (aFileNameW.ToWideString(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &aWicDecoder.ChangePtr()) != S_OK)
    {
      Message::DefaultMessenger()->Send ("Error: cannot create WIC Image Decoder", Message_Fail);
      return false;
    }
  }

  UINT aFrameCount = 0, aFrameSizeX = 0, aFrameSizeY = 0;
  WICPixelFormatGUID aWicPixelFormat = getNullGuid();
  Image_ComPtr<IWICBitmapFrameDecode> aWicFrameDecode;
  if (aWicDecoder->GetFrameCount (&aFrameCount) != S_OK
   || aFrameCount < 1
   || aWicDecoder->GetFrame (0, &aWicFrameDecode.ChangePtr()) != S_OK
   || aWicFrameDecode->GetSize (&aFrameSizeX, &aFrameSizeY) != S_OK
   || aWicFrameDecode->GetPixelFormat (&aWicPixelFormat))
  {
    Message::DefaultMessenger()->Send ("Error: cannot get WIC Image Frame", Message_Fail);
    return false;
  }

  Image_ComPtr<IWICFormatConverter> aWicConvertedFrame;
  Image_Format aPixelFormat = convertFromWicFormat (aWicPixelFormat);
  if (aPixelFormat == Image_Format_UNKNOWN)
  {
    aPixelFormat = Image_Format_RGB;
    if (aWicImgFactory->CreateFormatConverter (&aWicConvertedFrame.ChangePtr()) != S_OK
     || aWicConvertedFrame->Initialize (aWicFrameDecode.get(), convertToWicFormat (aPixelFormat), WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom) != S_OK)
    {
      Message::DefaultMessenger()->Send ("Error: cannot convert WIC Image Frame to RGB format", Message_Fail);
      return false;
    }
    aWicFrameDecode.Nullify();
  }

  if (!base_type::InitTrash (aPixelFormat, aFrameSizeX, aFrameSizeY))
  {
    Message::DefaultMessenger()->Send ("Error: cannot initialize memory for image", Message_Fail);
    return false;
  }

  IWICBitmapSource* aWicSrc = aWicFrameDecode.get();
  if(!aWicConvertedFrame.IsNull())
  {
    aWicSrc = aWicConvertedFrame.get();
  }
  if (aWicSrc->CopyPixels (NULL, (UINT )SizeRowBytes(), (UINT )SizeBytes(), ChangeData()) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot copy pixels from WIC Image", Message_Fail);
    return false;
  }
  SetTopDown (true);
  return true;
#else
  (void )theData;
  (void )theLength;
  (void )theFileName;
  Message::DefaultMessenger()->Send ("Error: WinCodec image library is unavailable", Message_Fail);
  return false;
#endif
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
bool Image_WinCodec::Load (std::istream& theStream,
                           const TCollection_AsciiString& theFilePath)
{
  return loadStreamToBuffer (theStream, theFilePath);
}

// =======================================================================
// function : Save
// purpose  :
// =======================================================================
bool Image_WinCodec::Save (const TCollection_AsciiString& theFileName)
{
#if defined(HAVE_WINCODEC)
  TCollection_AsciiString aFileNameLower = theFileName;
  aFileNameLower.LowerCase();
  GUID aFileFormat = getNullGuid();
  if (aFileNameLower.EndsWith (".ppm"))
  {
    return savePPM (theFileName);
  }
  else if (aFileNameLower.EndsWith (".bmp"))
  {
    aFileFormat = GUID_ContainerFormatBmp;
  }
  else if (aFileNameLower.EndsWith (".png"))
  {
    aFileFormat = GUID_ContainerFormatPng;
  }
  else if (aFileNameLower.EndsWith (".jpg")
        || aFileNameLower.EndsWith (".jpeg"))
  {
    aFileFormat = GUID_ContainerFormatJpeg;
  }
  else if (aFileNameLower.EndsWith (".tiff"))
  {
    aFileFormat = GUID_ContainerFormatTiff;
  }
  else if (aFileNameLower.EndsWith (".gif"))
  {
    aFileFormat = GUID_ContainerFormatGif;
  }

  if (aFileFormat == getNullGuid())
  {
    Message::DefaultMessenger()->Send ("Error: unsupported image format", Message_Fail);
    return false;
  }

  Image_ComPtr<IWICImagingFactory> aWicImgFactory;
  CoInitializeEx (NULL, COINIT_MULTITHREADED);
  if (CoCreateInstance (CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&aWicImgFactory.ChangePtr())) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot initialize WIC Imaging Factory", Message_Fail);
    return false;
  }

  Image_ComPtr<IWICStream> aWicFileStream;
  Image_ComPtr<IWICBitmapEncoder> aWicEncoder;
  const TCollection_ExtendedString aFileNameW (theFileName);
  if (aWicImgFactory->CreateStream (&aWicFileStream.ChangePtr()) != S_OK
   || aWicFileStream->InitializeFromFilename (aFileNameW.ToWideString(), GENERIC_WRITE) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot create WIC File Stream", Message_Fail);
    return false;
  }
  if (aWicImgFactory->CreateEncoder (aFileFormat, NULL, &aWicEncoder.ChangePtr()) != S_OK
   || aWicEncoder->Initialize (aWicFileStream.get(), WICBitmapEncoderNoCache) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot create WIC Encoder", Message_Fail);
    return false;
  }

  const WICPixelFormatGUID aWicPixelFormat = convertToWicFormat (myImgFormat);
  if (aWicPixelFormat == getNullGuid())
  {
    Message::DefaultMessenger()->Send ("Error: unsupported pixel format", Message_Fail);
    return false;
  }

  WICPixelFormatGUID aWicPixelFormatRes = aWicPixelFormat;
  Image_ComPtr<IWICBitmapFrameEncode> aWicFrameEncode;
  if (aWicEncoder->CreateNewFrame (&aWicFrameEncode.ChangePtr(), NULL) != S_OK
   || aWicFrameEncode->Initialize (NULL) != S_OK
   || aWicFrameEncode->SetSize ((UINT )SizeX(), (UINT )SizeY()) != S_OK
   || aWicFrameEncode->SetPixelFormat (&aWicPixelFormatRes) != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot create WIC Frame", Message_Fail);
    return false;
  }

  if (aWicPixelFormatRes != aWicPixelFormat)
  {
    Message::DefaultMessenger()->Send ("Error: pixel format is unsupported by image format", Message_Fail);
    return false;
  }

  if (IsTopDown())
  {
    if (aWicFrameEncode->WritePixels ((UINT )SizeY(), (UINT )SizeRowBytes(), (UINT )SizeBytes(), (BYTE* )Data()) != S_OK)
    {
      Message::DefaultMessenger()->Send ("Error: cannot write pixels to WIC Frame", Message_Fail);
      return false;
    }
  }
  else
  {
    for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
    {
      if (aWicFrameEncode->WritePixels (1, (UINT )SizeRowBytes(), (UINT )SizeRowBytes(), (BYTE* )Row (aRow)) != S_OK)
      {
        Message::DefaultMessenger()->Send ("Error: cannot write pixels to WIC Frame", Message_Fail);
        return false;
      }
    }
  }

  if (aWicFrameEncode->Commit() != S_OK
   || aWicEncoder->Commit() != S_OK)
  {
    Message::DefaultMessenger()->Send ("Error: cannot commit data to WIC Frame", Message_Fail);
    return false;
  }
  if (aWicFileStream->Commit (STGC_DEFAULT) != S_OK)
  {
    //Message::DefaultMessenger()->Send ("Error: cannot commit data to WIC File Stream", Message_Fail);
    //return false;
  }
  return true;
#else
  (void )theFileName;
  Message::DefaultMessenger()->Send ("Error: WinCodec image library is unavailable", Message_Fail);
  return false;
#endif
}

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

#ifndef _Image_FreeImage_HeaderFile
#define _Image_FreeImage_HeaderFile

#include <Image_AlienPixMapI.hxx>

struct FIBITMAP;

//! Image class that implements file reading/writing operations using FreeImage library.
//! Supported image formats:
//! - *.bmp - bitmap image, lossless format without compression.
//! - *.ppm - PPM (Portable Pixmap Format), lossless format without compression.
//! - *.png - PNG (Portable Network Graphics) lossless format with compression.
//! - *.jpg, *.jpe, *.jpeg - JPEG/JIFF (Joint Photographic Experts Group) lossy format (compressed with quality losses). YUV color space used (automatically converted from/to RGB).
//! - *.tif, *.tiff - TIFF (Tagged Image File Format).
//! - *.tga - TGA (Truevision Targa Graphic), lossless format.
//! - *.gif - GIF (Graphical Interchange Format), lossy format. Color stored using palette (up to 256 distinct colors).
//! - *.exr - OpenEXR high dynamic-range format (supports float pixel formats). 
class Image_FreeImage : public Image_AlienPixMapI
{
  DEFINE_STANDARD_RTTIEXT(Image_FreeImage, Image_AlienPixMapI)
public:

  //! Return TRUE if FreeImage library is available.
  Standard_EXPORT static bool IsAvailable();

  //! Return default rows order used by FreeImage library, which is Bottom-Up.
  static bool IsTopDownDefault() { return false; }
public:

  //! Empty constructor.
  Standard_EXPORT Image_FreeImage();

  //! Destructor
  Standard_EXPORT virtual ~Image_FreeImage();

  using Image_AlienPixMapI::Load;

  //! Read image data from stream.
  Standard_EXPORT virtual bool Load (std::istream& theStream,
                                     const TCollection_AsciiString& theFileName) Standard_OVERRIDE;

  //! Read image data from memory buffer.
  //! @param theData     memory pointer to read from;
  //!                    when NULL, function will attempt to open theFileName file
  //! @param theLength   memory buffer length
  //! @param theFileName optional file name
  Standard_EXPORT virtual bool Load (const Standard_Byte* theData,
                                     Standard_Size theLength,
                                     const TCollection_AsciiString& theFileName) Standard_OVERRIDE;

  //! Write image data to file using file extension to determine compression format.
  Standard_EXPORT virtual bool Save (const TCollection_AsciiString& theFileName) Standard_OVERRIDE;

  //! Initialize image plane with required dimensions.
  //! @param thePixelFormat if specified pixel format doesn't supported by image library
  //!                       than nearest supported will be used instead!
  //! @param theSizeX image width
  //! @param theSizeY image height
  //! @param theSizeRowBytes ignored parameter, 4-bytes alignment is enforced by FreeImage library
  Standard_EXPORT virtual bool InitTrash (Image_Format        thePixelFormat,
                                          const Standard_Size theSizeX,
                                          const Standard_Size theSizeY,
                                          const Standard_Size theSizeRowBytes = 0) Standard_OVERRIDE;

  //! Method correctly deallocate internal buffer.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Performs gamma correction on image.
  //! theGamma - gamma value to use; a value of 1.0 leaves the image alone
  Standard_EXPORT virtual bool AdjustGamma (Standard_Real theGammaCorr) Standard_OVERRIDE;

public:

  //! Create default instance of this class.
  virtual Handle(Image_AlienPixMapI) createDefault() const Standard_OVERRIDE
  {
    return new Image_FreeImage();
  }

private:

  //! Copying allowed only within Handles
  Image_FreeImage            (const Image_FreeImage& );
  Image_FreeImage& operator= (const Image_FreeImage& );

private:

  FIBITMAP* myLibImage;

};

DEFINE_STANDARD_HANDLE(Image_FreeImage, Image_AlienPixMapI)

#endif // _Image_FreeImage_HeaderFile

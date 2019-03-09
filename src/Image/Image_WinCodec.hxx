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

#ifndef _Image_WinCodec_HeaderFile
#define _Image_WinCodec_HeaderFile

#include <Image_AlienPixMapI.hxx>

//! Image class that implements file reading/writing operations using WinCodec image library.
//! Supported image formats:
//! - *.bmp, *.png, *.jpg, *.tiff, *.gif.
class Image_WinCodec : public Image_AlienPixMapI
{
  DEFINE_STANDARD_RTTIEXT(Image_WinCodec, Image_AlienPixMapI)
public:

  //! Return TRUE if WinCodec library is available.
  Standard_EXPORT static bool IsAvailable();

  //! Return default rows order used by WinCodec library, which is Top-Down.
  static bool IsTopDownDefault() { return true; }
public:

  //! Empty constructor.
  Standard_EXPORT Image_WinCodec();

  //! Destructor
  Standard_EXPORT virtual ~Image_WinCodec();

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
  //! @param theSizeRowBytes may be ignored by this class and required alignment will be used instead!
  Standard_EXPORT virtual bool InitTrash (Image_Format        thePixelFormat,
                                          const Standard_Size theSizeX,
                                          const Standard_Size theSizeY,
                                          const Standard_Size theSizeRowBytes = 0) Standard_OVERRIDE;

public:

  //! Create default instance of this class.
  virtual Handle(Image_AlienPixMapI) createDefault() const Standard_OVERRIDE
  {
    return new Image_WinCodec();
  }

private:

  //! Copying allowed only within Handles
  Image_WinCodec            (const Image_WinCodec& );
  Image_WinCodec& operator= (const Image_WinCodec& );

};

DEFINE_STANDARD_HANDLE(Image_WinCodec, Image_AlienPixMapI)

#endif // _Image_WinCodec_HeaderFile

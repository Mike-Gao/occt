// Created on: 2012-07-18
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _Image_AlienPixMap_H__
#define _Image_AlienPixMap_H__

#include <Image_AlienPixMapI.hxx>

//! Image class that support file reading/writing operations using auxiliary image library.
//! This is a wrapper over Image_FreeImage or Image_WinCodec basing on availability.
class Image_AlienPixMap : public Image_AlienPixMapI
{
  DEFINE_STANDARD_RTTIEXT(Image_AlienPixMap, Image_AlienPixMapI)
public:

  //! Return default rows order used by underlying image library.
  Standard_EXPORT static bool IsTopDownDefault();

  //! Create default instance of available image library or NULL if no library available.
  Standard_EXPORT static Handle(Image_AlienPixMapI) CreateDefault();

  //! Setup default image library to be used as factory.
  //! Note that access to the factory is not protected by mutex,
  //! make sure to call this method at the early application startup stage before using.
  //! In this way, application might provide image library replacement implementing
  //! image reading/writing operations which will be used by standard image tools within OCCT
  //! (like image dump or texture loads).
  Standard_EXPORT static void SetDefaultFactory (const Handle(Image_AlienPixMapI)& theLibrary);

public:

  //! Empty constructor.
  Standard_EXPORT Image_AlienPixMap();

  //! Destructor
  Standard_EXPORT virtual ~Image_AlienPixMap();

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

  //! Method correctly deallocate internal buffer.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Performs gamma correction on image.
  //! theGamma - gamma value to use; a value of 1.0 leaves the image alone
  Standard_EXPORT virtual bool AdjustGamma (Standard_Real theGammaCorr) Standard_OVERRIDE;

  //! Setup scanlines order in memory - top-down or bottom-up.
  virtual void SetTopDown (bool theIsTopDown) Standard_OVERRIDE
  {
    if (!myLibImage.IsNull())
    {
      myLibImage->SetTopDown (theIsTopDown);
    }
    base_type::SetTopDown (theIsTopDown);
  }

private:

  //! Return NULL.
  Handle(Image_AlienPixMapI) createDefault() const Standard_OVERRIDE { return Handle(Image_AlienPixMapI)(); }

  //! Copying allowed only within Handles
  Image_AlienPixMap            (const Image_AlienPixMap& );
  Image_AlienPixMap& operator= (const Image_AlienPixMap& );

private:

  Handle(Image_AlienPixMapI) myLibImage;

};

DEFINE_STANDARD_HANDLE(Image_AlienPixMap, Image_PixMap)

#endif // _Image_AlienPixMap_H__

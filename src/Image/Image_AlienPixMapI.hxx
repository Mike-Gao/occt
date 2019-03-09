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

#ifndef _Image_AlienPixMapI_HeaderFile
#define _Image_AlienPixMapI_HeaderFile

#include <Image_PixMap.hxx>
#include <TCollection_AsciiString.hxx>

//! Interface for reading/writing image files.
class Image_AlienPixMapI : public Image_PixMap
{
  DEFINE_STANDARD_RTTIEXT(Image_AlienPixMapI, Image_PixMap)
public:

  //! Empty constructor.
  Image_AlienPixMapI() {}

  //! Read image data from file.
  bool Load (const TCollection_AsciiString& theFileName)
  {
    return Load (NULL, 0, theFileName);
  }

  //! Read image data from stream.
  virtual bool Load (std::istream& theStream,
                     const TCollection_AsciiString& theFileName) = 0;

  //! Read image data from memory buffer.
  //! @param theData     memory pointer to read from;
  //!                    when NULL, function will attempt to open theFileName file
  //! @param theLength   memory buffer length
  //! @param theFileName optional file name
  virtual bool Load (const Standard_Byte* theData,
                     Standard_Size theLength,
                     const TCollection_AsciiString& theFileName) = 0;

  //! Write image data to file using file extension to determine compression format.
  virtual bool Save (const TCollection_AsciiString& theFileName) = 0;

  //! Performs gamma correction on image.
  //! theGamma - gamma value to use; a value of 1.0 leaves the image alone
  virtual bool AdjustGamma (Standard_Real theGammaCorr) { (void )theGammaCorr; return false; }

  //! Initializes image data as copy of another image.
  Standard_EXPORT virtual bool InitCopy (const Image_PixMap& theCopy) Standard_OVERRIDE;

public:

  //! Create default instance of this class.
  virtual Handle(Image_AlienPixMapI) createDefault() const = 0;

protected:

  //! Fallback copying stream data into transient buffer
  Standard_EXPORT bool loadStreamToBuffer (std::istream& theStream,
                                           const TCollection_AsciiString& theFileName);

  //! Built-in PPM export
  Standard_EXPORT bool savePPM (const TCollection_AsciiString& theFileName) const;

  //! Wrapper initialization for sub-classes.
  bool initWrapper (Image_Format thePixelFormat,
                    Standard_Byte* theDataPtr,
                    Standard_Size theSizeX,
                    Standard_Size theSizeY,
                    Standard_Size theSizeRowBytes)
  {
    return Image_PixMap::InitWrapper (thePixelFormat, theDataPtr, theSizeX, theSizeY, theSizeRowBytes);
  }

private:

  //! Wrapper initialization by user is disallowed for this class (will return false in any case)!
  //! Use only copying and allocation initializers.
  Standard_EXPORT virtual bool InitWrapper (Image_Format        thePixelFormat,
                                            Standard_Byte*      theDataPtr,
                                            const Standard_Size theSizeX,
                                            const Standard_Size theSizeY,
                                            const Standard_Size theSizeRowBytes) Standard_OVERRIDE;

};

DEFINE_STANDARD_HANDLE(Image_AlienPixMapI, Image_PixMap)

#endif // _Image_AlienPixMapI_HeaderFile

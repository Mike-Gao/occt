// Created on: 2010-09-16
// Created by: KGV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#include <Image_AlienPixMap.hxx>

#include <Image_FreeImage.hxx>
#include <Image_WinCodec.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Image_AlienPixMap,Image_PixMap)

namespace
{
  //! Global instance of image factory.
  static Handle(Image_AlienPixMapI)& getFactory()
  {
    static Handle(Image_AlienPixMapI) THE_IMAGE_FACTORY =
    #if defined(HAVE_FREEIMAGE)
      new Image_FreeImage();
    #elif defined(HAVE_WINCODEC)
      new Image_WinCodec();
    #else
      Handle(Image_AlienPixMapI)();
    #endif
    return THE_IMAGE_FACTORY;
  }
}

// =======================================================================
// function : CreateDefault
// purpose  :
// =======================================================================
Handle(Image_AlienPixMapI) Image_AlienPixMap::CreateDefault()
{
  if (!getFactory().IsNull())
  {
    return getFactory()->createDefault();
  }
  return Handle(Image_AlienPixMapI)();
}

// =======================================================================
// function : SetDefaultFactory
// purpose  :
// =======================================================================
void Image_AlienPixMap::SetDefaultFactory (const Handle(Image_AlienPixMapI)& theLibrary)
{
  getFactory() = theLibrary;
}

// =======================================================================
// function : Image_AlienPixMap
// purpose  :
// =======================================================================
Image_AlienPixMap::Image_AlienPixMap()
{
  myLibImage = CreateDefault();
}

// =======================================================================
// function : ~Image_AlienPixMap
// purpose  :
// =======================================================================
Image_AlienPixMap::~Image_AlienPixMap()
{
  Clear();
}

// =======================================================================
// function : InitTrash
// purpose  :
// =======================================================================
bool Image_AlienPixMap::InitTrash (Image_Format        thePixelFormat,
                                   const Standard_Size theSizeX,
                                   const Standard_Size theSizeY,
                                   const Standard_Size theSizeRowBytes)
{
  Clear();
  if (myLibImage.IsNull())
  {
    return base_type::InitTrash (thePixelFormat, theSizeX, theSizeY, theSizeRowBytes);
  }
  if (myLibImage->InitTrash (thePixelFormat, theSizeX, theSizeY, theSizeRowBytes))
  {
    Handle(Image_AlienPixMapI) aLibImage = myLibImage;
    myLibImage.Nullify(); // prevent clearing
    initWrapper (aLibImage->Format(), aLibImage->ChangeData(),
                 aLibImage->SizeX(), aLibImage->SizeY(), aLibImage->SizeRowBytes());
    myLibImage = aLibImage;
    base_type::SetTopDown (myLibImage->IsTopDown());
    return true;
  }
  return false;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Image_AlienPixMap::Clear()
{
  Image_PixMap::Clear();
  if (!myLibImage.IsNull())
  {
    myLibImage->Clear();
  }
}

// =======================================================================
// function : IsTopDownDefault
// purpose  :
// =======================================================================
bool Image_AlienPixMap::IsTopDownDefault()
{
  return getFactory().IsNull()
      || getFactory()->IsTopDown();
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
bool Image_AlienPixMap::Load (const Standard_Byte* theData,
                              Standard_Size theLength,
                              const TCollection_AsciiString& theImagePath)
{
  Clear();
  if (!myLibImage.IsNull()
    && myLibImage->Load (theData, theLength, theImagePath))
  {
    Handle(Image_AlienPixMapI) aLibImage = myLibImage;
    myLibImage.Nullify(); // prevent clearing
    initWrapper (aLibImage->Format(), aLibImage->ChangeData(),
                 aLibImage->SizeX(), aLibImage->SizeY(), aLibImage->SizeRowBytes());
    myLibImage = aLibImage;
    base_type::SetTopDown (myLibImage->IsTopDown());
    return true;
  }
  return false;
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
bool Image_AlienPixMap::Load (std::istream& theStream,
                              const TCollection_AsciiString& theFileName)
{
  Clear();
  if (!myLibImage.IsNull()
    && myLibImage->Load (theStream, theFileName))
  {
    Handle(Image_AlienPixMapI) aLibImage = myLibImage;
    myLibImage.Nullify(); // prevent clearing
    initWrapper (aLibImage->Format(), aLibImage->ChangeData(),
                 aLibImage->SizeX(), aLibImage->SizeY(), aLibImage->SizeRowBytes());
    myLibImage = aLibImage;
    base_type::SetTopDown (myLibImage->IsTopDown());
    return true;
  }
  return false;
}

// =======================================================================
// function : Save
// purpose  :
// =======================================================================
bool Image_AlienPixMap::Save (const TCollection_AsciiString& theFileName)
{
  TCollection_AsciiString aFileNameLower = theFileName;
  aFileNameLower.LowerCase();
  if (aFileNameLower.EndsWith (".ppm")
   || myLibImage.IsNull())
  {
    if (!aFileNameLower.EndsWith (".ppm"))
    {
      Message::DefaultMessenger()->Send ("Image_AlienPixMap, no image library available! Image saved in PPM format.", Message_Warning);
    }
    return savePPM (theFileName);
  }

  return myLibImage->Save (theFileName);
}

// =======================================================================
// function : AdjustGamma
// purpose  :
// =======================================================================
bool Image_AlienPixMap::AdjustGamma (Standard_Real theGammaCorr)
{
  return !myLibImage.IsNull()
       && myLibImage->AdjustGamma (theGammaCorr);
}

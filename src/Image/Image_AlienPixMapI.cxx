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

#include <Image_AlienPixMapI.hxx>
#include <gp.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_Array1.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_OpenFile.hxx>
#include <fstream>
#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(Image_AlienPixMapI, Image_PixMap)

// =======================================================================
// function : InitWrapper
// purpose  :
// =======================================================================
bool Image_AlienPixMapI::InitWrapper (Image_Format,
                                      Standard_Byte*,
                                      const Standard_Size,
                                      const Standard_Size,
                                      const Standard_Size)
{
  Clear();
  return false;
}

// =======================================================================
// function : InitCopy
// purpose  :
// =======================================================================
bool Image_AlienPixMapI::InitCopy (const Image_PixMap& theCopy)
{
  if (&theCopy == this)
  {
    // self-copying disallowed
    return false;
  }
  if (!InitTrash (theCopy.Format(), theCopy.SizeX(), theCopy.SizeY(), theCopy.SizeRowBytes()))
  {
    return false;
  }

  if (myImgFormat == theCopy.Format())
  {
    if (SizeRowBytes() == theCopy.SizeRowBytes()
     && TopDownInc()   == theCopy.TopDownInc())
    {
      // copy with one call
      memcpy (ChangeData(), theCopy.Data(), std::min (SizeBytes(), theCopy.SizeBytes()));
      return true;
    }

    // copy row-by-row
    const Standard_Size aRowSizeBytes = std::min (SizeRowBytes(), theCopy.SizeRowBytes());
    for (Standard_Size aRow = 0; aRow < myData.SizeY; ++aRow)
    {
      memcpy (ChangeRow (aRow), theCopy.Row (aRow), aRowSizeBytes);
    }
    return true;
  }

  // pixel format conversion required
  Clear();
  return false;
}

// =======================================================================
// function : loadStreamToBuffer
// purpose  :
// =======================================================================
bool Image_AlienPixMapI::loadStreamToBuffer (std::istream& theStream,
                                             const TCollection_AsciiString& theFileName)
{
  Clear();

  const std::streamoff aStart = theStream.tellg();
  theStream.seekg (0, std::ios::end);
  const Standard_Integer aLen = Standard_Integer(theStream.tellg() - aStart);
  theStream.seekg (aStart);
  if (aLen <= 0)
  {
    Message::DefaultMessenger()->Send ("Error: empty stream", Message_Fail);
    return false;
  }

  NCollection_Array1<Standard_Byte> aBuff (1, aLen);
  if (!theStream.read ((char* )&aBuff.ChangeFirst(), aBuff.Size()))
  {
    Message::DefaultMessenger()->Send ("Error: unable to read stream", Message_Fail);
    return false;
  }

  return Load (&aBuff.ChangeFirst(), aBuff.Size(), theFileName);
}

// =======================================================================
// function : savePPM
// purpose  :
// =======================================================================
bool Image_AlienPixMapI::savePPM (const TCollection_AsciiString& theFileName) const
{
  if (IsEmpty())
  {
    return false;
  }

  // Open file
  FILE* aFile = OSD_OpenFile (theFileName.ToCString(), "wb");
  if (aFile == NULL)
  {
    return false;
  }

  // Write header
  fprintf (aFile, "P6\n%d %d\n255\n", (int )SizeX(), (int )SizeY());
  fprintf (aFile, "# Image stored by OpenCASCADE framework in linear RGB colorspace\n");

  // Write pixel data
  Standard_Byte aByte;
  for (Standard_Size aRow = 0; aRow < SizeY(); ++aRow)
  {
    for (Standard_Size aCol = 0; aCol < SizeX(); ++aCol)
    {
      // extremely SLOW but universal (implemented for all supported pixel formats)
      const Quantity_ColorRGBA aColor = PixelColor ((Standard_Integer )aCol, (Standard_Integer )aRow);
      aByte = Standard_Byte(aColor.GetRGB().Red()   * 255.0); fwrite (&aByte, 1, 1, aFile);
      aByte = Standard_Byte(aColor.GetRGB().Green() * 255.0); fwrite (&aByte, 1, 1, aFile);
      aByte = Standard_Byte(aColor.GetRGB().Blue()  * 255.0); fwrite (&aByte, 1, 1, aFile);
    }
  }

  // Close file
  fclose (aFile);
  return true;
}

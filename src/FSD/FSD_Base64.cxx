// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

// This code is taken from the snippet provided by the user polfosol
// in the discussion https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

#include <FSD_Base64.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>

// =======================================================================
// function : Encode
// =======================================================================
TCollection_AsciiString  FSD_Base64::Encode (const Standard_Byte* theData,
                                             const Standard_Size  theLen)
{
  static const char* B64chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  Standard_Size aPad = theLen % 3;
  const Standard_Size aSize64 = 4 * (int(aPad > 0) + theLen / 3);
  TCollection_AsciiString aStr64 (static_cast<Standard_Integer> (aSize64), '=');
  char* pStr64 = const_cast<char*> (aStr64.ToCString());

  for (Standard_Size i = 0; i < theLen - aPad; i += 3)
  {
    int aWord = int(theData[i]) << 16 | int(theData[i + 1]) << 8 | theData[i + 2];
    *pStr64++ = static_cast<char> (B64chars[aWord >> 18]);
    *pStr64++ = static_cast<char> (B64chars[aWord >> 12 & 0x3F]);
    *pStr64++ = static_cast<char> (B64chars[aWord >> 6 & 0x3F]);
    *pStr64++ = static_cast<char> (B64chars[aWord & 0x3F]);
  }
  if (aPad--)    /// padding
  {
    int aWord = aPad ? int(theData[theLen - 2]) << 8 | theData[theLen - 1] : theData[theLen - 1];
    *pStr64++ = static_cast<char> (aPad ? B64chars[aWord >> 10] : B64chars[aWord >> 2]);
    *pStr64++ = static_cast<char> (aPad ? B64chars[aWord >> 4 & 0x03F] : B64chars[(aWord & 3) << 4]);
    *pStr64++ = static_cast<char> (aPad ? B64chars[(aWord & 0xF) << 2] : '=');
  }
  return aStr64;
}

// =======================================================================
// function : Decode
// =======================================================================
Handle(NCollection_Buffer) FSD_Base64::Decode (const Standard_Byte* theStr,
                                               const Standard_Size  theLen)
{
  static const Standard_Byte B64index[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
    7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
    0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

  // Calculate pad bytes and number of bytes without pad
  Standard_Integer aPad = theLen > 0 && (theLen % 4 || theStr[theLen - 1] == '=');
  const Standard_Size aNbIter = ((theLen + 3) / 4 - aPad) * 4;
  if (theLen > aNbIter + 2 && theStr[aNbIter + 2] != '=')
  {
    aPad++;
  }

  // Calculate new size and allocate the buffer
  const Standard_Size aNewSize = aNbIter / 4 * 3 + aPad;
  Handle(NCollection_Buffer) aData =
    new NCollection_Buffer (NCollection_BaseAllocator::CommonBaseAllocator());
  if (!aData->Allocate (aNewSize))
  {
    Message::DefaultMessenger()->Send ("Fail to allocate memory.", Message_Fail);
    return Handle(NCollection_Buffer)();
  }

  // Decoding loop
  Standard_Byte* aDataPtr = aData->ChangeData();
  for (Standard_Size i = 0; i < aNbIter; i += 4)
  {
    Standard_Integer aWord = B64index[theStr[i]] << 18 | B64index[theStr[i + 1]] << 12 |
                             B64index[theStr[i + 2]] << 6 | B64index[theStr[i + 3]];
    *aDataPtr++ = static_cast<Standard_Byte> (aWord >> 16);
    *aDataPtr++ = static_cast<Standard_Byte> (aWord >> 8 & 0xFF);
    *aDataPtr++ = static_cast<Standard_Byte> (aWord & 0xFF);
  }

  // Decoding pad bytes
  if (aPad > 0)
  {
    Standard_Integer aWord = B64index[theStr[aNbIter]] << 18 | B64index[theStr[aNbIter + 1]] << 12;
    *aDataPtr++ = static_cast<Standard_Byte> (aWord >> 16);

    if (aPad > 1)
    {
      aWord |= B64index[theStr[aNbIter + 2]] << 6;
      *aDataPtr++ = static_cast<Standard_Byte> (aWord >> 8 & 0xFF);
    }
  }
  return aData;
}

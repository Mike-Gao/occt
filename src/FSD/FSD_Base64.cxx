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
Standard_Size FSD_Base64::Encode (char* theEncodedStr,
                                  const Standard_Byte* theData,
                                  const Standard_Size theDataLen)
{
  static const char* B64chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  if (theDataLen == 0)
  {
    return 0;
  }

  Standard_Size aPad = theDataLen % 3;
  const Standard_Size aSize64 = 4 * (int(aPad > 0) + theDataLen / 3);
  if (!theEncodedStr)
  {
    return aSize64;
  }

  Standard_Size iStr = 0;
  for (Standard_Size i = 0; i < theDataLen - aPad; i += 3)
  {
    int aWord = int(theData[i]) << 16 | int(theData[i + 1]) << 8 | theData[i + 2];
    theEncodedStr[iStr++] = static_cast<char> (B64chars[aWord >> 18]);
    theEncodedStr[iStr++] = static_cast<char> (B64chars[aWord >> 12 & 0x3F]);
    theEncodedStr[iStr++] = static_cast<char> (B64chars[aWord >> 6 & 0x3F]);
    theEncodedStr[iStr++] = static_cast<char> (B64chars[aWord & 0x3F]);
  }
  if (aPad--)    /// padding
  {
    int aWord = aPad ? int(theData[theDataLen - 2]) << 8 | theData[theDataLen - 1]
                     : theData[theDataLen - 1];
    theEncodedStr[iStr++] = static_cast<char> (aPad ? B64chars[aWord >> 10] : B64chars[aWord >> 2]);
    theEncodedStr[iStr++] = static_cast<char> (aPad ? B64chars[aWord >> 4 & 0x03F] : B64chars[(aWord & 3) << 4]);
    theEncodedStr[iStr++] = static_cast<char> (aPad ? B64chars[(aWord & 0xF) << 2] : '=');
  }
  while (iStr < aSize64)
  {
    theEncodedStr[iStr++] = '=';
  }
  return aSize64;
}

// =======================================================================
// function : Decode
// =======================================================================
Standard_Size FSD_Base64::Decode (Standard_Byte* theDecodedData,
                                  Standard_CString theEncodedStr,
                                  const Standard_Size theStrLen)
{
  static const Standard_Byte B64index[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
    7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
    0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

  if (theStrLen == 0)
  {
    return 0;
  }

  // Calculate pad bytes and number of bytes without pad
  Standard_Integer aPad = theStrLen > 0 && (theStrLen % 4 || theEncodedStr[theStrLen - 1] == '=');
  const Standard_Size aNbIter = ((theStrLen + 3) / 4 - aPad) * 4;
  if (theStrLen > aNbIter + 2 && theEncodedStr[aNbIter + 2] != '=')
  {
    aPad++;
  }

  // Calculate new size and allocate the buffer
  const Standard_Size aDecodedSize = aNbIter / 4 * 3 + aPad;
  if (!theDecodedData)
  {
    return aDecodedSize;
  }

  // Decoding loop
  for (Standard_Size i = 0; i < aNbIter; i += 4)
  {
    Standard_Integer aWord = B64index[Standard_Size (theEncodedStr[i])] << 18 |
                             B64index[Standard_Size (theEncodedStr[i + 1])] << 12 |
                             B64index[Standard_Size (theEncodedStr[i + 2])] << 6 |
                             B64index[Standard_Size (theEncodedStr[i + 3])];
    *theDecodedData++ = static_cast<Standard_Byte> (aWord >> 16);
    *theDecodedData++ = static_cast<Standard_Byte> (aWord >> 8 & 0xFF);
    *theDecodedData++ = static_cast<Standard_Byte> (aWord & 0xFF);
  }

  // Decoding pad bytes
  if (aPad > 0)
  {
    Standard_Integer aWord = B64index[Standard_Size (theEncodedStr[aNbIter])] << 18 |
                             B64index[Standard_Size (theEncodedStr[aNbIter + 1])] << 12;
    *theDecodedData++ = static_cast<Standard_Byte> (aWord >> 16);

    if (aPad > 1)
    {
      aWord |= B64index[Standard_Size (theEncodedStr[aNbIter + 2])] << 6;
      *theDecodedData++ = static_cast<Standard_Byte> (aWord >> 8 & 0xFF);
    }
  }
  return aDecodedSize;
}

// =======================================================================
// function : Decode
// =======================================================================
Handle(NCollection_Buffer) FSD_Base64::Decode (Standard_CString theEncodedStr,
                                               const Standard_Size theStrLen)
{
  Standard_Size aDataSize = Decode (NULL, theEncodedStr, theStrLen);
  Handle(NCollection_Buffer) aBuf =
    new NCollection_Buffer (NCollection_BaseAllocator::CommonBaseAllocator());
  if (!aBuf->Allocate (aDataSize))
  {
    Message::DefaultMessenger()->Send ("Fail to allocate memory.", Message_Fail);
    return NULL;
  }
  Decode (aBuf->ChangeData(), theEncodedStr, theStrLen);
  return aBuf;
}

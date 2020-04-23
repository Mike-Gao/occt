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

#ifndef _FSD_Base64_HeaderFile
#define _FSD_Base64_HeaderFile

#include <NCollection_Buffer.hxx>

//! Tool for encoding/decoding base64 stream.
class FSD_Base64
{
public:

  //! Function encoding a buffer to base64.
  //! @param theEncodedStr [out] the place for encoded string. Terminating null is not put.
  //!        If it is NULL just return the needed size.
  //! @param theData [in] the input binary data.
  //! @param theDataLen [in] the length of input data in bytes.
  //! @return the length of the encoded string not including terminating null.
  Standard_EXPORT static Standard_Size Encode (char* theEncodedStr,
                                               const Standard_Byte* theData,
                                               const Standard_Size theDataLen);

  //! Function decoding base64 string.
  //! @param theDecodedData [out] the place for decoded data.
  //!        If it is NULL just return the needed size.
  //! @param theEncodedStr [in] the input encoded string.
  //! @param theStrLen [in] the length of input encoded string.
  //! @return the length of the decoded data in bytes.
  Standard_EXPORT static Standard_Size Decode (Standard_Byte* theDecodedData,
                                               Standard_CString theEncodedStr,
                                               const Standard_Size theStrLen);

  //! Function decoding base64 string.
  //! @param theEncodedStr [in] the input encoded string.
  //! @param theStrLen [in] the length of input encoded string.
  //! @return null handle if theLen is 0 or in case of out of memory condition.
  Standard_EXPORT static Handle(NCollection_Buffer) Decode (Standard_CString theStr,
                                                            const Standard_Size theLen);
};

#endif // _FSD_Base64_HeaderFile

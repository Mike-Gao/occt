// Created on: 2000-09-28
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _STEPCAFControl_ExternFile_HeaderFile
#define _STEPCAFControl_ExternFile_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_ReturnStatus.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_Label.hxx>
#include <MMgt_TShared.hxx>
class XSControl_WorkSession;
class TCollection_HAsciiString;
class TDF_Label;


class STEPCAFControl_ExternFile;
DEFINE_STANDARD_HANDLE(STEPCAFControl_ExternFile, MMgt_TShared)

//! Auxiliary class serving as container for data resulting
//! from translation of external file
class STEPCAFControl_ExternFile : public MMgt_TShared
{

public:

  
  //! Creates an empty structure
  Standard_EXPORT STEPCAFControl_ExternFile();
  
    void SetWS (const Handle(XSControl_WorkSession)& WS);
  
    Handle(XSControl_WorkSession) GetWS() const;
  
    void SetLoadStatus (const Interface_ReturnStatus stat);
  
    Interface_ReturnStatus GetLoadStatus() const;
  
    void SetTransferStatus (const Standard_Boolean isok);
  
    Standard_Boolean GetTransferStatus() const;
  
    void SetWriteStatus (const Interface_ReturnStatus stat);
  
    Interface_ReturnStatus GetWriteStatus() const;
  
    void SetName (const Handle(TCollection_HAsciiString)& name);
  
    Handle(TCollection_HAsciiString) GetName() const;
  
    void SetLabel (const TDF_Label& L);
  
    TDF_Label GetLabel() const;




  DEFINE_STANDARD_RTTIEXT(STEPCAFControl_ExternFile,MMgt_TShared)

protected:




private:


  Handle(XSControl_WorkSession) myWS;
  Interface_ReturnStatus myLoadStatus;
  Standard_Boolean myTransferStatus;
  Interface_ReturnStatus myWriteStatus;
  Handle(TCollection_HAsciiString) myName;
  TDF_Label myLabel;


};


#include <STEPCAFControl_ExternFile.lxx>





#endif // _STEPCAFControl_ExternFile_HeaderFile

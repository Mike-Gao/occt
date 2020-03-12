// Created on: 2020-03-18
// Created by: Irina KRYLOVA
// Copyright (c) 2020 OPEN CASCADE SAS
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


#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_KinematicTool.hxx>
#include <XmlMXCAFDoc_KinematicToolDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_KinematicToolDriver, XmlMDF_ADriver)

//=======================================================================
//function : XmlMXCAFDoc_KinematicToolDriver
//purpose  : 
//=======================================================================
XmlMXCAFDoc_KinematicToolDriver::XmlMXCAFDoc_KinematicToolDriver
  (const Handle(Message_Messenger)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, "xcaf", "KinematicTool")
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_KinematicToolDriver::NewEmpty() const
{
  return new XCAFDoc_KinematicTool();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_KinematicToolDriver::Paste(const XmlObjMgt_Persistent&,
                                                        const Handle(TDF_Attribute)&,
                                                        XmlObjMgt_RRelocationTable&) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_KinematicToolDriver::Paste(const Handle(TDF_Attribute)&,
                                            XmlObjMgt_Persistent&,
                                            XmlObjMgt_SRelocationTable&) const
{
}

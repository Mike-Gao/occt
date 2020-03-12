// Created on: 2020-03-30
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


#include <BinMXCAFDoc_KinematicPairValueDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_KinematicPairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_KinematicPairValueDriver, BinMDF_ADriver)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BinMXCAFDoc_KinematicPairValueDriver::
BinMXCAFDoc_KinematicPairValueDriver (const Handle(Message_Messenger)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_KinematicPairValue)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_KinematicPairValueDriver::NewEmpty() const
{
  return new XCAFDoc_KinematicPairValue();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_KinematicPairValueDriver::Paste (const BinObjMgt_Persistent&  /*theSource*/,
                                                              const Handle(TDF_Attribute)& /*theTarget*/,
                                                              BinObjMgt_RRelocationTable&  /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMXCAFDoc_KinematicPairValueDriver::Paste (const Handle(TDF_Attribute)&  /*theSource*/,
                                                  BinObjMgt_Persistent&         /*theTarget*/,
                                                  BinObjMgt_SRelocationTable&   /*theRelocTable*/) const
{
}

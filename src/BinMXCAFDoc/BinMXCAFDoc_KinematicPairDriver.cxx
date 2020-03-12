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


#include <BinMXCAFDoc_KinematicPairDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_KinematicPair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_KinematicPairDriver, BinMDF_ADriver)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BinMXCAFDoc_KinematicPairDriver::BinMXCAFDoc_KinematicPairDriver (const Handle(Message_Messenger)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_KinematicPair)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_KinematicPairDriver::NewEmpty() const
{
  return new XCAFDoc_KinematicPair();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_KinematicPairDriver::Paste (const BinObjMgt_Persistent&  /*theSource*/,
                                                         const Handle(TDF_Attribute)& /*theTarget*/,
                                                         BinObjMgt_RRelocationTable&  /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMXCAFDoc_KinematicPairDriver::Paste (const Handle(TDF_Attribute)&  /*theSource*/,
                                             BinObjMgt_Persistent&         /*theTarget*/,
                                             BinObjMgt_SRelocationTable&   /*theRelocTable*/) const
{
}

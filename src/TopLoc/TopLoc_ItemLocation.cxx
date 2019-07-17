// Created on: 1991-01-21
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <TopLoc_Datum3D.hxx>
#include <TCollection.hxx>
#include <TopLoc_ItemLocation.hxx>
#include <TopLoc_Location.hxx>
#include <TopLoc_SListOfItemLocation.hxx>

//=======================================================================
//function : TopLoc_ItemLocation
//purpose  : constructor
//=======================================================================
TopLoc_ItemLocation::TopLoc_ItemLocation 
  (const Handle(TopLoc_Datum3D)& D, 
   const Standard_Integer P) :
  myDatum(D),
  myPower(P),
  myTrsf (D->Transformation().Powered (P))
{
}

const TCollection_AsciiString TopLoc_ItemLocation_ClassName = "TopLoc_ItemLocation";

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void TopLoc_ItemLocation::Dump (Standard_OStream& OS, const Standard_Integer theMask) const
{
  DUMP_START_KEY (OS, TopLoc_ItemLocation_ClassName);

  DUMP_VALUES (OS, "Power", myPower);
  {
    Standard_SStream aTmpStream;
    myTrsf.Dump (aTmpStream);
    DUMP_VALUES (OS, "Trsf", TCollection::ToDumpString (aTmpStream));
  }
  {
    Standard_SStream aTmpStream;
    if (!myDatum.IsNull())
      myDatum->Dump (aTmpStream);
    DUMP_VALUES (OS, "Datum", TCollection::ToDumpString (aTmpStream));
  }

  DUMP_STOP_KEY (OS, TopLoc_ItemLocation_ClassName);
}

// Copyright (c) 1995-1999 Matra Datavision
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


#include <gp_Mat.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_List.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection.hxx>
#include <TCollection_AsciiString.hxx>

#include <TCollection_AsciiString.hxx>

Standard_Boolean gp_XYZ::IsEqual (const gp_XYZ& Other,
				  const Standard_Real Tolerance) const {
  Standard_Real val;
  val = x - Other.x;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  val = y - Other.y;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  val = z - Other.z;
  if (val < 0) val = - val;
  if (val > Tolerance) return Standard_False;
  return Standard_True;
}

const TCollection_AsciiString gp_XYZ_ClassName = "gp_XYZ";

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void gp_XYZ::Dump (Standard_OStream& OS, const Standard_Integer theMask) const
{
  DUMP_START_KEY (OS, gp_XYZ_ClassName);

  DUMP_VALUES (OS, "X", x);
  DUMP_VALUES (OS, "Y", y);
  DUMP_VALUES (OS, "Z", z);

  DUMP_STOP_KEY (OS, gp_XYZ_ClassName);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean gp_XYZ::Init (const Standard_OStream& OS)
{
  /*NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aStreamValues;
  Standard_SStream aSStream (OS);
  TCollection::Split (aSStream, aStreamValues);

  TCollection_AsciiString anXYZValue;
  if (aStreamValues.FindFromKey (gp_XYZ_ClassName, anXYZValue))
    anXYZValue = anXYZValue;
  else if (aStreamValues.Size() == 1)
  {
    TCollection_AsciiString aValueStr = aStreamValues.FindFromIndex (1);
    Standard_Integer aPosition = aValueStr.Search (gp_XYZ_ClassName + TCollection::ClassNameSeparator());
    if (aPosition < 1)
      return Standard_False;
    anXYZValue = aValueStr.Split (aPosition);
  }

  NCollection_Vector<Standard_Real> aValues;
  if (!TCollection::SplitReal (anXYZValue, TCollection::VectorSeparator(), aValues))
    return Standard_False;

  if (aValues.Size() != 3)
    return Standard_False;

  x = aValues.Value (1);
  y = aValues.Value (2);
  z = aValues.Value (3);
  */
  return Standard_True;
}

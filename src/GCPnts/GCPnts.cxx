// Created on: 2020-05-18
// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#include <GCPnts.hxx>

#include <Precision.hxx>
//=======================================================================
//function : FillParams
//purpose  : 
//=======================================================================
void GCPnts::FillParams (const TColStd_Array1OfReal& theKnots,
                         const Standard_Integer theDegree,
                         const Standard_Real theParMin,
                         const Standard_Real theParMax,
                         NCollection_Vector<Standard_Real>& theParams)
{
  Standard_Real aPrevPar = theParMin;
  theParams.Append (aPrevPar);

  Standard_Integer aNbP = Max (theDegree, 1);

  for (Standard_Integer i = 1;
    (i < theKnots.Length()) && (theKnots (i) < (theParMax - Precision::PConfusion())); ++i)
  {
    if (theKnots (i + 1) < theParMin + Precision::PConfusion())
      continue;

    Standard_Real aStep = (theKnots (i + 1) - theKnots (i)) / aNbP;
    for (Standard_Integer k = 1; k <= aNbP ; ++k)
    {
      Standard_Real aPar = theKnots (i) + k * aStep;
      if (aPar > theParMax - Precision::PConfusion())
        break;

      if (aPar > aPrevPar + Precision::PConfusion())
      {
        theParams.Append (aPar);
        aPrevPar = aPar;
      }
    }
  }
  theParams.Append (theParMax);
}

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

#ifndef _GCPnts_HeaderFile
#define _GCPnts_HeaderFile

#include <TColStd_Array1OfReal.hxx>
#include <NCollection_Vector.hxx>

//! The GCPnts package provides general utilities for
//! Curves analysis.
class GCPnts
{
public:

  //! Fills <theParams> vector with sampling parameters on the curve
  Standard_EXPORT static void FillParams (const TColStd_Array1OfReal& theKnots,
                                          const Standard_Integer theDegree,
                                          const Standard_Real theParMin,
                                          const Standard_Real theParMax,
                                          NCollection_Vector<Standard_Real>& theParams);
};

#endif

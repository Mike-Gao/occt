// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _IMeshTools_Parameters_HeaderFile
#define _IMeshTools_Parameters_HeaderFile

#include <Precision.hxx>

//! Structure storing meshing parameters
struct IMeshTools_Parameters {

  //! Default constructor
  IMeshTools_Parameters ()
    :
    Angle (0.5),
    Deflection (0.001),
    MinSize (Precision::Confusion ()),
    InParallel (Standard_False),
    Relative (Standard_False),
    InternalVerticesMode (Standard_True),
    ControlSurfaceDeflection (Standard_True),
    CleanModel(Standard_True)
  {
  }

  //! Angular deflection
  Standard_Real                                    Angle;

  //! Deflection
  Standard_Real                                    Deflection;

  //! Minimal allowed size of mesh element
  Standard_Real                                    MinSize;

  //! Switches on/off multi-thread computation
  Standard_Boolean                                 InParallel;

  //! Switches on/off relative computation of edge tolerance<br>
  //! If true, deflection used for the polygonalisation of each edge will be 
  //! <defle> * Size of Edge. The deflection used for the faces will be the 
  //! maximum deflection of their edges.
  Standard_Boolean                                 Relative;

  //! Mode to take or not to take internal face vertices into account
  //! in triangulation process
  Standard_Boolean                                 InternalVerticesMode;

  //! Parameter to check the deviation of triangulation and interior of
  //! the face
  Standard_Boolean                                 ControlSurfaceDeflection;

  //! Cleans temporary data model when algorithm is finished.
  Standard_Boolean                                 CleanModel;
};

#endif

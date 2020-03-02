// Created on: 2016-07-22
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

#ifndef _BRepMesh_ModelPostProcessor_HeaderFile
#define _BRepMesh_ModelPostProcessor_HeaderFile

#include <IMeshTools_ModelAlgo.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshData_Types.hxx>

//! Class implements functionality of model post-processing tool.
//! Stores polygons on triangulations to TopoDS_Edge.
class BRepMesh_ModelPostProcessor : public IMeshTools_ModelAlgo
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_ModelPostProcessor();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ModelPostProcessor();

  //! Dumps the content of me into the stream
  virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE
  {
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
    OCCT_DUMP_BASE_CLASS (theOStream, theDepth, IMeshTools_ModelAlgo)
  }

protected:

  //! Performs processing of edges of the given model.
  Standard_EXPORT virtual Standard_Boolean performInternal (
    const Handle(IMeshData_Model)& theModel,
    const IMeshTools_Parameters&   theParameters) Standard_OVERRIDE;
};

#endif

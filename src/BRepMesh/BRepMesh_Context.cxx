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

#include <BRepMesh_Context.hxx>
#include <BRepMesh_ModelBuilder.hxx>
#include <BRepMesh_EdgeDiscret.hxx>
#include <BRepMesh_ModelHealer.hxx>
#include <BRepMesh_FaceDiscret.hxx>
#include <BRepMesh_ModelPreProcessor.hxx>
#include <BRepMesh_ModelPostProcessor.hxx>

#include <BRepMesh_MeshAlgoFactory.hxx>
#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
BRepMesh_Context::BRepMesh_Context ()
{
  enum MeshAlgo
  {
    MeshAlgo_Default   = 0x0,
    MeshAlgo_Delabella = 0x1
  };

  char* anAlgoVar;
  anAlgoVar = getenv ("CSF_MeshAlgo");
  const Standard_Integer anAlgoId = (anAlgoVar ? atoi (anAlgoVar) : MeshAlgo_Default);

  Handle (IMeshTools_MeshAlgoFactory) aAlgoFactory;
  switch (anAlgoId)
  {
  case MeshAlgo_Delabella:
    aAlgoFactory = new BRepMesh_DelabellaMeshAlgoFactory;
    break;

  default:
    aAlgoFactory = new BRepMesh_MeshAlgoFactory;
    break;
  }

  SetModelBuilder (new BRepMesh_ModelBuilder);
  SetEdgeDiscret  (new BRepMesh_EdgeDiscret);
  SetModelHealer  (new BRepMesh_ModelHealer);
  SetPreProcessor (new BRepMesh_ModelPreProcessor);
  SetFaceDiscret  (new BRepMesh_FaceDiscret (aAlgoFactory));
  SetPostProcessor(new BRepMesh_ModelPostProcessor);
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
BRepMesh_Context::~BRepMesh_Context ()
{
}

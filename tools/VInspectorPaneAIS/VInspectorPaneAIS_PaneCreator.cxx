// Created on: 2018-12-08
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/VInspectorPaneAIS_PaneCreator.hxx>
#include <inspector/VInspectorPaneAIS_Shape.hxx>
#include <inspector/VInspectorPaneAIS_Trihedron.hxx>

#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VInspectorPaneAIS_PaneCreator, VInspectorAPI_CallBack)

// =======================================================================
// function : createPane
// purpose :
// =======================================================================
ViewControl_Pane* VInspectorPaneAIS_PaneCreator::createPane (const Standard_CString& theName)
{
  if (theName == STANDARD_TYPE (AIS_Shape)->Name())
    return new VInspectorPaneAIS_Shape();
  //else if (theName == STANDARD_TYPE (AIS_Trihedron)->Name())
  //  return new VInspectorPaneAIS_Trihedron();

  return NULL;
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
ViewControl_Pane* VInspectorPaneAIS_PaneCreator::GetPane (const Standard_CString& theName)
{
  if (myPanes.IsBound (theName))
    return myPanes.Find (theName);

  ViewControl_Pane* aPane = createPane (theName);
  if (aPane)
    myPanes.Bind (theName, aPane);

  return aPane;
}

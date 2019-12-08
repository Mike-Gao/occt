// Created on: 2017-06-16
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

#include <inspector/View_Viewer.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_Version.hxx>

#define USE_CLIPPLANE

#ifdef USE_CLIPPLANE
#include <Graphic3d_ClipPlane.hxx>
#include <gp_Pln.hxx>
#endif

// =======================================================================
// function : CreateView
// purpose :
// =======================================================================
void View_Viewer::CreateView()
{
  if (myView.IsNull())
  {
    myView = myContext->CurrentViewer()->CreateView();

#ifdef USE_CLIPPLANE
    gp_Pln aPln (gp_Pnt (50, 0, 0), gp_Dir (-1., 0., 0.));
    Handle(Graphic3d_ClipPlane) aClipPlane = new Graphic3d_ClipPlane(aPln);
    myView->AddClipPlane (aClipPlane);
#endif
  }
}

// =======================================================================
// function : CreateView
// purpose :
// =======================================================================
void View_Viewer::SetWindow(const Handle(Aspect_Window)& theWindow)
{
  myView->SetWindow (theWindow);
  if (!theWindow->IsMapped())
    theWindow->Map();
}

// =======================================================================
// function : InitViewer
// purpose :
// =======================================================================
void View_Viewer::InitViewer (const Handle(AIS_InteractiveContext)& theContext)
{
  myContext = theContext;
  myViewer = myContext->CurrentViewer();
}

// =======================================================================
// function : CreateStandardViewer
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) View_Viewer::CreateStandardViewer()
{
  Handle(Aspect_DisplayConnection) aDisplayConnection = new Aspect_DisplayConnection();
  static Handle(OpenGl_GraphicDriver) aGraphicDriver = new OpenGl_GraphicDriver (aDisplayConnection);

#if OCC_VERSION_HEX > 0x060901
  Handle(V3d_Viewer) aViewer = new V3d_Viewer (aGraphicDriver);
#else
  TCollection_AsciiString a3DName ("Visu3D");
  Handle(V3d_Viewer) aViewer = new V3d_Viewer (aGraphicDriver, a3DName.ToExtString(), "", 1000.0, V3d_XposYnegZpos,
    Quantity_NOC_GRAY30, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT, Standard_True, Standard_False);
#endif

  aViewer->SetDefaultLights();
  aViewer->SetLightOn();
  aViewer->SetDefaultBackgroundColor (Quantity_NOC_GRAY30);

  Handle(AIS_InteractiveContext) aContext = new AIS_InteractiveContext (aViewer);
  aContext->UpdateCurrentViewer();

  return aContext;
}

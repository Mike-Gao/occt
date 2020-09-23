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

#include "ApplicationCommon.h"
#include "DocumentCommon.h"
#include "Material.h"
#include "Transparency.h"

#include <Standard_WarningsDisable.hxx>
#include <QStatusBar>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveObject.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <OpenGl_GraphicDriver.hxx>

#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
#include <OSD_Environment.hxx>
#endif
#include <TCollection_AsciiString.hxx>

// =======================================================================
// function : Viewer
// purpose  :
// =======================================================================
Handle(V3d_Viewer) DocumentCommon::Viewer(const Standard_ExtString,
  const Standard_CString,
  const Standard_Real theViewSize,
  const V3d_TypeOfOrientation theViewProj,
  const Standard_Boolean theComputedMode,
  const Standard_Boolean theDefaultComputedMode)
{
  static Handle(OpenGl_GraphicDriver) aGraphicDriver;

  if (aGraphicDriver.IsNull()) {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    aDisplayConnection = new Aspect_DisplayConnection(OSD_Environment("DISPLAY").Value());
#endif
    aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);
  }

  Handle(V3d_Viewer) aViewer = new V3d_Viewer(aGraphicDriver);
  aViewer->SetDefaultViewSize(theViewSize);
  aViewer->SetDefaultViewProj(theViewProj);
  aViewer->SetComputedMode(theComputedMode);
  aViewer->SetDefaultComputedMode(theDefaultComputedMode);
  return aViewer;
}

DocumentCommon::DocumentCommon(ApplicationCommonWindow* app) :
  QObject(app),
  myContextIsEmpty(true)
{
  TCollection_ExtendedString a3DName("Visu3D");

  myViewer = Viewer(a3DName.ToExtString(), "", 1000.0, V3d_XposYnegZpos, Standard_True, Standard_True);

  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  myContext = new AIS_InteractiveContext(myViewer);
}

DocumentCommon::~DocumentCommon()
{
}


Handle(AIS_InteractiveContext) DocumentCommon::getContext()
{
  return myContext;
}
Handle(V3d_Viewer) DocumentCommon::getViewer()
{
  return myViewer;
}

void DocumentCommon::setViewer(Handle(V3d_Viewer) theViewer)
{
  myViewer = theViewer;
}

void DocumentCommon::SetObjects(const NCollection_Vector<Handle(AIS_InteractiveObject)>& theObjects,
  Standard_Boolean theDisplayShaded)
{
  myContext->RemoveAll(Standard_False);
  if (theObjects.IsEmpty())
    myContextIsEmpty = true;
  else
    myContextIsEmpty = false;
  for (const Handle(AIS_InteractiveObject) anObject : theObjects) {
    if (!theDisplayShaded)
      myContext->Display(anObject, Standard_False);
    else
      myContext->Display(anObject, AIS_Shaded, 0, Standard_False);
  }
  myViewer->Redraw();
}



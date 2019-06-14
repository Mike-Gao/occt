// Created on: 2018-12-11
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <inspector/View_CameraPositionPrs.hxx>

#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>

#include <Prs3d_Root.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

// =======================================================================
// function : StartTransformation
// purpose :
// =======================================================================
void View_CameraPositionPrs::StartTransformation (const int theX, const int theY)
{
  myIsTransformationStarted = Standard_True;
  myStartPosition = gp_Pnt2d (theX, theY);
}

// =======================================================================
// function : StopTransformation
// purpose :
// =======================================================================
void View_CameraPositionPrs::StopTransformation (const int theX, const int theY)
{
  myIsTransformationStarted = Standard_False;
  myStartPosition = gp_Pnt2d (0, 0);

  int aValue = 9;
}

// =======================================================================
// function : Transform
// purpose :
// =======================================================================
void View_CameraPositionPrs::Transform (const int theX, const int theY, const Handle(V3d_View) theView)
{
  // scale camera symmetry according to the center
  if (!myIsTransformationStarted)
    return;

  Standard_Integer aXDiff = theX - myStartPosition.X();
  Standard_Integer anYDiff = theY - myStartPosition.Y();

  Handle(V3d_View) aTargetView = GetView();
  if (aTargetView.IsNull())
    return;
  TColgp_SequenceOfPnt aPoints = getCameraPoints(aTargetView->Camera());
  TColgp_SequenceOfPnt2d aPixelPoints = projectToView (aPoints, theView);
  Standard_Real aWidthInPixels = aPixelPoints.Value(2).Distance (aPixelPoints.Value(1));
  Standard_Real aHeightInPixels = aPixelPoints.Value(3).Distance (aPixelPoints.Value(2));

  if (aWidthInPixels < Precision::Confusion() ||
      aHeightInPixels < Precision::Confusion())
    return;

  Standard_Real aXScaleToMove = aXDiff / aWidthInPixels;
  Standard_Real anYScaleToMove = anYDiff / aHeightInPixels;

  TColgp_SequenceOfPnt2d aTargetPixelPoints = projectToView (aPoints, aTargetView);
  Standard_Real aTargetWidthInPixels = aTargetPixelPoints.Value(2).Distance (aTargetPixelPoints.Value(1));
  Standard_Real aTargetHeightInPixels = aTargetPixelPoints.Value(3).Distance (aTargetPixelPoints.Value(2));
  if (aTargetWidthInPixels < Precision::Confusion() ||
      aTargetHeightInPixels < Precision::Confusion())
    return;

  gp_XYZ aTargetCameraDimensions = aTargetView->Camera()->ViewDimensions();
  Standard_Real aTargetXDiff = aXScaleToMove * aTargetWidthInPixels;
  Standard_Real aTargetYDiff = anYScaleToMove * aTargetHeightInPixels;

  double aRatio = aHeightInPixels / aWidthInPixels;

  bool isXScale = true;
  if (aTargetXDiff == 0 && aTargetYDiff == 0)
    return;
  else if (aTargetXDiff == 0)
    isXScale = false;
  else if (aTargetYDiff == 0)
    isXScale = true;
  else // both X, Y are not zero
  {
    double aDiffRatio = aTargetYDiff / aTargetXDiff;
    isXScale = aDiffRatio > aRatio;
  }

  Standard_Real aNewScale = 1;
  if (isXScale)
  {
    bool isIncrease = true;
    double aScaleSign = isIncrease ? 1.0 : -1.0;

    double aScale = aTargetView->Camera()->Scale();
    Standard_Real aTargetWidthInPixelsNew = aScale + aTargetXDiff * aScaleSign;
    aNewScale = aTargetWidthInPixelsNew;
  }
  else
  {
    std::cout << "Nothing to do" << std::endl;
    myStartPosition = gp_Pnt2d (theX, theY);
    return;
  }

  aTargetView->Camera()->SetScale (aNewScale);
  aTargetView->AutoZFit();
  //aTargetView->ImmediateUpdate();
  //aTargetView->Pan (aTargetXDiff, aTargetYDiff);

  myStartPosition = gp_Pnt2d (theX, theY);

  // move camera center
  //if (!myIsTransformationStarted)
  //  return;

  //Standard_Integer aXDiff = theX - myStartPosition.X();
  //Standard_Integer anYDiff = theY - myStartPosition.Y();
  //aXDiff = -aXDiff;

  //Handle(V3d_View) aTargetView = GetView();
  //if (aTargetView.IsNull())
  //  return;
  //TColgp_SequenceOfPnt aPoints = getCameraPoints(aTargetView->Camera());
  //TColgp_SequenceOfPnt2d aPixelPoints = projectToView (aPoints, theView);
  //Standard_Real aWidthInPixels = aPixelPoints.Value(2).Distance (aPixelPoints.Value(1));
  //Standard_Real aHeightInPixels = aPixelPoints.Value(3).Distance (aPixelPoints.Value(2));

  //if (aWidthInPixels < Precision::Confusion() ||
  //    aHeightInPixels < Precision::Confusion())
  //  return;

  //Standard_Real aXScaleToMove = aXDiff / aWidthInPixels;
  //Standard_Real anYScaleToMove = anYDiff / aHeightInPixels;

  //TColgp_SequenceOfPnt2d aTargetPixelPoints = projectToView (aPoints, aTargetView);
  //Standard_Real aTargetWidthInPixels = aTargetPixelPoints.Value(2).Distance (aTargetPixelPoints.Value(1));
  //Standard_Real aTargetHeightInPixels = aTargetPixelPoints.Value(3).Distance (aTargetPixelPoints.Value(2));
  //if (aTargetWidthInPixels < Precision::Confusion() ||
  //    aTargetHeightInPixels < Precision::Confusion())
  //  return;

  //gp_XYZ aTargetCameraDimensions = aTargetView->Camera()->ViewDimensions();
  //Standard_Real aTargetXDiff = aXScaleToMove * aTargetWidthInPixels;
  //Standard_Real aTargetYDiff = anYScaleToMove * aTargetHeightInPixels;

  //aTargetView->Pan (aTargetXDiff, aTargetYDiff);

  //myStartPosition = gp_Pnt2d (theX, theY);
}

// =======================================================================
// function : CreateView
// purpose :
// =======================================================================
void View_CameraPositionPrs::Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                      const Handle(Prs3d_Presentation)& thePresentation,
                                      const Standard_Integer theMode)
{
  thePresentation->Clear();
  if (GetView().IsNull())
    return;

  Handle (Graphic3d_Camera) aCamera = GetView()->Camera();

  Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePresentation);
  Handle(Graphic3d_AspectLine3d) aLineAspect = new Graphic3d_AspectLine3d (Quantity_NOC_RED, Aspect_TOL_SOLID, 5);
  aGroup->SetGroupPrimitivesAspect (aLineAspect);

  gp_Pnt aCameraCenter = aCamera->Center();
  gp_XYZ aCameraDims = aCamera->ViewDimensions();
  gp_XYZ aCameraHalfDims = 0.5 * aCameraDims;

  gp_Dir anEyeDir (gp_Vec (aCameraCenter, aCamera->Eye()));
  gp_Dir anUpVec = aCamera->Up();

  gp_Dir aZAxis = anEyeDir;
  gp_Dir anYAxis = anUpVec;
  gp_Dir aXAxis = aZAxis ^ anYAxis;

  aZAxis = gp_Dir ((gp_Vec (aCamera->Eye(), aCameraCenter)));
  aZAxis.Reverse();
  double aZValue = 0;//aCamera->Scale();

  TColgp_SequenceOfPnt aPoints = getCameraPoints (aCamera);
  {
    //aZValue = aZAxis.Z() * aCamera->Scale();

    Handle(Graphic3d_ArrayOfPolylines) aSegments = new Graphic3d_ArrayOfPolylines (aPoints.Size());
    for (Standard_Integer aPointIndex = 1, aPointCount = aPoints.Length(); aPointIndex <= aPointCount; aPointIndex++)
    {
      aSegments->SetVertice (aPointIndex, aPoints (aPointIndex));
      ////aSegments->SetVertice (aPointIndex, gp_Pnt (aPoints (aPointIndex).X(), aPoints (aPointIndex).Y(), aZValue));
      //gp_Pnt aPoint = aPoints (aPointIndex);
      //aPoint.Translate(gp_Vec(aCamera->Eye(), gp::Origin()));

      //aSegments->SetVertice (aPointIndex, gp_Pnt (aPoint.X(), aPoint.Y(), aPoint.Z()));
    }
    aGroup->AddPrimitiveArray (aSegments);
  }

  {
    aGroup = Prs3d_Root::NewGroup (thePresentation);
    aLineAspect = new Graphic3d_AspectLine3d (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 3);
    aGroup->SetGroupPrimitivesAspect (aLineAspect);

    //aZValue = aZAxis.Z() * aCamera->Scale();
    aZValue = 20;//aCamera->ZFar() / aCamera->Scale();

    double aDistance = aCameraCenter.Distance (aCamera->Eye());
    Handle (Graphic3d_Camera) aDefCamera = GetView()->DefaultCamera();
    double aDefCameraScale = aDefCamera->Scale();
    double aScale = aCamera->Scale();

    double aMoveDistance = -aDistance + aDistance * (1 - aScale / aDefCameraScale);

    //std::cout << "aDistance = " << aDistance << "aScale = " << aScale << "1 - aScale / aDefCameraScale = " << 1 - aScale / aDefCameraScale
    //  << "aMoveDistance = " << aMoveDistance << std::endl;

    Handle(Graphic3d_ArrayOfPolylines) aSegments = new Graphic3d_ArrayOfPolylines (aPoints.Size());
    for (Standard_Integer aPointIndex = 1, aPointCount = aPoints.Length(); aPointIndex <= aPointCount; aPointIndex++)
    {
      //aSegments->SetVertice (aPointIndex, aPoints (aPointIndex));
      //aSegments->SetVertice (aPointIndex, gp_Pnt (aPoints (aPointIndex).X(), aPoints (aPointIndex).Y(), aZValue));
      gp_Pnt aPoint = aPoints (aPointIndex);
      gp_Dir aDir = gp_Dir (gp_Vec(aCamera->Eye(), aCameraCenter));
      gp_Pnt aNewPoint = gp_Pnt(aPoint.XYZ() + aDir.XYZ() * aMoveDistance);
      gp_Vec aVec (aPoint, aNewPoint);
      aPoint.Translate(aVec);

      aSegments->SetVertice (aPointIndex, gp_Pnt (aPoint.X(), aPoint.Y(), aPoint.Z()));

    }
    aGroup->AddPrimitiveArray (aSegments);
  }

  Handle(Graphic3d_ArrayOfPoints) aVertices = new Graphic3d_ArrayOfPoints(1);
  aVertices->SetVertice (1, aCameraCenter);

  Handle(Graphic3d_AspectMarker3d) aMarkerAspect = new Graphic3d_AspectMarker3d (Aspect_TOM_PLUS, Quantity_NOC_GREEN, 3);
  aGroup->SetGroupPrimitivesAspect (aMarkerAspect);
  aGroup->AddPrimitiveArray(aVertices);
}

// =======================================================================
// function : ComputeSelection
// purpose :
// =======================================================================
void View_CameraPositionPrs::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                               const Standard_Integer theMode)
{
}

// =======================================================================
// function : GetCamera
// purpose :
// =======================================================================
Handle(V3d_View) View_CameraPositionPrs::GetView() const
{
  Handle(V3d_Viewer) aViewer = myContext->CurrentViewer();

  int aViewId = 0;
  for (V3d_ListOfViewIterator anActiveViewIter (aViewer->ActiveViewIterator()); anActiveViewIter.More(); anActiveViewIter.Next())
  {
    Handle(V3d_View) aView = anActiveViewIter.Value();
    if (aView->View().IsNull())
      continue;

    if (myViewId == aViewId)
      return aView;
    aViewId++;
  }
  return Handle(V3d_View)();
}

// =======================================================================
// function : getCameraPoints
// purpose :
// =======================================================================
TColgp_SequenceOfPnt View_CameraPositionPrs::getCameraPoints (const Handle (Graphic3d_Camera)& theCamera)
{
  gp_Pnt aCameraCenter = theCamera->Center();
  gp_XYZ aCameraDims = theCamera->ViewDimensions();
  gp_XYZ aCameraHalfDims = 0.5 * aCameraDims;

  gp_Dir anEyeDir (gp_Vec (aCameraCenter, theCamera->Eye()));
  gp_Dir anUpVec = theCamera->Up();

  gp_Dir aZAxis = anEyeDir;
  gp_Dir anYAxis = anUpVec;
  gp_Dir aXAxis = aZAxis ^ anYAxis;

  aZAxis = gp_Dir ((gp_Vec (theCamera->Eye(), aCameraCenter)));
  aZAxis.Reverse();
  double aZValue = 0;//aCamera->Scale();

  TColgp_SequenceOfPnt aPoints;
  aPoints.Append (gp_Pnt (aCameraCenter.XYZ() - aXAxis.XYZ() * aCameraHalfDims.X() + anYAxis.XYZ() * aCameraHalfDims.Y()));
  aPoints.Append (gp_Pnt (aCameraCenter.XYZ() + aXAxis.XYZ() * aCameraHalfDims.X() + anYAxis.XYZ() * aCameraHalfDims.Y()));
  aPoints.Append (gp_Pnt (aCameraCenter.XYZ() + aXAxis.XYZ() * aCameraHalfDims.X() - anYAxis.XYZ() * aCameraHalfDims.Y()));
  aPoints.Append (gp_Pnt (aCameraCenter.XYZ() - aXAxis.XYZ() * aCameraHalfDims.X() - anYAxis.XYZ() * aCameraHalfDims.Y()));
  aPoints.Append (gp_Pnt (aCameraCenter.XYZ() - aXAxis.XYZ() * aCameraHalfDims.X() + anYAxis.XYZ() * aCameraHalfDims.Y()));

  return aPoints;
}

// =======================================================================
// function : projectToView
// purpose :
// =======================================================================
TColgp_SequenceOfPnt2d View_CameraPositionPrs::projectToView (const TColgp_SequenceOfPnt& thePoints,
                                                              const Handle(V3d_View)& theView)
{
  TColgp_SequenceOfPnt2d aPoints;

  Standard_Integer aX, anY;
  for (Standard_Integer aPointIndex = 1, aPointCount = thePoints.Length(); aPointIndex <= aPointCount; aPointIndex++)
  {
    gp_Pnt aPoint = thePoints (aPointIndex);

    theView->Convert (aPoint.X(), aPoint.Y(), aPoint.Z(), aX, anY);
    aPoints.Append (gp_Pnt2d (aX, anY));
  }
  return aPoints;
}

// Created on: 2019-07-14
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <inspector/View_DisplayPreview.hxx>
#include <inspector/View_PreviewParameters.hxx>
#include <inspector/View_Tools.hxx>

#include <inspector/ViewControl_Tools.hxx>

#include <inspector/Convert_Tools.hxx>
#include <inspector/Convert_TransientShape.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <Geom_Axis2Placement.hxx>
#include <Prs3d_PointAspect.hxx>
#include <TopoDS_Compound.hxx>

void enableGlobalClipping (const Handle(AIS_InteractiveObject)& theObject, const bool theIsEnable)
{
  if (theIsEnable)
  {
    theObject->SetClipPlanes (Handle(Graphic3d_SequenceOfHClipPlane)());
  }
  else
  {
    Handle(Graphic3d_SequenceOfHClipPlane) aPlanes = new Graphic3d_SequenceOfHClipPlane;
    aPlanes->SetOverrideGlobal (Standard_True);
    theObject->SetClipPlanes (aPlanes);
  }
}

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
View_DisplayPreview::View_DisplayPreview()
{
  myPreviewParameters = new View_PreviewParameters();
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void View_DisplayPreview::SetContext (const Handle(AIS_InteractiveContext)& theContext)
{
  if (myContext == theContext)
    return;

  // remove all preview presentations from the previous context, display it in the new 

  myContext = theContext;
  if (!theContext.IsNull())
    myPreviewParameters->GetDrawer()->Link (theContext->DefaultDrawer());
}

// =======================================================================
// function : UpdatePreview
// purpose :
// =======================================================================
void View_DisplayPreview::UpdatePreview (const View_DisplayActionType,
                                         const NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  if (myContext.IsNull())
    return;

  // clear previous previews
  for (NCollection_List<Handle(AIS_InteractiveObject)>::Iterator anIterator (myPreviewReadyPresentations); anIterator.More(); anIterator.Next())
  {
    if (!anIterator.Value()->GetContext().IsNull())
      anIterator.Value()->GetContext()->Remove (anIterator.Value(), Standard_True);
  }
  myPreviewReadyPresentations.Clear();

  if (thePresentations.IsEmpty())
  {
    if (!myPreviewPresentation.IsNull() && !myPreviewPresentation->GetContext().IsNull())
      myPreviewPresentation->GetContext()->Remove (myPreviewPresentation, Standard_True);
    myPreviewPresentation = NULL;

    return;
  }

  // display parameter previews
  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);
  for (NCollection_List<Handle(Standard_Transient)>::Iterator anIterator (thePresentations); anIterator.More(); anIterator.Next())
  {
    Handle(Convert_TransientShape) aShapePtr = Handle(Convert_TransientShape)::DownCast (anIterator.Value());
    if (!aShapePtr.IsNull())
    {
      aBuilder.Add (aCompound, aShapePtr->GetShape());
    }
    Handle(AIS_InteractiveObject) aPrs = Handle(AIS_InteractiveObject)::DownCast (anIterator.Value());
    if (!aPrs.IsNull() && aPrs->GetContext().IsNull()/*is not displayed in another context*/)
    {
      myContext->Display (aPrs, AIS_Shaded, -1/*do not participate in selection*/, Standard_True);
      enableGlobalClipping(aPrs, false);
      myPreviewReadyPresentations.Append (aPrs);
    }
  }

  if (myPreviewPresentation.IsNull())
  {
    myPreviewPresentation = new AIS_Shape (aCompound);
    Quantity_Color aColor(Quantity_NOC_TOMATO);
    myPreviewPresentation->Attributes()->SetPointAspect (new Prs3d_PointAspect (Aspect_TOM_O_PLUS, aColor, 3.0));
    myPreviewPresentation->SetAttributes (myPreviewParameters->GetDrawer());

    //myPreviewPresentation->SetTransformPersistence(thePersistent);
    myContext->Display (myPreviewPresentation, AIS_Shaded, -1/*do not participate in selection*/, Standard_True);
    enableGlobalClipping(myPreviewPresentation, false);
  }
  else
  {
    Handle(AIS_Shape)::DownCast (myPreviewPresentation)->Set (aCompound);
    //myPreviewPresentation->SetTransformPersistence(thePersistent);
    if (!myPreviewPresentation.IsNull() && !myPreviewPresentation->GetContext().IsNull())
    {
      myPreviewPresentation->GetContext()->Redisplay (myPreviewPresentation, Standard_True);
    }
  }
}

// =======================================================================
// function : DisplayDefaultPreview
// purpose :
// =======================================================================

void View_DisplayPreview::DisplayDefaultPreview()
{
  if (myContext.IsNull())
    return;

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (gp_Pnt(25., 10., 0.)));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt(20., 20., 0.), gp_Pnt(30., 20., 10.)));
  //aBuilder.Add (aCompound, BRepBuilderAPI_MakeFace (gp_Pln (gp_Pnt (20., 30., 0.), gp_Dir (1., 0., 0.))).Face());
  aBuilder.Add (aCompound, Convert_Tools::CreateBoxShape (gp_Pnt(20., 40., 0.), gp_Pnt(30., 60., 10.)));

  Handle(AIS_Shape) aDefaultPreview = new AIS_Shape (aCompound);
  aDefaultPreview->SetAttributes (myPreviewParameters->GetDrawer());
  myContext->Display (aDefaultPreview, AIS_Shaded, -1/*do not participate in selection*/, Standard_True);
}

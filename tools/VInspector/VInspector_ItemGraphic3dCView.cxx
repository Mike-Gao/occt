// Created on: 2018-08-16
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

#include <inspector/VInspector_ItemGraphic3dCView.hxx>

#include <inspector/VInspector_ItemV3dView.hxx>
#include <inspector/VInspector_ItemGraphic3dCamera.hxx>
#include <inspector/VInspector_ItemGraphic3dClipPlane.hxx>
#include <inspector/VInspector_ItemOpenGlLayerList.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <Graphic3d_CView.hxx>
#include <OpenGl_View.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : GetLayerList
// purpose :
// =======================================================================
const OpenGl_LayerList& VInspector_ItemGraphic3dCView::GetLayerList (Standard_Boolean& isDefault) const
{
  isDefault = Standard_True;

  Handle(Graphic3d_CView) aCView = GetCView();
  if (aCView.IsNull())
    return myDefaultLayer;

  Handle(OpenGl_View) aOpenGlView = Handle(OpenGl_View)::DownCast (aCView);
  if (aOpenGlView.IsNull())
    return myDefaultLayer;

  isDefault = Standard_False;
  return aOpenGlView->LayerList();
}

// =======================================================================
// function : GetClipPlane
// purpose :
// =======================================================================
Handle(Graphic3d_ClipPlane) VInspector_ItemGraphic3dCView::GetClipPlane(const int /*theRow*/)
{
  //Handle(V3d_View) aView = GetView();

  //const Handle(Graphic3d_SequenceOfHClipPlane)& aClipPlanes = aView->ClipPlanes();

  //Standard_Integer aPlaneId = 0;
  //for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aClipPlanes); aPlaneIt.More(); aPlaneIt.Next(), ++aPlaneId)
  //{
  //  if (aPlaneId == theRow)
  //    return aPlaneIt.Value();
  //}
  return 0;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCView::Init()
{
  VInspector_ItemV3dViewPtr aParentItem = itemDynamicCast<VInspector_ItemV3dView>(Parent());

  myCView = aParentItem->GetView()->View();

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCView::Reset()
{
  VInspector_ItemBase::Reset();
  myCView = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCView::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemGraphic3dCView*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemGraphic3dCView::initRowCount() const
{
  if (Column() != 0)
    return 0;

  int aNbElements = 2; // Camera, OpenGl_LayerList

  Handle(Graphic3d_CView) aCView = GetCView();
  const Handle(Graphic3d_SequenceOfHClipPlane)& aClipPlanes = aCView->ClipPlanes();
  if (!aClipPlanes.IsNull())
    aNbElements += aClipPlanes->Size();

  return aNbElements;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemGraphic3dCView::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(Graphic3d_CView) aCView = GetCView();
  if (aCView.IsNull())
    return Column() == 0 ? "Empty Graphic3d_CView" : "";

  switch (Column())
  {
    case 0: return theItemRole == Qt::DisplayRole ? aCView->DynamicType()->Name()
                                                  : STANDARD_TYPE (Graphic3d_CView)->Name();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemGraphic3dCView::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemGraphic3dCamera::CreateItem (currentItem(), theRow, theColumn);
  else if (theRow == 1)
    return VInspector_ItemOpenGlLayerList::CreateItem (currentItem(), theRow, theColumn);
  else
    return VInspector_ItemGraphic3dClipPlane::CreateItem (currentItem(), theRow, theColumn);
}

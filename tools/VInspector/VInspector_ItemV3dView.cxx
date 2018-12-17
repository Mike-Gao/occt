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

#include <inspector/VInspector_ItemV3dView.hxx>

#include <AIS.hxx>
#include <AIS_InteractiveContext.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Compound.hxx>
#include <V3d_ListOfLight.hxx>

#include <inspector/ViewControl_Tools.hxx>
#include <inspector/VInspector_ItemAspectWindow.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemGraphic3dCamera.hxx>
#include <inspector/VInspector_ItemGraphic3dCView.hxx>
#include <inspector/VInspector_ItemV3dViewer.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemV3dView::initRowCount() const
{
  if (Column() != 0)
    return 0;

  return 3; // 0 - Default Camera, 1 - Aspect_Window, 2 - CView
  // TODO: V3d_ListOfLight, V3d_Trihedron,
  //Aspect_Grid-MyPlane-MyTrsf-MyGridEchoStructure-MyGridEchoGroup
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemV3dView::initValue (const int theItemRole) const
{
  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  if (GetView().IsNull())
    return Column() == 0 ? "Empty view" : "";

  switch (Column())
  {
    case 0: return GetView()->DynamicType()->Name();
    case 1: return rowCount();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemV3dView::Init()
{
  VInspector_ItemV3dViewerPtr aParentItem = itemDynamicCast<VInspector_ItemV3dViewer>(Parent());
  Handle(V3d_View) aView;
  if (aParentItem)
  {
    aView = aParentItem->GetView (Row());
  }
  setView (aView);

  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemV3dView::Reset()
{
  VInspector_ItemBase::Reset();

  setView (NULL);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemV3dView::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemV3dView*>(this)->Init();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemV3dView::GetTableRowCount() const
{
  return 60;
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType VInspector_ItemV3dView::GetTableEditType (const int, const int) const
{
  return ViewControl_EditType_None;
}

// =======================================================================
// function : GetTableEnumValues
// purpose :
// =======================================================================
QList<QVariant> VInspector_ItemV3dView::GetTableEnumValues (const int, const int) const
{
  QList<QVariant> aValues;
  return aValues;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemV3dView::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  Handle(V3d_View) aView = GetView();
  if (aView.IsNull())
    return QVariant();

  bool isFirstColumn = theColumn == 0;
  switch (theRow)
  {
    case 0:
    {
      if (isFirstColumn)
        return QVariant ("ImmediateUpdate");

      Standard_Boolean aCurState = aView->SetImmediateUpdate (Standard_False);
      aView->SetImmediateUpdate (aCurState);

      return aCurState;
    }
    break;
    case 1:
    {
      if (isFirstColumn)
        return QVariant ("ActiveLights");

      V3d_ListOfLightIterator aLightsIt = aView->ActiveLightIterator();
      Standard_Integer aNbOfLights = 0;
      while (aLightsIt.More())
      {
        aNbOfLights++;
        aLightsIt.Next();
      }
      return aNbOfLights;
    }
    case 2:
    {
      if (isFirstColumn)
        return QVariant ("Axis: origin");

      Standard_Real aX, anY, aZ, aVx, aVy, aVz;
      aView->Axis (aX, anY, aZ, aVx, aVy, aVz);

      return ViewControl_Tools::ToString (gp_Pnt (aX, anY, aZ)).ToCString();
    }
    case 3:
    {
      if (isFirstColumn)
        return QVariant ("Axis: direction");

      Standard_Real aX, anY, aZ, aVx, aVy, aVz;
      aView->Axis (aX, anY, aZ, aVx, aVy, aVz);

      return ViewControl_Tools::ToString (gp_Dir (aVx, aVy, aVz)).ToCString();
    }
    case 4: return isFirstColumn ? QVariant ("ComputedMode") : QVariant (aView->ComputedMode());
    case 5: return isFirstColumn ? QVariant ("AutoZFitMode") : QVariant (aView->AutoZFitMode());
    case 6: return isFirstColumn ? QVariant ("AutoZFitScaleFactor") : QVariant (aView->AutoZFitScaleFactor());
  }
  return QVariant();
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool VInspector_ItemV3dView::SetTableData (const int, const int, const QVariant&)
{
  return true;
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemV3dView::buildPresentationShape (const Handle(V3d_View)& theView)
{
  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  Standard_Real aX, anY, aZ, aVx, aVy, aVz;
  theView->Axis (aX, anY, aZ, aVx, aVy, aVz);
  gp_Pnt anOrigin (aX, anY, aZ);

  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (anOrigin));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Lin (anOrigin, gp_Dir (aVx, aVy, aVz))));

  return aCompound;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemV3dView::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemGraphic3dCamera::CreateItem (currentItem(), theRow, theColumn);
  else if (theRow == 1)
    return VInspector_ItemAspectWindow::CreateItem (currentItem(), theRow, theColumn);
  else if (theRow == 2)
    return VInspector_ItemGraphic3dCView::CreateItem (currentItem(), theRow, theColumn);

  return TreeModel_ItemBasePtr();
}

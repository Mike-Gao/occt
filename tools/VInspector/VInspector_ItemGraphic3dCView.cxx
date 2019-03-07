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
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <Graphic3d_CView.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : GetClipPlane
// purpose :
// =======================================================================
Handle(Graphic3d_ClipPlane) VInspector_ItemGraphic3dCView::GetClipPlane(const int theRow)
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

  int aNbElements = 1; // Camera

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
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemGraphic3dCView::GetTableRowCount() const
{
  return 10;
}

// =======================================================================
// function : GetTableEditType
// purpose :
// =======================================================================
ViewControl_EditType VInspector_ItemGraphic3dCView::GetTableEditType (const int theRow, const int) const
{
  switch (theRow)
  {
    /*case 4: return ViewControl_EditType_Line;
    case 5: return ViewControl_EditType_Combo;
    case 6: return ViewControl_EditType_Bool;
    case 12: return ViewControl_EditType_Bool;
    case 17: return ViewControl_EditType_Combo;
    case 18: return ViewControl_EditType_Bool;
    case 22: return ViewControl_EditType_Bool;*/
    default: return ViewControl_EditType_None;
  }
}

// =======================================================================
// function : GetTableEnumValues
// purpose :
// =======================================================================
QList<QVariant> VInspector_ItemGraphic3dCView::GetTableEnumValues (const int theRow, const int) const
{
  QList<QVariant> aValues;
  /*switch (theRow)
  {
    case 5:
    {
      for (int i = 0; i <= Aspect_TOFM_FRONT_SIDE; i++)
        aValues.append (Aspect::TypeOfFacingModelToString((Aspect_TypeOfFacingModel)i));
    }
    break;
    case 17:
    {
      for (int i = 0; i <= PrsMgr_TOP_ProjectorDependant; i++)
        aValues.append (PrsMgr::TypeOfPresentation3dToString ((PrsMgr_TypeOfPresentation3d)i));
    }
    break;
    default: break;
  }*/
  return aValues;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemGraphic3dCView::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  Handle(Graphic3d_CView) aCView = GetCView();
  if (aCView.IsNull())
    return QVariant();

  //bool isFirstColumn = theColumn == 0;
  //switch (theRow)
  //{
  //  case 0: return isFirstColumn ? QVariant ("LineAspect") : QVariant (ViewControl_Tools::GetPointerInfo (aGroup->LineAspect()).ToCString());
  //  case 1: return isFirstColumn ? QVariant ("FillAreaAspect") : QVariant (ViewControl_Tools::GetPointerInfo (aGroup->FillAreaAspect()).ToCString());
  //  case 2: return isFirstColumn ? QVariant ("TextAspect") : QVariant (ViewControl_Tools::GetPointerInfo (aGroup->TextAspect()).ToCString());
  //  case 3: return isFirstColumn ? QVariant ("MarkerAspect") : QVariant (ViewControl_Tools::GetPointerInfo (aGroup->MarkerAspect()).ToCString());
  //  case 4: return isFirstColumn ? QVariant ("ContainsFacet") : QVariant (aGroup->ContainsFacet());
  //  case 5: return isFirstColumn ? QVariant ("IsDeleted") : QVariant (aGroup->IsDeleted());
  //  case 6: return isFirstColumn ? QVariant ("IsEmpty") : QVariant (aGroup->IsEmpty());
  //  case 7: return isFirstColumn ? QVariant ("IsClosed") : QVariant (aGroup->IsClosed());
  //  case 8:
  //  {
  //    if (isFirstColumn)
  //      return QVariant ("MinMaxValues");
  //    Standard_Real aXMin, anYMin, aZMin, aXMax, anYMax, aZMax;
  //    aGroup->MinMaxValues (aXMin, anYMin, aZMin, aXMax, anYMax, aZMax);
  //    Bnd_Box aBox;
  //    aBox.Update(aXMin, anYMin, aZMin, aXMax, anYMax, aZMax);
  //    return QVariant (ViewControl_Tools::ToString (aBox).ToCString());
  //  }
  //  case 9:
  //  {
  //    if (isFirstColumn)
  //      return QVariant ("BoundingBox");
  //    const Graphic3d_BndBox4f& aBndBox = aGroup->BoundingBox();
  //    Bnd_Box aBox;
  //    aBox.Update((Standard_Real )aBndBox.CornerMin().x(),
  //                (Standard_Real )aBndBox.CornerMin().y(),
  //                (Standard_Real )aBndBox.CornerMin().z(),
  //                (Standard_Real )aBndBox.CornerMax().x(),
  //                (Standard_Real )aBndBox.CornerMax().y(),
  //                (Standard_Real )aBndBox.CornerMax().z());
  //    return QVariant (ViewControl_Tools::ToString (aBox).ToCString());
  //  }
  //}
  return QVariant();
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool VInspector_ItemGraphic3dCView::SetTableData (const int theRow, const int, const QVariant& theValue)
{
  /*Handle(AIS_InteractiveObject) aPrs = GetInteractiveObject();
  switch (theRow)
  {
    case 4:
    {
      double aValue = theValue.toDouble();
      if (aValue > 0) aPrs->SetWidth (aValue);
      else aPrs->UnsetWidth();
    }
    break;
    case 5: aPrs->SetCurrentFacingModel (Aspect::TypeOfFacingModelFromString (theValue.toString().toStdString().c_str()));
    case 6: aPrs->SetInfiniteState (theValue.toBool());
    case 12: aPrs->SetAutoHilight(theValue.toBool());
    case 17: aPrs->SetTypeOfPresentation (PrsMgr::TypeOfPresentation3dFromString (theValue.toString().toStdString().c_str()));
    case 18: aPrs->SetMutable (theValue.toBool());
    case 22: if (!theValue.toBool()) aPrs->ResetTransformation();
    default: return false;
  }*/
  return true;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemGraphic3dCView::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemGraphic3dCamera::CreateItem (currentItem(), theRow, theColumn);
  else
    return VInspector_ItemGraphic3dClipPlane::CreateItem (currentItem(), theRow, theColumn);
}

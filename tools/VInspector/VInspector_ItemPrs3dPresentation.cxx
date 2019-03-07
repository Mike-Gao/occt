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

#include <inspector/VInspector_ItemPrs3dPresentation.hxx>
#include <inspector/VInspector_ItemGraphic3dCStructure.hxx>
#include <inspector/VInspector_ItemPresentations.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Graphic3d.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemPrs3dPresentation::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (Column() != 0 || (theItemRole != Qt::DisplayRole && theItemRole != Qt::ToolTipRole))
    return QVariant();

  Handle(Prs3d_Presentation) aPresentation = GetPresentation();
  bool aNullPresentation = aPresentation.IsNull();

  switch (Column())
  {
    case 0:
    {
      return theItemRole == Qt::ToolTipRole
        ? (aNullPresentation ? QVariant("Prs3d_Presentation is empty") : QVariant (aPresentation->DynamicType()->Name()))
        : QVariant (myName.ToCString());
    }
  }

  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemPrs3dPresentation::initRowCount() const
{
  if (Column() != 0)
    return 0;

  //Handle(Prs3d_Presentation) aPresentation = GetPresentation();

  //Standard_EXPORT const Handle(Graphic3d_SequenceOfHClipPlane)& ClipPlanes() const;

  return 1; //aPresentation->NumberOfGroups();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemPrs3dPresentation::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
    return VInspector_ItemGraphic3dCStructure::CreateItem (currentItem(), theRow, theColumn);
  //if (theRow == 1)
  //  return VInspector_ItemPrs3dPresentation::CreateItem (currentItem(), theRow, theColumn);
  //else
  //  return VInspector_ItemSelectMgrSelection::CreateItem(currentItem(), theRow, theColumn);
  return TreeModel_ItemBasePtr();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemPrs3dPresentation::Init()
{
  VInspector_ItemPresentationsPtr aParentItem = itemDynamicCast<VInspector_ItemPresentations>(Parent());

  TCollection_AsciiString aName;
  Handle(Prs3d_Presentation) aPresentation = aParentItem->GetPresentation (Row(), aName);
  setPresentation (aPresentation, aName);
  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemPrs3dPresentation::Reset()
{
  VInspector_ItemBase::Reset();

  setPresentation (NULL, "None");
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemPrs3dPresentation::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemPrs3dPresentation*>(this)->Init();
}

// =======================================================================
// function : setPresentation
// purpose :
// =======================================================================
void VInspector_ItemPrs3dPresentation::setPresentation (const Handle(Prs3d_Presentation)& thePresentation,
                                                         const TCollection_AsciiString& theName)
{
  myPresentation = thePresentation;
  myName = theName;
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemPrs3dPresentation::GetTableRowCount() const
{
  return 14;
}

// =======================================================================
// function : GetTableEnumValues
// purpose :
// =======================================================================
QList<QVariant> VInspector_ItemPrs3dPresentation::GetTableEnumValues (const int theRow, const int) const
{
  QList<QVariant> aValues;
  switch (theRow)
  {
    case 4:
    {
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_UNKNOWN));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Default));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Top));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_Topmost));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_TopOSD));
      aValues.append (Graphic3d::ZLayerIdToString (Graphic3d_ZLayerId_BotOSD));
    }
    break;
    default: break;
  }
  return aValues;
}


// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemPrs3dPresentation::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  bool isFirstColumn = theColumn == 0;

  Handle(Prs3d_Presentation) aPrs = GetPresentation();
  switch (theRow)
  {
    case 0: return ViewControl_Table::SeparatorData();
    case 1: return isFirstColumn ? QVariant (STANDARD_TYPE (Prs3d_Presentation)->Name())
                                 : ViewControl_Tools::GetPointerInfo (aPrs).ToCString();
    case 2: return ViewControl_Table::SeparatorData();
    case 3: return isFirstColumn ? QVariant ("DisplayPriority") : QVariant (aPrs->DisplayPriority());
    case 4: return isFirstColumn ? QVariant ("GetZLayer")
                                 : QVariant (Graphic3d::ZLayerIdToString (aPrs->GetZLayer()));
    case 5: return isFirstColumn ? QVariant ("MinMaxValues")
                                 : QVariant (ViewControl_Tools::ToString (aPrs->MinMaxValues()).ToCString());
    case 6: return isFirstColumn ? QVariant ("ContainsFacet") : QVariant (aPrs->ContainsFacet());
    case 7: return isFirstColumn ? QVariant ("IsDeleted") : QVariant (aPrs->IsDeleted());
    case 8: return isFirstColumn ? QVariant ("IsDisplayed") : QVariant (aPrs->IsDisplayed());
    case 9: return isFirstColumn ? QVariant ("IsEmpty") : QVariant (aPrs->IsEmpty());
    case 10: return isFirstColumn ? QVariant ("IsInfinite") : QVariant (aPrs->IsInfinite());
    case 11: return isFirstColumn ? QVariant ("IsHighlighted") : QVariant (aPrs->IsHighlighted());
    case 12: return isFirstColumn ? QVariant ("IsTransformed") : QVariant (aPrs->IsTransformed());
    case 13: return isFirstColumn ? QVariant ("IsVisible") : QVariant (aPrs->IsVisible());
    default: return QVariant();
  }
  return QVariant();
}

// =======================================================================
// function : SetTableData
// purpose :
// =======================================================================
bool VInspector_ItemPrs3dPresentation::SetTableData (const int theRow, const int, const QVariant& theValue)
{
  Handle(Prs3d_Presentation) aPrs = GetPresentation();
  switch (theRow)
  {
    case 8: aPrs->SetZLayer (Graphic3d::ZLayerIdFromString (theValue.toString().toStdString().c_str()));
    default: return false;
  }
  return true;
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemPrs3dPresentation::buildPresentationShape()
{
  if (!myPresentation.IsNull())
    myPresentationShape = VInspector_Tools::CreateShape (myPresentation->MinMaxValues());

  return TopoDS_Shape();
}

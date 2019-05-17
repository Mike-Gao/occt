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

#include <inspector/VInspector_ItemOpenGlElement.hxx>

#include <inspector/VInspector_ItemGraphic3dGroup.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Text.hxx>
#include <SelectMgr.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemOpenGlElement::Init()
{
  VInspector_ItemGraphic3dGroupPtr aParentItem = itemDynamicCast<VInspector_ItemGraphic3dGroup>(Parent());
  myElement = aParentItem->GetElementNode (Row());

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemOpenGlElement::Reset()
{
  VInspector_ItemBase::Reset();
  myElement = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemOpenGlElement::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemOpenGlElement*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemOpenGlElement::initRowCount() const
{
  if (Column() != 0)
    return 0;

  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemOpenGlElement::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(OpenGl_Element) anElement = GetElement();
  if (anElement.IsNull())
    return Column() == 0 ? "Empty element" : "";

  switch (Column())
  {
    case 0:
    {
      return theItemRole == Qt::ToolTipRole ? QVariant ("")
                                            : QVariant (anElement->DynamicType()->Name());
    }
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : GetTableRowCount
// purpose :
// =======================================================================
int VInspector_ItemOpenGlElement::GetTableRowCount() const
{
  return 40;
}

// =======================================================================
// function : GetTableData
// purpose :
// =======================================================================
QVariant VInspector_ItemOpenGlElement::GetTableData (const int theRow, const int theColumn, const int theRole) const
{
  if (theRole != Qt::DisplayRole)
    return QVariant();

  Handle(OpenGl_Element) anElement = GetElement();
  if (anElement.IsNull())
    return QVariant();

  bool isFirstColumn = theColumn == 0;

  if (!Handle(OpenGl_PrimitiveArray)::DownCast (anElement).IsNull())
  {
    Handle(OpenGl_PrimitiveArray) aCElement = Handle(OpenGl_PrimitiveArray)::DownCast (anElement);
    switch (theRow)
    {
      case 0: return isFirstColumn ? QVariant ("IsInitialized") : QVariant (aCElement->IsInitialized());
      case 1: return isFirstColumn ? QVariant ("DrawMode") : QVariant (aCElement->DrawMode());
      case 2: return isFirstColumn ? QVariant ("IsFillDrawMode") : QVariant (aCElement->IsFillDrawMode());

      case 3: return isFirstColumn ? QVariant ("GetUID") : QVariant (aCElement->GetUID());
      case 4: return isFirstColumn ? QVariant ("IndexVbo()")
        : !aCElement->IndexVbo().IsNull() ? QVariant (ViewControl_Tools::GetPointerInfo (aCElement->IndexVbo()).ToCString()) : QVariant();
      case 5: return isFirstColumn ? QVariant ("AttributesVbo()")
        : !aCElement->AttributesVbo().IsNull() ? QVariant (ViewControl_Tools::GetPointerInfo (aCElement->AttributesVbo()).ToCString()) : QVariant();

      case 6: return isFirstColumn ? QVariant ("Indices") : VInspector_Tools::ToVariant (aCElement->Indices());
      case 7: return isFirstColumn ? QVariant ("Attributes") : VInspector_Tools::ToVariant (aCElement->Attributes());
      case 8: return isFirstColumn ? QVariant ("Bounds") : VInspector_Tools::ToVariant (aCElement->Bounds());
      default: return QVariant();
    }
  }
  else if (!Handle(OpenGl_Text)::DownCast (anElement).IsNull())
  {
    Handle(OpenGl_Text) aCmElement = Handle(OpenGl_Text)::DownCast (anElement);
    switch (theRow)
    {
      case 0:
      {
        if (isFirstColumn)
          return QVariant ("PixelTolerance");
        //: QVariant (myContext->PixelTolerance());
        return QVariant();
      }
      default: return QVariant();
    }
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemOpenGlElement::createChild (int theRow, int theColumn)
{
  (void)theRow;
  (void)theColumn;
  //if (theRow == 0)
  //  return VInspector_ItemFolderObject::CreateItem (currentItem(), theRow, theColumn);
  //else
  //  return VInspector_ItemPresentableObject::CreateItem (currentItem(), theRow, theColumn);

  return TreeModel_ItemBasePtr();
}
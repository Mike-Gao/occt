// Created on: 2018-08-26
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

#include <inspector/VInspector_ItemPrs3dAspect.hxx>
#include <inspector/VInspector_ItemPrs3dDrawer.hxx>
#include <inspector/ViewControl_ColorSelector.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Prs3d.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

int GetMaterialRows() { return 25; }

// =======================================================================
// function : initValue
// purpose :
// =======================================================================

QVariant VInspector_ItemPrs3dAspect::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole == Qt::DisplayRole || theItemRole == Qt::ToolTipRole)
  {
    Handle(Prs3d_BasicAspect) anAspect = GetAspect();
    bool aNullAspect = anAspect.IsNull();
    switch (Column())
    {
      case 0:
      {
        return theItemRole == Qt::ToolTipRole
          ? (aNullAspect ? QVariant("Prs3d_BasicAspect is empty") : QVariant (anAspect->DynamicType()->Name()))
          : QVariant (myName.ToCString());
      }
      default: break;
    }
  }
  if (theItemRole == Qt::ForegroundRole)
  {
    Handle(Prs3d_BasicAspect) anAspect = GetAspect();
    bool aNullAspect = anAspect.IsNull();
    if (aNullAspect) return QColor (Qt::lightGray);

    VInspector_ItemPrs3dDrawerPtr aParentItem = itemDynamicCast<VInspector_ItemPrs3dDrawer>(Parent());
    if (aParentItem)
    {
      Handle(Prs3d_Drawer) aDrawer = aParentItem->GetDrawer();
      if (aDrawer->Link().IsNull())
        return QVariant(); // default color

      TCollection_AsciiString aName;
      Standard_Boolean isOwnAspect;
      aParentItem->GetPrs3dAspect(Row(), aName, isOwnAspect);
    if (!isOwnAspect)
      return QColor (Qt::lightGray);
    }
    else
    {
      VInspector_ItemPrs3dAspectPtr aParentAspectItem = itemDynamicCast<VInspector_ItemPrs3dAspect>(Parent());
      if (aParentAspectItem)
        return aParentAspectItem->initValue (theItemRole);
    }
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================

int VInspector_ItemPrs3dAspect::initRowCount() const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return 0;

  Standard_CString anAspectKind = anAspect->DynamicType()->Name();
  if (anAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
    return 3 + 2 * (int) Prs3d_DP_None;
  else if (anAspectKind == STANDARD_TYPE (Prs3d_PlaneAspect)->Name())
    return 3;
  else if (anAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
    return 3;

  return 0;
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void VInspector_ItemPrs3dAspect::initStream (Standard_OStream& theOStream) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return;

  anAspect->DumpJson (theOStream);
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================

TreeModel_ItemBasePtr VInspector_ItemPrs3dAspect::createChild (int theRow, int theColumn)
{
  return VInspector_ItemPrs3dAspect::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemPrs3dAspect::Init()
{
  TCollection_AsciiString aName;
  Handle(Prs3d_BasicAspect) anAspect;
  VInspector_ItemPrs3dDrawerPtr aParentItem = itemDynamicCast<VInspector_ItemPrs3dDrawer>(Parent());

  if (aParentItem)
  {
    Standard_Boolean isOwnAspect;
    anAspect = aParentItem->GetPrs3dAspect(Row(), aName, isOwnAspect);
  }
  else
  {
    VInspector_ItemPrs3dAspectPtr aParentAspectItem = itemDynamicCast<VInspector_ItemPrs3dAspect>(Parent());
    Standard_Boolean isOwnAspect;
    anAspect = aParentAspectItem->GetPrs3dAspect(Row(), aName, isOwnAspect);
  }

  setAspect(anAspect, aName);
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemPrs3dAspect::Reset()
{
  VInspector_ItemBase::Reset();

  setAspect (NULL, TCollection_AsciiString());
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemPrs3dAspect::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemPrs3dAspect*>(this)->Init();
}

// =======================================================================
// function : GetPrs3dAspect
// purpose :
// =======================================================================

Handle(Prs3d_BasicAspect) VInspector_ItemPrs3dAspect::GetPrs3dAspect (const int theRow,
                                                                      TCollection_AsciiString& theName,
                                                                      Standard_Boolean& theOwnAspect) const
{
  Handle(Prs3d_BasicAspect) anAspect = GetAspect();
  if (anAspect.IsNull())
    return 0;

  Standard_CString anAspectKind = anAspect->DynamicType()->Name();
  theOwnAspect = true;
  if (anAspectKind == STANDARD_TYPE (Prs3d_DatumAspect)->Name())
  {
    Handle(Prs3d_DatumAspect) aCustomAspect = Handle(Prs3d_DatumAspect)::DownCast (anAspect);
    switch (theRow) {
      case 0: theName = "TextAspect"; return aCustomAspect->TextAspect();
      case 1: theName = "PointAspect"; return aCustomAspect->PointAspect();
      case 2: theName = "ArrowAspect"; return aCustomAspect->ArrowAspect();
      default:
      {
        if (theRow >= 3 && theRow < 3 + Prs3d_DP_None)
        {
          Prs3d_DatumParts aParts = (Prs3d_DatumParts)(theRow - 3);
          theName = TCollection_AsciiString ("LineAspect: ") + aParts;
          return aCustomAspect->LineAspect (aParts);
        }

        if (theRow >= 3 + Prs3d_DP_None && theRow < 3 + 2 * Prs3d_DP_None)
        {
          Prs3d_DatumParts aParts = (Prs3d_DatumParts)(theRow - (3+ Prs3d_DP_None));
          theName = TCollection_AsciiString ("ShadingAspect: ") + aParts;
          return aCustomAspect->ShadingAspect (aParts);
        }
      }
      break;
    }
  }
  else if (anAspectKind == STANDARD_TYPE (Prs3d_PlaneAspect)->Name())
  {
    Handle(Prs3d_PlaneAspect) aCustomAspect = Handle(Prs3d_PlaneAspect)::DownCast (anAspect);
    switch (theRow)
    {
      case 0: theName = "EdgesAspect"; return aCustomAspect->EdgesAspect();
      case 1: theName = "IsoAspect"; return aCustomAspect->IsoAspect();
      case 2: theName = "ArrowAspect"; return aCustomAspect->ArrowAspect();
    }
  }
  else if (anAspectKind == STANDARD_TYPE (Prs3d_DimensionAspect)->Name())
  {
    Handle(Prs3d_DimensionAspect) aCustomAspect = Handle(Prs3d_DimensionAspect)::DownCast (anAspect);
    switch (theRow)
    {
      case 0: theName = "LineAspect"; return aCustomAspect->LineAspect();
      case 1: theName = "TextAspect"; return aCustomAspect->TextAspect();
      case 2: theName = "ArrowAspect"; return aCustomAspect->ArrowAspect();
    }
  }

  theName = "None";
  return Handle(Prs3d_BasicAspect)();
}

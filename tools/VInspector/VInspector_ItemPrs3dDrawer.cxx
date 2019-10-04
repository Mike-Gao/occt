// Created on: 2018-08-10
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

#include <inspector/VInspector_ItemPrs3dDrawer.hxx>
#include <inspector/VInspector_ItemPrs3dAspect.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <inspector/VInspector_ItemFolderObject.hxx>
#include <inspector/ViewControl_ColorSelector.hxx>
#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <Prs3d.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <TCollection_AsciiString.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================

QVariant VInspector_ItemPrs3dDrawer::initValue (int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  Handle(Prs3d_Drawer) aDrawer = GetDrawer();
  bool aNullDrawer = aDrawer.IsNull();
  if (theItemRole == Qt::DisplayRole || theItemRole == Qt::ToolTipRole)
  {
    switch (Column())
    {
      case 0:
      {
        return theItemRole == Qt::ToolTipRole
          ? (aNullDrawer ? QVariant("Prs3d_Drawer is empty") : QVariant (aDrawer->DynamicType()->Name()))
          : QVariant (myName.ToCString());
      }
      default: break;
    }
  }
  if (theItemRole == Qt::ForegroundRole)
    return !aNullDrawer ? QColor (Qt::black) : QColor (Qt::lightGray);

  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================

int VInspector_ItemPrs3dDrawer::initRowCount() const
{
  VInspector_ItemPrs3dDrawerPtr aParentDrawerItem = itemDynamicCast<VInspector_ItemPrs3dDrawer>(Parent());
  if (aParentDrawerItem)
    return 0;

  Handle(Prs3d_Drawer) aDrawer = GetDrawer();
  if (aDrawer.IsNull())
    return 0;

  int aChildCount = 1; // Link
  aChildCount += 18; // Prs3d_Drawer aspects
  return aChildCount;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================

TreeModel_ItemBasePtr VInspector_ItemPrs3dDrawer::createChild (int theRow, int theColumn)
{
  if (theRow == 0)
  {
    return VInspector_ItemPrs3dDrawer::CreateItem (currentItem(), theRow, theColumn); // "Link"
  }
  else
    return VInspector_ItemPrs3dAspect::CreateItem (currentItem(), theRow, theColumn); // "Aspects"

  //return TreeModel_ItemBasePtr();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemPrs3dDrawer::Init()
{
  VInspector_ItemFolderObjectPtr aParentItem = itemDynamicCast<VInspector_ItemFolderObject>(Parent());
  Handle(Prs3d_Drawer) aDrawer;
  TCollection_AsciiString aName;
  if (aParentItem)
    aDrawer = aParentItem->GetPrs3dDrawer(Row(), aName);
  else
  {
    VInspector_ItemPrs3dDrawerPtr aParentDrawerItem = itemDynamicCast<VInspector_ItemPrs3dDrawer>(Parent());
    if (aParentDrawerItem)
    {
      aDrawer = aParentDrawerItem->GetDrawer()->Link();
      aName = "Link";
    }
  }
  setDrawer (aDrawer, aName);
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemPrs3dDrawer::Reset()
{
  VInspector_ItemBase::Reset();

  setDrawer (NULL, "None");
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemPrs3dDrawer::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemPrs3dDrawer*>(this)->Init();
}

// =======================================================================
// function : GetPrs3dAspect
// purpose :
// =======================================================================

Handle(Prs3d_BasicAspect) VInspector_ItemPrs3dDrawer::GetPrs3dAspect (const int theRow,
                                                                      TCollection_AsciiString& theName,
                                                                      Standard_Boolean& theOwnAspect) const
{
  if (theRow == 0) // "Link" driver, there is no aspect
    return 0;

  Handle(Prs3d_Drawer) aDrawer = GetDrawer();
  if (aDrawer.IsNull())
    return 0;

  // do not create a new instance of the aspect
  switch (theRow)
  {
    case 1: theName = "UIsoAspect"; theOwnAspect = aDrawer->HasOwnUIsoAspect(); return aDrawer->UIsoAspect();
    case 2: theName = "VIsoAspect"; theOwnAspect = aDrawer->HasOwnVIsoAspect(); return aDrawer->VIsoAspect();
    case 3: theName = "WireAspect"; theOwnAspect = aDrawer->HasOwnWireAspect(); return aDrawer->WireAspect();
    case 4: theName = "PointAspect"; theOwnAspect = aDrawer->HasOwnPointAspect(); return aDrawer->PointAspect();
    case 5: theName = "LineAspect"; theOwnAspect = aDrawer->HasOwnLineAspect(); return aDrawer->LineAspect();
    case 6: theName = "TextAspect"; theOwnAspect = aDrawer->HasOwnTextAspect(); return aDrawer->TextAspect();
    case 7: theName = "ShadingAspect"; theOwnAspect = aDrawer->HasOwnShadingAspect(); return aDrawer->ShadingAspect();
    case 8: theName = "PlaneAspect"; theOwnAspect = aDrawer->HasOwnPlaneAspect(); return aDrawer->PlaneAspect();
    case 9: theName = "SeenLineAspect"; theOwnAspect = aDrawer->HasOwnSeenLineAspect(); return aDrawer->SeenLineAspect();
    case 10: theName = "ArrowAspect"; theOwnAspect = aDrawer->HasOwnArrowAspect(); return aDrawer->ArrowAspect();
    case 11: theName = "HiddenLineAspect"; theOwnAspect = aDrawer->HasOwnHiddenLineAspect(); return aDrawer->HiddenLineAspect();
    case 12: theName = "VectorAspect"; theOwnAspect = aDrawer->HasOwnVectorAspect(); return aDrawer->VectorAspect();
    case 13: theName = "DatumAspect"; theOwnAspect = aDrawer->HasOwnDatumAspect(); return aDrawer->DatumAspect();
    case 14: theName = "SectionAspect"; theOwnAspect = aDrawer->HasOwnSectionAspect(); return aDrawer->SectionAspect();
    case 15: theName = "FreeBoundaryAspect"; theOwnAspect = aDrawer->HasOwnFreeBoundaryAspect(); return aDrawer->FreeBoundaryAspect();
    case 16: theName = "UnFreeBoundaryAspect"; theOwnAspect = aDrawer->HasOwnUnFreeBoundaryAspect(); return aDrawer->UnFreeBoundaryAspect();
    case 17: theName = "FaceBoundaryAspect"; theOwnAspect = aDrawer->HasOwnFaceBoundaryAspect(); return aDrawer->FaceBoundaryAspect();
    case 18: theName = "DimensionAspect"; theOwnAspect = aDrawer->HasOwnDimensionAspect(); return aDrawer->DimensionAspect();
    default: break;
  }

  theName = "None";
  return Handle(Prs3d_BasicAspect)();
}


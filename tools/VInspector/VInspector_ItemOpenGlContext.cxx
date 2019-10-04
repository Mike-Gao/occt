// Created on: 2019-03-15
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

#include <inspector/VInspector_ItemOpenGlContext.hxx>

//#include <inspector/VInspector_ItemOpenGlContextList.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <OpenGl_Layer.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Text.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemOpenGlContext::Init()
{
  //VInspector_ItemOpenGlContextListPtr aParentItem = itemDynamicCast<VInspector_ItemOpenGlContextList>(Parent());
  //myLayer = aParentItem->GetLayer (Row(), myLayerId);

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemOpenGlContext::Reset()
{
  VInspector_ItemBase::Reset();
  myLayer = NULL;
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemOpenGlContext::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemOpenGlContext*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemOpenGlContext::initRowCount() const
{
  if (Column() != 0)
    return 0;

  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemOpenGlContext::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(OpenGl_Layer) aLayer = GetLayer();
  if (aLayer.IsNull())
    return Column() == 0 ? "Empty element" : "";

  switch (Column())
  {
    case 0:
    {
      TCollection_AsciiString aLayerId (myLayerId);
      return theItemRole == Qt::ToolTipRole ? QVariant ("")
                                            : QVariant (QString("%1 (%2)")
                                            .arg(aLayer->DynamicType()->Name())
                                            .arg (aLayerId.ToCString()));
    }
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemOpenGlContext::createChild (int theRow, int theColumn)
{
  (void)theRow;
  (void)theColumn;
  return TreeModel_ItemBasePtr();
}

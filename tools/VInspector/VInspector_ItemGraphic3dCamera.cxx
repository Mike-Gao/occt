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

#include <inspector/VInspector_ItemGraphic3dCamera.hxx>

#include <inspector/VInspector_ItemGraphic3dCView.hxx>
#include <inspector/VInspector_ItemV3dView.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <inspector/ViewControl_Table.hxx>
#include <inspector/ViewControl_Tools.hxx>

#include <AIS.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Graphic3d_Camera.hxx>
#include <TopoDS_Compound.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCamera::Init()
{
  VInspector_ItemV3dViewPtr aParentItem = itemDynamicCast<VInspector_ItemV3dView>(Parent());
  if (aParentItem)
    myCamera = aParentItem->GetView()->DefaultCamera();
  else
  {
    VInspector_ItemGraphic3dCViewPtr aParentItem = itemDynamicCast<VInspector_ItemGraphic3dCView>(Parent());
    if (aParentItem)
      myCamera = aParentItem->GetCView()->Camera();
  }

  UpdatePresentationShape();
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCamera::Reset()
{
  VInspector_ItemBase::Reset();
  myCamera = NULL;
}

// =======================================================================
// function : GetStream
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCamera::GetStream (Standard_OStream& theOStream) const
{
  Handle(Graphic3d_Camera) aCamera = GetCamera();
  if (aCamera.IsNull())
    return;

  aCamera->DumpJson (theOStream);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemGraphic3dCamera::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemGraphic3dCamera*>(this)->Init();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemGraphic3dCamera::initRowCount() const
{
  Handle(Graphic3d_Camera) aCamera = GetCamera(); // we need Init() call for having GetStream() filled

  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemGraphic3dCamera::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  Handle(Graphic3d_Camera) aCamera = GetCamera();
  if (aCamera.IsNull())
    return Column() == 0 ? "Empty Graphic3d_Camera" : "";

  switch (Column())
  {
    case 0: return theItemRole == Qt::DisplayRole ? aCamera->DynamicType()->Name()
                                                  : STANDARD_TYPE (Graphic3d_Camera)->Name();
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemGraphic3dCamera::buildPresentationShape (const Handle(Graphic3d_Camera)& theCamera)
{
  if (theCamera.IsNull())
    return TopoDS_Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  const gp_Pnt& aCenter = theCamera->Center ();
  const gp_Pnt& anEye = theCamera->Eye ();
  const gp_Dir& anUp = theCamera->Up();
  const gp_Dir& aCameraDirection = theCamera->Direction();

  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (aCenter));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeVertex (anEye));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Lin (anEye, anUp)));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Lin (anEye, aCameraDirection)));

  return aCompound;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemGraphic3dCamera::createChild (int, int)
{
  return TreeModel_ItemBasePtr();
}

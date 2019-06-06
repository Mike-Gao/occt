// Created on: 2019-04-29
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

#include <inspector/VInspector_ItemBVHTreeNode.hxx>

#include <inspector/VInspector_ItemBVHTree.hxx>
//#include <inspector/VInspector_ItemSelectMgrBaseFrustum.hxx>
//
#include <inspector/VInspector_Tools.hxx>

#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemBVHTreeNode::initRowCount() const
{
  return 0;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemBVHTreeNode::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  switch (Column())
  {
    case 0: return QString ("TreeNode_%1").arg (Row()); break;
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemBVHTreeNode::Init()
{
  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemBVHTreeNode::Reset()
{
  VInspector_ItemBase::Reset();
}

// =======================================================================
// function : GetTree
// purpose :
// =======================================================================

opencascade::handle<BVH_Tree<Standard_Real, 3> > VInspector_ItemBVHTreeNode::GetTree() const
{
  VInspector_ItemBVHTreePtr anObjectParent = itemDynamicCast<VInspector_ItemBVHTree>(Parent());

  return anObjectParent->GetTree();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemBVHTreeNode::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemBVHTreeNode*>(this)->Init();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemBVHTreeNode::createChild (int, int)
{
  return TreeModel_ItemBasePtr();
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemBVHTreeNode::buildPresentationShape()
{
  opencascade::handle<BVH_Tree<Standard_Real, 3> > aBVHTree = GetTree();
  if (aBVHTree.IsNull())
    return TopoDS_Shape();

  Standard_SStream OS;
  aBVHTree->DumpNode (Row(), OS);

  Standard_Integer aColumnCount;
  NCollection_Vector<TCollection_AsciiString> aValues;
  Message::ConvertStream (OS, aColumnCount, aValues);

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);
  for (int aValueId = 0; aValueId < aValues.Size(); )
  {
    for (int aColumnId = 0; aColumnId < aColumnCount; aColumnId++, aValueId++)
    {
      if (aColumnId != 1)
        continue;

      TCollection_AsciiString aValue = aValues.Value (aValueId);
      Bnd_Box aBox;
      if (!aBox.FromString (aValue))
        continue;

      TopoDS_Shape aShape = VInspector_Tools::CreateShape (aBox);
      aBuilder.Add (aCompound, aShape);
    }
  }
  return aCompound;
}

// =======================================================================
// function : Dump
// purpose :
// =======================================================================
Standard_Boolean VInspector_ItemBVHTreeNode::Dump (Standard_OStream& OS) const
{
  opencascade::handle<BVH_Tree<Standard_Real, 3> > aBVHTree = GetTree();
  if (aBVHTree.IsNull())
    return Standard_False;

  aBVHTree->DumpNode (Row(), OS);
  return Standard_True;
}

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

#include <inspector/VInspector_ItemBVHTree.hxx>

#include <inspector/VInspector_ItemSelectMgrSelectableObjectSet.hxx>
#include <inspector/VInspector_ItemSelectMgrSensitiveEntitySet.hxx>
#include <inspector/VInspector_ItemBVHTreeNode.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <inspector/Convert_Tools.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ItemBVHTree::VInspector_ItemBVHTree (TreeModel_ItemBasePtr theParent,
                                                const int theRow, const int theColumn)
: VInspector_ItemBase(theParent, theRow, theColumn)
{
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemBVHTree::initRowCount() const
{
  if (Column() != 0)
    return  0;

  opencascade::handle<BVH_Tree<Standard_Real, 3> > aTree =  GetTree();
  if (aTree.IsNull())
    return 0;

  return aTree->Length();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemBVHTree::initValue (const int theItemRole) const
{
  QVariant aParentValue = VInspector_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  if (GetTree().IsNull())
    return Column() == 0 ? "Empty BVH tree" : "";

  switch (Column())
  {
    case 0:
    {
      TCollection_AsciiString aName = TCollection_AsciiString (GetTree()->DynamicType()->Name()) +
                                      TCollection_AsciiString (" (") + myName.ToCString() + ")";
      return aName.ToCString();
    }
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================

void VInspector_ItemBVHTree::Init()
{
  VInspector_ItemSelectMgrSelectableObjectSetPtr anObjectParent = itemDynamicCast<VInspector_ItemSelectMgrSelectableObjectSet>(Parent());
  opencascade::handle<BVH_Tree<Standard_Real, 3> > aBVHTree;
  if (anObjectParent)
  {
    aBVHTree = anObjectParent->GetBVHTree (Row(), myName);
  }
  else
  {
    VInspector_ItemSelectMgrSensitiveEntitySetPtr anEntityParent = itemDynamicCast<VInspector_ItemSelectMgrSensitiveEntitySet>(Parent());
    if (anEntityParent)
      aBVHTree = anEntityParent->GetBVHTree (Row(), myName);
  }

  setTree (aBVHTree);

  UpdatePresentationShape();
  TreeModel_ItemBase::Init(); // to use getIO() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================

void VInspector_ItemBVHTree::Reset()
{
  VInspector_ItemBase::Reset();

  setTree (NULL);
  myName = TCollection_AsciiString();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================

void VInspector_ItemBVHTree::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemBVHTree*>(this)->Init();
}

// =======================================================================
// function : Dump
// purpose :
// =======================================================================
Standard_Boolean VInspector_ItemBVHTree::Dump (Standard_OStream& theOStream) const
{
  opencascade::handle<BVH_Tree<Standard_Real, 3> > aBVHTree = GetTree();
  if (aBVHTree.IsNull())
    return Standard_False;

  aBVHTree->DumpJson (theOStream);
  return Standard_True;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemBVHTree::createChild (int theRow, int theColumn)
{
  return VInspector_ItemBVHTreeNode::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : buildPresentationShape
// purpose :
// =======================================================================
TopoDS_Shape VInspector_ItemBVHTree::buildPresentationShape()
{
  return TopoDS_Shape();
  /*opencascade::handle<BVH_Tree<Standard_Real, 3> > aBVHTree = myTree;
  if (aBVHTree.IsNull())
    return TopoDS_Shape();

  Standard_SStream OS;
  //aBVHTree->DumpJsonNode (Row(), OS);
  aBVHTree->DumpJson (OS);

  Standard_Integer aColumnCount;
  NCollection_Vector<TCollection_AsciiString> aValues;
  Message::ConvertStream (theOStream, aColumnCount, aValues);

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
      if (!aBox.Init (Standard_SStream (aValue.ToCString())))
        continue;

      TopoDS_Shape aShape = Convert_Tools::CreateShape (aBox);
      aBuilder.Add (aCompound, aShape);
    }
  }
  return aCompound;*/
}

// Created on: 2019-02-25
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

#include <inspector/TreeModel_ItemStream.hxx>

#include <inspector/TreeModel_ItemProperties.hxx>

#include <Standard_Dump.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TreeModel_ItemStream::TreeModel_ItemStream (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
 : TreeModel_ItemBase (theParent, theRow, theColumn)
{
  //myProperties = new TreeModel_ItemProperties();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void TreeModel_ItemStream::Init()
{
  //if (!GetProperties()->Item())
  //  myProperties->SetItem (currentItem());

  //TCollection_AsciiString aKey, aKeyValue, aPropertiesValue;
  //TreeModel_ItemStreamPtr aStreamParent = itemDynamicCast<TreeModel_ItemStream>(Parent());
  //if (!aStreamParent)
  //{
  //  Standard_SStream aStream;
  //  Parent()->Stream (aStream);

  //  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aValues;
  //  Standard_Dump::SplitJson (Standard_Dump::Text (aStream), aValues);

  //  aKey = aValues.FindKey (Row() + 1);
  //  aKeyValue = aValues.FindFromIndex (Row() + 1);

  //  // one row value, like gp_XYZ, without additional { for type
  //  aValues.Clear();
  //  if (!Standard_Dump::SplitJson (aKeyValue, aValues))
  //    aPropertiesValue = Standard_Dump::Text (aStream);
  //  else
  //    aPropertiesValue = aKeyValue;
  //}
  //else
  //{
  //  TreeModel_ItemStreamPtr aStreamParent = itemDynamicCast<TreeModel_ItemStream>(Parent());

  //  TCollection_AsciiString aValue;
  //  aStreamParent->GetChildStream (Row(), aKey, aKeyValue);
  //  aPropertiesValue = aKeyValue;
  //}

  //myKey = aKey;
  ////myProperties->Init (aPropertiesValue);
  //myStreamValue = aKeyValue;

  //NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aValues;
  //Standard_Dump::SplitJson (aKeyValue, aValues);

  //for (Standard_Integer anIndex = 1; anIndex <= aValues.Size(); anIndex++)
  //{
  //  TCollection_AsciiString aValue = aValues.FindFromIndex (anIndex);
  //  if (Standard_Dump::HasChildKey (aValue))
  //    myChildren.Add (aValues.FindKey (anIndex), aValue);
  //}

  TreeModel_ItemBase::Init();

  //NCollection_List<Standard_Integer> aHierarchicalValues;
  int aStreamChildrenCount = 0;
  if (Column() == 0)
  {
    if (!myProperties)
    {
      myProperties = new TreeModel_ItemProperties();
      myProperties->SetItem (currentItem());
    }
    myProperties->Init();
    aStreamChildrenCount = myProperties->Children().Extent();
  }
  m_iStreamChildren = aStreamChildrenCount;//aHierarchicalValues.Extent();
  initStream (myStream);
}

// =======================================================================
// function : Rest
// purpose :
// =======================================================================
void TreeModel_ItemStream::Reset()
{
  //myKey = "";
  //myStreamValue = "";

  //myChildren.Clear();

  myStream.str ("");
  TreeModel_ItemBase::Reset();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant TreeModel_ItemStream::initValue (const int theItemRole) const
{
  QVariant aParentValue = TreeModel_ItemBase::initValue (theItemRole);
  if (aParentValue.isValid())
    return aParentValue;

  if (Column() != 0)
    return QVariant();

  if (theItemRole == Qt::ForegroundRole)
    return QColor (Qt::darkBlue);

  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::EditRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  switch (Column())
  {
    case 0: return Properties() ? Properties()->GetKey().ToCString() : "";
  }
  return QVariant();
}

//// =======================================================================
//// function : GetChildStream
//// purpose :
//// =======================================================================
//void TreeModel_ItemStream::GetChildStream (const int theRowId,
//                                           TCollection_AsciiString& theKey,
//                                           TCollection_AsciiString& theValue) const
//{
//  TreeModel_ItemBasePtr aParentItem = Parent();
//  if (!aParentItem)
//    return;
//
//  theKey = myChildren.FindKey (theRowId + 1);
//  theValue = myChildren.FindFromIndex (theRowId + 1);
//}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void TreeModel_ItemStream::initStream (Standard_OStream& theOStream) const
{
  if (!Properties())
    return;
  //Handle(AIS_InteractiveObject) anIO = GetInteractiveObject();
  //if (anIO.IsNull())
  //  return;

  theOStream << Properties()->StreamValue();
  //anIO->DumpJson (theOStream);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void TreeModel_ItemStream::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<TreeModel_ItemStream*>(this)->Init();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr TreeModel_ItemStream::createChild (int theRow, int theColumn)
{
  return TreeModel_ItemStream::CreateItem (currentItem(), theRow, theColumn);
}

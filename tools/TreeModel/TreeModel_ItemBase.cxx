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

#include <inspector/TreeModel_ItemBase.hxx>
#include <inspector/TreeModel_ItemRole.hxx>
#include <inspector/TreeModel_ItemStream.hxx>

#include <Standard_Dump.hxx>
#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
TreeModel_ItemBase::TreeModel_ItemBase (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
 : m_bInitialized (false), m_iStreamChildren (0)
{
  m_pParent = theParent;
  m_iRow = theRow;
  m_iColumn = theColumn;
}

// =======================================================================
// function :  Reset
// purpose :
// =======================================================================
void TreeModel_ItemBase::Reset()
{
  for (PositionToItemHash::const_iterator aChildrenIt = m_ChildItems.begin(); aChildrenIt != m_ChildItems.end(); aChildrenIt++)
  {
    TreeModel_ItemBasePtr anItem = aChildrenIt.value();
    if (anItem)
      anItem->Reset();
  }
  m_bInitialized = false;
  myCachedValues.clear();
}

// =======================================================================
// function :  Reset
// purpose :
// =======================================================================
void TreeModel_ItemBase::Reset (int theRole)
{
  if (!myCachedValues.contains (theRole))
    return;

  myCachedValues.remove (theRole);
}

// =======================================================================
// function :  child
// purpose :
// =======================================================================
TreeModel_ItemBasePtr TreeModel_ItemBase::Child (int theRow, int theColumn, const bool isToCreate)
{
  QPair<int, int> aPos = qMakePair (theRow, theColumn);

  if (m_ChildItems.contains (aPos))
    return m_ChildItems[aPos];

  TreeModel_ItemBasePtr anItem;
  if (isToCreate) {
    if (theRow < m_iStreamChildren)
      anItem = TreeModel_ItemStream::CreateItem (currentItem(), theRow, theColumn);
    else
      anItem = createChild (theRow - m_iStreamChildren, theColumn);

    if (anItem)
      m_ChildItems[aPos] = anItem;
  }
  return anItem;
}

// =======================================================================
// function :  currentItem
// purpose :
// =======================================================================
const TreeModel_ItemBasePtr TreeModel_ItemBase::currentItem()
{
  return TreeModel_ItemBasePtr (this);
}

// =======================================================================
// function :  cachedValue
// purpose :
// =======================================================================
QVariant TreeModel_ItemBase::cachedValue (const int theItemRole) const
{
  if (myCachedValues.contains (theItemRole))
    return myCachedValues[theItemRole];

  QVariant aValueToCache;
  if (theItemRole == TreeModel_ItemRole_RowCountRole)
    aValueToCache = initRowCount() + m_iStreamChildren;
  else
    aValueToCache = initValue (theItemRole);

  const_cast<TreeModel_ItemBase*>(this)->myCachedValues.insert (theItemRole, aValueToCache);
  return myCachedValues.contains (theItemRole) ? myCachedValues[theItemRole] : QVariant();
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void TreeModel_ItemBase::Init()
{
  m_bInitialized = true;

  NCollection_List<Standard_Integer> aHierarchicalValues;
  if (Column() == 0)
  {
    NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aValues;
    Standard_SStream aStream;
    GetStream (aStream);
    Standard_Dump::SplitJson (Standard_Dump::Text (aStream), aValues);
    aHierarchicalValues = Standard_Dump::HierarchicalValueIndices (aValues);

    //if (aHierarchicalValues.Size() == 1)
  }
  m_iStreamChildren = aHierarchicalValues.Extent();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant TreeModel_ItemBase::initValue (const int theItemRole) const
{
  if (theItemRole != Qt::DisplayRole && theItemRole != Qt::ToolTipRole)
    return QVariant();

  switch (Column())
  {
    case 1: { return rowCount(); }
    //case 2: return ViewControl_Tools::GetPointerInfo (GetObject(), true).ToCString();
    case 3: { return Row(); }
  }

  return QVariant();
}

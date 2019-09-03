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

#include <inspector/TreeModel_ItemProperties.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TreeModel_ItemProperties, Standard_Transient)

// =======================================================================
// function :  Reset
// purpose :
// =======================================================================
void TreeModel_ItemProperties::Reset()
{
  m_bInitialized = false;
  myCachedValues.clear();

  myCachedDimValues.clear();
  myCachedPresentations.clear();
}

// =======================================================================
// function : GetTableFlags
// purpose :
// =======================================================================

Qt::ItemFlags TreeModel_ItemProperties::GetTableFlags (const int, const int theColumn) const
{
  Qt::ItemFlags aFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (theColumn == 1)
    aFlags = aFlags | Qt::ItemIsEditable;

  return aFlags;
}

// =======================================================================
// function : cachedValueRowCount
// purpose :
// =======================================================================

int TreeModel_ItemProperties::cachedDimValue (const TreeModel_DimType theDimType) const
{
  if (myCachedDimValues.contains (theDimType))
    return myCachedDimValues[theDimType].toInt();

  QVariant aValueToCache;
  switch (theDimType)
  {
    case TreeModel_DimType_Rows:    aValueToCache = GetTableRowCount(); break;
    case TreeModel_DimType_Columns: aValueToCache = GetTableColumnCount(); break;
  }

  const_cast<TreeModel_ItemProperties*>(this)->myCachedDimValues.insert (theDimType, aValueToCache);
  return myCachedDimValues.contains (theDimType) ? myCachedDimValues[theDimType].toInt() : 0;
}

// =======================================================================
// function : cachedValue
// purpose :
// =======================================================================

QVariant TreeModel_ItemProperties::cachedValue (const int theRow, const int theColumn, int theRole) const
{
  QPair<int, int> aPos = qMakePair (theRow, theColumn);
  if (myCachedValues.contains (aPos) && myCachedValues[aPos].contains (theRole))
    return myCachedValues[aPos][theRole];

  QVariant aValueToCache = GetTableData (theRow, theColumn, theRole);

  QMap<int, QVariant> aValuesToCache;
  if (myCachedValues.contains(aPos))
    aValuesToCache = myCachedValues[aPos];
  aValuesToCache.insert (theRole, aValueToCache);

  const_cast<TreeModel_ItemProperties*>(this)->myCachedValues.insert (aPos, aValuesToCache);
  return aValueToCache;
}

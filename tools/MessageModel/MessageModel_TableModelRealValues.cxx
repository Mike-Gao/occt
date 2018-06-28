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

#include <inspector/MessageModel_TableModelRealValues.hxx>

#include <inspector/ViewControl_TableModel.hxx>
#include <inspector/ViewControl_TableModelFilter.hxx>

const int REAL_SIGNS = 16;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================

MessageModel_TableModelRealValues::MessageModel_TableModelRealValues (const Handle(Message_Attribute)& theAttribute,
  const int theSectionWidth)
 : myAttribute (Handle(Message_AttributeVectorOfReal)::DownCast (theAttribute)), myFilter(0)
{
  if (myAttribute->GetColumnCount() > 0)
    myFilter = new ViewControl_TableModelFilter (myAttribute->GetColumnCount());

  SetUseTableSeparateSize (!myFilter);

  SetUseTableProperties (true);
  SetUseTablePropertiesXStep (true, -1);

  SetDefaultSectionSize (Qt::Horizontal, theSectionWidth);
}

// =======================================================================
// function : ColumnCount
// purpose :
// =======================================================================

int MessageModel_TableModelRealValues::ColumnCount (const QModelIndex&) const
{
  int aColumnCount = myAttribute->GetValues().Length();
  if (myFilter)
    aColumnCount = myFilter->ColumnCount (aColumnCount);

  return aColumnCount;
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant MessageModel_TableModelRealValues::Data (const int theRow, const int theColumn, int theRole) const
{
  if (theRole != Qt::DisplayRole || theColumn > myAttribute->GetValues().Length())
  return QVariant();

  int aRow = theRow, aColumn = theColumn;
  if (myFilter)
    myFilter->GetSourcePosition (theRow, theColumn, aRow, aColumn);

  return myAttribute->GetValues().Value(aColumn);
}

// =======================================================================
// function : GetRangeValues
// purpose :
// =======================================================================

void MessageModel_TableModelRealValues::GetRangeValues (QString& theMinValue, QString& theMaxValue, const QModelIndexList& theSelected) const
{
  double aMinValue, aMaxValue;
  aMinValue = DBL_MAX;
  aMaxValue = DBL_MIN;

  NCollection_Vector<double> aValues = myAttribute->GetValues();
  int aValuesSize = aValues.Length();
  if (aValuesSize < 1)
    return;

  if (theSelected.isEmpty())
  {
    aMinValue = aMaxValue = aValues.First();
    for (int i = 1; i < aValuesSize; i++)
    {
      double aValue = aValues.Value (i);
      if (aValue < aMinValue) aMinValue = aValue;
      if (aValue > aMaxValue) aMaxValue = aValue;
    }
  }
  else
  {
    ViewControl_TableModel* aModel = (ViewControl_TableModel*)theSelected[0].model();
    int aRow, aColumn;
    for (QModelIndexList::const_iterator aSelIt = theSelected.begin(); aSelIt != theSelected.end(); aSelIt++)
    {
      if (myFilter)
        myFilter->GetSourcePosition (*aSelIt, aRow, aColumn);
      else
        aModel->GetSourcePosition (*aSelIt, aRow, aColumn);
      if (aColumn >= aValuesSize)
        continue;

      double aValue = aValues.Value (aColumn);
      if (aValue < aMinValue) aMinValue = aValue;
      if (aValue > aMaxValue) aMaxValue = aValue;
    }
  }

  theMinValue = QString::number(aMinValue, 'g', REAL_SIGNS);
  theMaxValue = QString::number(aMaxValue, 'g', REAL_SIGNS);
}

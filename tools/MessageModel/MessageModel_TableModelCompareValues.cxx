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

#include <inspector/MessageModel_TableModelCompareValues.hxx>

#include <inspector/ViewControl_TableModel.hxx>

#include <Precision.hxx>

const int REAL_SIGNS = 16;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================

MessageModel_TableModelCompareValues::MessageModel_TableModelCompareValues (ViewControl_TableModelValues* theValues1,
  ViewControl_TableModelValues* theValues2)
 : myValues1 (theValues1), myValues2 (theValues2)
{
  SetUseTableSeparateSize (theValues1->UseTableSeparateSize());
  SetUseTableProperties (false);

  Standard_Integer aSectionSize;
  theValues1->GetDefaultSectionSize(Qt::Horizontal, aSectionSize);
  SetDefaultSectionSize (Qt::Horizontal, aSectionSize);
}

// =======================================================================
// function : Destructor
// purpose :
// =======================================================================

MessageModel_TableModelCompareValues::~MessageModel_TableModelCompareValues()
{
  if (myValues1)
    delete myValues1;

  if (myValues2)
    delete myValues2;
}

// =======================================================================
// function : ColumnCount
// purpose :
// =======================================================================

int MessageModel_TableModelCompareValues::ColumnCount (const QModelIndex& theIndex) const
{
  int aColumnCount1 = myValues1->ColumnCount (theIndex);
  return aColumnCount1 == myValues2->ColumnCount (theIndex) ? aColumnCount1 : 0;
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================

QVariant MessageModel_TableModelCompareValues::Data (const int theRow, const int theColumn, int theRole) const
{
  QVariant aValue1 = myValues1->Data (theRow, theColumn, Qt::DisplayRole);
  QVariant aValue2 = myValues2->Data (theRow, theColumn, Qt::DisplayRole);

  QVariant aToolTipValue;
  QVariant aDifference;
  if (aValue1 != aValue2) {
    bool isOk1, isOk2;
    double aDoubleValue_1 = aValue1.toDouble(&isOk1);
    double aDoubleValue_2 = aValue2.toDouble(&isOk2);

    if (isOk1 && isOk2)
    {
      double aDoubleDifference = fabs (aDoubleValue_1 - aDoubleValue_2);
      if (aDoubleDifference > Precision::Confusion())
      {
        aDifference = aDoubleDifference;
        aToolTipValue = QString ("%1 (first value)\n%2 (second value)\n%3 (difference: first-second)")
                          .arg (QString::number (aDoubleValue_1, 'g', REAL_SIGNS))
                          .arg (QString::number (aDoubleValue_2, 'g', REAL_SIGNS))
                          .arg (aDifference.toString());
      }
    }
    else {
      aDifference = QString ("%1 / %2").arg (aValue1.toString()).arg (aValue2.toString());
      aToolTipValue = QString ("(first value) / (second value)");
    }
  }

  switch (theRole)
  {
    case Qt::DisplayRole: return aDifference;
    case Qt::ToolTipRole: return aToolTipValue;
    case Qt::BackgroundRole: return !aDifference.isNull() ? QVariant (QColor (200, 191, 231)) : QVariant();
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : GetRangeValues
// purpose :
// =======================================================================

void MessageModel_TableModelCompareValues::GetRangeValues (QString& theMinValue, QString& theMaxValue,
  const QModelIndexList& theSelected) const
{
  (void)theMinValue; (void)theMaxValue; (void)theSelected;
}

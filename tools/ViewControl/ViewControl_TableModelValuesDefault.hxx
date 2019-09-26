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

#ifndef ViewControl_TableModelValuesDefault_H
#define ViewControl_TableModelValuesDefault_H

#include <Standard.hxx>

#include <inspector/ViewControl_TableModelValues.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <QColor>
#include <QList>
#include <QModelIndexList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class QItemDelegate;

//! \class ViewControl_TableModelValuesDefault
//! \brief This is an interace for ViewControl_TableModel to give real values of the model
//! It should be filled or redefined.
class ViewControl_TableModelValuesDefault : public ViewControl_TableModelValues
{
public:

  //! Constructor
  Standard_EXPORT ViewControl_TableModelValuesDefault (const Qt::Orientation& theOrientation = Qt::Vertical)
    : ViewControl_TableModelValues (theOrientation) {}

  //! Destructor
  virtual ~ViewControl_TableModelValuesDefault() {}

  //! Fills the model values.
  //! \param theValues a container of table model values
  void SetValues (const QVector<QVariant>& theValues) { myValues = theValues; }

  //! Returns number of columns, size of header values
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int ColumnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns number of rows, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  virtual int RowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { return ColumnCount (theParent) > 0 ? GetValuesCount() / ColumnCount (theParent) : 0; }

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It returns value only for DisplayRole.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant Data (const int theRow, const int theColumn, int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns number of rows, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  virtual int GetValuesCount () const { return myValues.size(); }

protected:
  //! Finds position in internal vector of values using the table column/row count
  //! \param theRow a row of a table cell
  //! \param theColumn a column of a table cell
  size_t getPosition (const int theRow, const int theColumn) const { return ColumnCount() * theRow + theColumn; }

  //! Returns true if the header item is italic of the parameter index
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param boolean value
  bool isItalicHeader (const int theRow, const int theColumn) const;

protected:

  QVector<QVariant> myValues; //! cached container of table values
};

#endif

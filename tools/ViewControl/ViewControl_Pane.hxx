// Created on: 2018-12-08
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

#ifndef ViewControl_Pane_H
#define ViewControl_Pane_H

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>

#include <inspector/ViewControl_EditType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class ViewControl_PaneItem;

//! \class ViewControl_PaneItem
//! Pane for getting/setting pane table values
//!
class ViewControl_Pane
{
public:

  //! Returns number of table rows
  //! \param theObject current pane object
  //! \return an integer value
  virtual int GetTableRowCount (const Handle(Standard_Transient)& theObject) const = 0;

  //! Returns table value for the row in form: <function name> <function value>
  //! \param theObject current pane object
  //! \param theRow a model index row
  //! \param theColumn a model index column
  virtual QVariant GetTableData (const Handle(Standard_Transient)& theObject,
                                 const int theRow, const int theColumn, const int theRole) const = 0;

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theObject current pane object
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  virtual ViewControl_EditType GetTableEditType (const Handle(Standard_Transient)& theObject,
                                                 const int theRow, const int theColumn) const
    { (void)theObject; (void)theRow; (void)theColumn; return ViewControl_EditType_None; }

  //! Returns container of string values for enumeration in the model row
  //! \param theObject current pane object
  //! \param theRow table model row index
  //! \param theColumn a model index column
  //! \return string values for the enumeration presented in the row or an empty container
  virtual QList<QVariant> GetTableEnumValues (const Handle(Standard_Transient)& theObject,
                                              const int theRow, const int theColumn) const
    { (void)theObject; (void)theRow; (void)theColumn; return QList<QVariant>(); }

  //! Sets the value into the table cell. Only 1st column value might be modified.
  //! \param theObject current pane object
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theValue a new cell value
  virtual bool SetTableData (const Handle(Standard_Transient)& theObject,
                             const int theRow, const int theColumn, const QVariant& theValue)
    { (void)theObject; (void)theRow; (void)theColumn; (void)theValue; return false; }

  //! Return selected element in the pane cell
  //! \param theObject current pane object
  //! \param theRow a model index row
  //! \param theColumn a model index column
  virtual ViewControl_PaneItem* GetSelected (const Handle(Standard_Transient)& theObject,
                                             const int theRow, const int theColumn)
  { (void)theObject; (void)theRow; (void)theColumn; return 0; }
};

#endif
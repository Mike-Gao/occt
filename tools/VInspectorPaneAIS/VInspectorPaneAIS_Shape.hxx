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

#ifndef VInspectorPaneAIS_Shape_H
#define VInspectorPaneAIS_Shape_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <inspector/ViewControl_Pane.hxx>

#include <AIS_Shape.hxx>

//! \class VInspectorPaneAIS_Shape
//! Item presents information about AIS_InteractiveObject.
//! Parent is item context, children are item selections.
class VInspectorPaneAIS_Shape : public ViewControl_Pane
{
public:
  //! Constructor
  VInspectorPaneAIS_Shape() : ViewControl_Pane() {}

  //! Destructor
  ~VInspectorPaneAIS_Shape() {}

  //! Returns number of table rows
  //! \return an integer value
  Standard_EXPORT virtual int GetTableRowCount(const Handle(Standard_Transient)& theObject) const Standard_OVERRIDE;

  //! Returns table value for the row in form: <function name> <function value>
  //! \param theRow a model index row
  //! \param theColumn a model index column
  Standard_EXPORT virtual QVariant GetTableData (const Handle(Standard_Transient)& theObject,
    const int theRow, const int theColumn, const int theRole) const Standard_OVERRIDE;

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  Standard_EXPORT virtual ViewControl_EditType GetTableEditType (const Handle(Standard_Transient)& theObject,
    const int theRow, const int theColumn) const Standard_OVERRIDE;

  //! Returns container of string values for enumeration in the model row
  //! \param theRow table model row index
  //! \param theColumn a model index column
  //! \return string values for the enumeration presented in the row or an empty container
  Standard_EXPORT virtual QList<QVariant> GetTableEnumValues (const Handle(Standard_Transient)& theObject,
    const int theRow, const int theColumn) const Standard_OVERRIDE;

  //! Sets the value into the table cell. Only 1st column value might be modified.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theValue a new cell value
  Standard_EXPORT virtual bool SetTableData (const Handle(Standard_Transient)& theObject,
    const int theRow, const int theColumn, const QVariant& theValue) Standard_OVERRIDE;

  //! Return selected element in the pane cell
  //! \param theObject current pane object
  //! \param theRow a model index row
  //! \param theColumn a model index column
  Standard_EXPORT virtual ViewControl_PaneItem* GetSelected (const Handle(Standard_Transient)& theObject,
    const int theRow, const int theColumn) Standard_OVERRIDE;

protected:
  //! Converts transient object to custom presentation type
  //! \return custom presentation
  Handle(AIS_Shape) GetPresentation (const Handle(Standard_Transient)& theObject) const
    { return Handle(AIS_Shape)::DownCast (theObject); };
};

#endif

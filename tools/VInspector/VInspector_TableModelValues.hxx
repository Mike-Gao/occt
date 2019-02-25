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

#ifndef VInspector_TableModelValues_H
#define VInspector_TableModelValues_H

#include <inspector/ViewControl_TableModelValues.hxx>
#include <inspector/VInspector_ItemBase.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

class ViewControl_PaneCreator;

#include <NCollection_List.hxx>

//! \class VInspector_TableModelValues
//! \brief This is an implementation for ViewControl_TableModel to present tree item values
class VInspector_TableModelValues : public ViewControl_TableModelValues
{
public:

  //! Constructor
  Standard_EXPORT VInspector_TableModelValues (const TreeModel_ItemBasePtr& theItem,
    const NCollection_List<Handle(ViewControl_PaneCreator)>& theCreators);

  //! Destructor
  virtual ~VInspector_TableModelValues() Standard_OVERRIDE {}

  //! Returns number of columns. It has two columns: <funciton name> <function value>
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int ColumnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 2; }

  //! Returns number of rows, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int RowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It returns value only for DisplayRole.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant Data (const int theRow, const int theColumn, int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Sets content of the model index for the given role, it is applyed to internal container of values
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return true if the value is changed
  Standard_EXPORT virtual bool SetData (const int theRow, const int theColumn, const QVariant& theValue,
                                        int theRole = Qt::DisplayRole) Standard_OVERRIDE;

  //! Returns flags for the item: ItemIsEnabled | Qt::ItemIsSelectable.
  //! Additional flag for the column 1 is Qt::ItemIsEditable.
  //! \param theIndex a model index
  //! \return flags
  virtual Qt::ItemFlags Flags (const QModelIndex& theIndex) const Standard_OVERRIDE;

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  virtual ViewControl_EditType GetEditType (const int theRow, const int theColumn) const Standard_OVERRIDE;

  //! Returns container of string values for enumeration in the model row
  //! \param theRow table model row index
  //! \param theColumn a model index column
  //! \return string values for the enumeration presented in the row or an empty container
  virtual QList<QVariant> GetEnumValues (const int theRow, const int theColumn) const Standard_OVERRIDE;

  //! Returns container of pane shapes
  //! \param theRow table model row index
  //! \param theColumn a model index column
  //! \param [out] output container of shapes to add pane new shapes if found
  Standard_EXPORT void GetPaneShapes (const int theRow, const int theColumn, NCollection_List<TopoDS_Shape>& theShapes);

private:

  //!< Returns source item base
  VInspector_ItemBasePtr GetItem() const;

private:
  NCollection_List<Handle(ViewControl_PaneCreator)> myCreators; //!< pane creators
  TreeModel_ItemBasePtr myItem; //!< source item base
};

#endif

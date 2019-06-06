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

#ifndef TreeModel_ItemProperties_H
#define TreeModel_ItemProperties_H

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

#include <NCollection_List.hxx>

#include <inspector/TreeModel_ItemBase.hxx>
#include <inspector/ViewControl_EditType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <QColor>
#include <QList>
#include <QModelIndexList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

DEFINE_STANDARD_HANDLE (TreeModel_ItemProperties, Standard_Transient)

//! \class TreeModel_ItemProperties
//! Class to manipulate properties of tree item. The properties are organized in table structure
class TreeModel_ItemProperties : public Standard_Transient
{
public:
  //! Constructor
  TreeModel_ItemProperties (const TreeModel_ItemBasePtr& theItem) : myItem (theItem) {}

  //! Destructor
  ~TreeModel_ItemProperties() {}

  //! If me has internal values, it should be initialized here.
  virtual void Init() {}

  //! If the item has internal values, there should be reseted here.
  Standard_EXPORT virtual void Reset() {}

  //! Returns number of item children
  //! \return an integer value, ZERO by default
  virtual int ChildItemCount() const { return 0; }

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr CreateChildItem (int theRow, int theColumn) const
  { (void)theRow; (void)theColumn; return TreeModel_ItemBasePtr(); }

  //! Returns number of table columns. Default value is two columns: title to value
  //! \return an integer value
  virtual int GetTableColumnCount() const { return 2; }

  //! Returns number of table rows
  //! \return an integer value
  virtual int GetTableRowCount() const { return 0; }

  //! Returns table value for the row in form: <function name> <function value>
  //! \param theRow a model index row
  //! \param theColumn a model index column
  virtual QVariant GetTableData (const int theRow, const int theColumn, const int theRole) const
  {  (void)theRow; (void)theColumn; (void)theRole; return QVariant(); }

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  virtual ViewControl_EditType GetTableEditType (const int theRow, const int theColumn) const
    { (void)theRow; (void)theColumn; return ViewControl_EditType_None; }

  //! Returns container of string values for enumeration in the model row
  //! \param theRow table model row index
  //! \param theColumn a model index column
  //! \return string values for the enumeration presented in the row or an empty container
  virtual QList<QVariant> GetTableEnumValues (const int theRow, const int theColumn) const
    { (void)theRow; (void)theColumn; return QList<QVariant>(); }

  //! Sets the value into the table cell. Only 1st column value might be modified.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theValue a new cell value
  virtual bool SetTableData (const int theRow, const int theColumn, const QVariant& theValue)
    { (void)theRow; (void)theColumn; (void)theValue; return false; }

  //! Returns presentation of the attribute to be visualized in the view
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \thePresentations [out] container of presentation handles to be visualized
  virtual void GetPresentations (const int theRow, const int theColumn,
                                 NCollection_List<Handle(Standard_Transient)>& thePresentations)
  { (void)theRow; (void)theColumn; (void)thePresentations; }

  //! Returns flags for the item: ItemIsEnabled | Qt::ItemIsSelectable.
  //! Additional flag for the column 1 is Qt::ItemIsEditable.
  //! \param theIndex a model index
  //! \return flags
  Standard_EXPORT virtual Qt::ItemFlags GetTableFlags (const int theRow, const int theColumn) const;


  DEFINE_STANDARD_RTTIEXT (TreeModel_ItemProperties, Standard_Transient)

protected:
  //! Returns current item
  TreeModel_ItemBasePtr getItem() const { return myItem; }

private:
  TreeModel_ItemBasePtr myItem; //! current item
};

#endif
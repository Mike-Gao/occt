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

#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>

#include <TCollection_AsciiString.hxx>

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
  //! enum defined the dimension type
  enum TreeModel_DimType
  {
    TreeModel_DimType_Rows,   //! defines number of rows
    TreeModel_DimType_Columns //! defines number of columns
  };

public:
  //! Constructor
  TreeModel_ItemProperties() {}

  //! Destructor
  ~TreeModel_ItemProperties() {}

  //! Sets the current item
  void SetItem (const TreeModel_ItemBasePtr& theItem) { myItem = theItem; }

  //! Returns the current item
  TreeModel_ItemBasePtr Item() const { return myItem; }

  //! Fill internal containers by stream values
  void Init (const TCollection_AsciiString& theStreamValue);

  //! If the item has internal values, there should be reseted here.
  Standard_EXPORT virtual void Reset();

  //! Returns number of table rows. It uses cached value of GetTableRowCount(), Reset() to reinit it.
  //! \return an integer value
  Standard_EXPORT int RowCount() const;

  //! Returns number of table columns. It uses cached value of GetTableColumnCount(), Reset() to reinit it.
  //! \return an integer value
  Standard_EXPORT int ColumnCount() const { return 2; }

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It uses cached value of GetTableData(), Reset() to reinit it.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return value interpreted depending on the given role
  QVariant Data (const int theRow, const int theColumn, int theRole = Qt::DisplayRole) const;

  //! Returns presentation of the attribute to be visualized in the view
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \thePresentations [out] container of presentation handles to be visualized
  virtual void GetPresentations (const int theRow, const int theColumn,
                                 NCollection_List<Handle(Standard_Transient)>& thePresentations);

  //! Returns flags for the item: ItemIsEnabled | Qt::ItemIsSelectable.
  //! Additional flag for the column 1 is Qt::ItemIsEditable.
  //! \param theIndex a model index
  //! \return flags
  Standard_EXPORT virtual Qt::ItemFlags GetTableFlags (const int theRow, const int theColumn) const;

  DEFINE_STANDARD_RTTIEXT (TreeModel_ItemProperties, Standard_Transient)

private:
  TreeModel_ItemBasePtr myItem; //!< current item

  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> myValues; //!< the values
};

#endif
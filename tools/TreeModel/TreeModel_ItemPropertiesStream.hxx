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

#ifndef MessageModel_ItemReportProperties_H
#define MessageModel_ItemReportProperties_H

#include <Standard.hxx>
#include <Message_AttributeStream.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TCollection_AsciiString.hxx>


#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <QColor>
#include <QList>
#include <QModelIndexList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class QItemDelegate;

DEFINE_STANDARD_HANDLE (TreeModel_ItemPropertiesStream, TreeModel_ItemProperties)

//! \class TreeModel_ItemPropertiesStream
//! \brief This is an interace for ViewControl_TableModel to give real values of the model
//! It should be filled or redefined.
class TreeModel_ItemPropertiesStream : public TreeModel_ItemProperties
{
public:

  //! Constructor
  Standard_EXPORT TreeModel_ItemPropertiesStream (TreeModel_ItemBasePtr theItem)
    : TreeModel_ItemProperties (theItem) {}

  //! Destructor
  virtual ~TreeModel_ItemPropertiesStream() {}

  //! Returns Key of the current stream
  const TCollection_AsciiString& GetKey() const { return myKey; }

  //! Returns Key of the current stream
  Standard_EXPORT TCollection_AsciiString GetChildKey (const Standard_Integer theRow) const;

  //! Returns sub stream inside the current stream
  Standard_EXPORT void GetChildStream (const Standard_Integer theRow, Standard_OStream& OS) const;

  //! If me has internal values, it should be initialized here.
  Standard_EXPORT virtual void Init (const Standard_SStream& theStream);

  //! If the item has internal values, there should be reseted here.
  Standard_EXPORT virtual void Reset();

  //! Returns number of rows, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int GetTableRowCount() const Standard_OVERRIDE;

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It returns value only for DisplayRole.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant GetTableData (const int theRow, const int theColumn, const int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  Standard_EXPORT virtual ViewControl_EditType GetTableEditType (const int theRow, const int theColumn) const Standard_OVERRIDE;

  //! Sets the value into the table cell. Only 1st column value might be modified.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theValue a new cell value
  Standard_EXPORT virtual bool SetTableData (const int theRow, const int theColumn, const QVariant& theValue) Standard_OVERRIDE;

  //! Returns presentation of the attribute to be visualized in the view
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \thePresentations [out] container of presentation handles to be visualized
  Standard_EXPORT virtual void GetPresentations (const int theRow, const int theColumn,
    NCollection_List<Handle(Standard_Transient)>& thePresentations) Standard_OVERRIDE;

  //! Returns number of item children
  //! \return an integer value, ZERO by default
  Standard_EXPORT virtual int ChildItemCount() const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  Standard_EXPORT virtual TreeModel_ItemBasePtr CreateChildItem (int theRow, int theColumn) const;

  DEFINE_STANDARD_RTTIEXT (TreeModel_ItemPropertiesStream, TreeModel_ItemProperties)

protected:
  //! Returns attribute with stream value
  const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& GetChildrenValues() const
  {
    return myChildren;
  }

  //! Returns attribute with stream value
  const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& GetValues() const
  {
    return myValues;
  }

protected:
  TCollection_AsciiString myKey;
  Standard_SStream myStream;
  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> myValues;
  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> myChildren;
};

#endif

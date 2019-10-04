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

#ifndef TreeModel_ItemStream_H
#define TreeModel_ItemStream_H

#include <Standard.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <inspector/TreeModel_ItemBase.hxx>

class TreeModel_ItemProperties;
class TreeModel_ItemStream;

typedef QExplicitlySharedDataPointer<TreeModel_ItemStream> TreeModel_ItemStreamPtr;

//! \class TreeModel_ItemStream
//! Parent item, that corresponds to AIS_InteractiveContext
//! Children of the item are:
//! - "Property" item to show context attributes such as selection filters and drawer properties
//! - presentation items to show all interactive elements displayed/erased in the context
class TreeModel_ItemStream : public TreeModel_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static TreeModel_ItemStreamPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return TreeModel_ItemStreamPtr (new TreeModel_ItemStream (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~TreeModel_ItemStream() Standard_OVERRIDE {};

  //! Sets the item internal initialized state to the true. If the item has internal values,
  //! there should be initialized here.
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets the item and the child items content. Sets the initialized state to false.
  //! If the item has internal values, there should be reseted here.
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns number of displayed presentations
  //! \return rows count
  virtual int initRowCount() const Standard_OVERRIDE { initItem(); return /*GetChildren().Extent()*/0; }

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT void GetChildStream (const int theRowId,
                                       TCollection_AsciiString& theKey,
                                       TCollection_AsciiString& theValue) const;

  //! Returns data object of the item.
  //! \return object
  const TCollection_AsciiString& GetKey() const { initItem(); return myKey; }

  const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& GetChildren() const
    { initItem(); return myChildren; }

  //! Returns the item properties
  Handle(TreeModel_ItemProperties) Properties() const { return myProperties; }

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  virtual void GetStream (Standard_OStream& theOStream) const Standard_OVERRIDE;

protected:

  //! Initialize the current item. It creates a backup of the specific item information
  //! Do nothing as context has been already set into item
  virtual void initItem() const Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  Standard_EXPORT TreeModel_ItemStream(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn);

protected:
  Handle(TreeModel_ItemProperties) myProperties; //!< the properties
  TCollection_AsciiString myKey; //!< the item key
  TCollection_AsciiString myStreamValue; //!< the stream value
  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> myChildren; //!< the children
};

#endif

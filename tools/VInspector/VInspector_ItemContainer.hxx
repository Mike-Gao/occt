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

#ifndef VInspector_ItemContainer_H
#define VInspector_ItemContainer_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <AIS_InteractiveObject.hxx>
#include <NCollection_List.hxx>
#include <TCollection_AsciiString.hxx>

class Prs3d_Drawer;

class QItemSelectionModel;

class VInspector_ItemContainer;
typedef QExplicitlySharedDataPointer<VInspector_ItemContainer> VInspector_ItemContainerPtr;

//! \class VInspector_ItemContainer
//! Item presents additional level of information in the tree model.
//! Parent is item context, children are either folder item or Selection filter item.
class VInspector_ItemContainer : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemContainerPtr CreateItem (TreeModel_ItemBasePtr theParent,
    const int theRow, const int theColumn)
  { return VInspector_ItemContainerPtr (new VInspector_ItemContainer (theParent, theRow, theColumn)); }
  //! Destructor
  virtual ~VInspector_ItemContainer() Standard_OVERRIDE {};

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { return NULL; }

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

protected:

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of item selected
  //! \return rows count
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  VInspector_ItemContainer (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemBase (theParent, theRow, theColumn) {}

};

#endif

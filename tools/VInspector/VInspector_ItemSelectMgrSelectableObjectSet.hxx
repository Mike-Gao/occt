// Created on: 2019-04-29
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

#ifndef VInspector_ItemSelectMgrSelectableObjectSet_H
#define VInspector_ItemSelectMgrSelectableObjectSet_H

#include <inspector/VInspector_ItemBase.hxx>

#include <BVH_Tree.hxx>
#include <Standard.hxx>
#include <SelectMgr_SelectableObjectSet.hxx>
#include <TCollection_AsciiString.hxx>

class SelectMgr_BaseFrustum;

class VInspector_ItemSelectMgrSelectableObjectSet;
typedef QExplicitlySharedDataPointer<VInspector_ItemSelectMgrSelectableObjectSet> VInspector_ItemSelectMgrSelectableObjectSetPtr;

//! \class VInspector_ItemSelectMgrSelectableObjectSet
//! Parent item, that corresponds Folder under the AIS_InteractiveContext
//! Children of the item are: none
class VInspector_ItemSelectMgrSelectableObjectSet : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemSelectMgrSelectableObjectSetPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemSelectMgrSelectableObjectSetPtr (new VInspector_ItemSelectMgrSelectableObjectSet (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemSelectMgrSelectableObjectSet() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return NULL; }

  //! Returns current drawer, initialize the drawer if it was not initialized yet
  Standard_EXPORT Standard_Boolean GetSelectableObjectSet (SelectMgr_SelectableObjectSet& theSet) const;

  //! Returns child BVH tree of the row
  opencascade::handle<BVH_Tree<Standard_Real, 3> > GetBVHTree (const int theRow, TCollection_AsciiString& theName) const;

protected:
  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of displayed presentations
  //! \return rows count
  Standard_EXPORT virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Returns number of table rows
  //! \return an integer value
  virtual int GetTableRowCount() const Standard_OVERRIDE;

  //! Returns table value for the row in form: <function name> <function value>
  //! \param theRow a model index row
  //! \param theColumn a model index column
  virtual QVariant GetTableData (const int theRow, const int theColumn, const int theRole) const Standard_OVERRIDE;

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
  VInspector_ItemSelectMgrSelectableObjectSet(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}

};

#endif

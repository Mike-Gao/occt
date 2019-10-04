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

#ifndef VInspector_ItemSelectMgrSensitiveEntitySet_H
#define VInspector_ItemSelectMgrSensitiveEntitySet_H

#include <AIS_InteractiveObject.hxx>
#include <BVH_Tree.hxx>
#include <SelectMgr_SensitiveEntitySet.hxx>
#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

class SelectMgr_SelectableObject;

class QItemSelectionModel;
class VInspector_ItemSelectMgrSensitiveEntitySet;

typedef QExplicitlySharedDataPointer<VInspector_ItemSelectMgrSensitiveEntitySet> VInspector_ItemSelectMgrSensitiveEntitySetPtr;

//! \class VInspector_ItemSelectMgrSensitiveEntitySet
class VInspector_ItemSelectMgrSensitiveEntitySet : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemSelectMgrSensitiveEntitySetPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemSelectMgrSensitiveEntitySetPtr (new VInspector_ItemSelectMgrSensitiveEntitySet (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemSelectMgrSensitiveEntitySet() Standard_OVERRIDE {};

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return myEntitySet; }

  //! \return the current sensitive entity
  Standard_EXPORT Handle(SelectMgr_SensitiveEntitySet) GetSensitiveEntitySet() const
  { return Handle(SelectMgr_SensitiveEntitySet)::DownCast (GetObject()); }

  //! \return the current sensitive entity
  Standard_EXPORT const Handle(SelectMgr_SelectableObject)& GetSelectableObject() const
  { GetObject(); return mySelectableObject; }

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns child BVH tree of the row
  opencascade::handle<BVH_Tree<Standard_Real, 3> > GetBVHTree (const int theRow, TCollection_AsciiString& theName) const;

protected:

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

  //! Returns table value for the row in form: <function name> <function value> depending on the aspect kind
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theEntityKind kind or kind of entity
  QVariant getTableData (const int theRow,
                         const int theColumn,
                         const int theRole,
                         const TCollection_AsciiString& theEntityKind) const;

protected:
  //! Build presentation shape
  //! \return generated shape of the item parameters
  virtual TopoDS_Shape buildPresentationShape() Standard_OVERRIDE;

  //! Constructor
  //! param theParent a parent item
  VInspector_ItemSelectMgrSensitiveEntitySet(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemBase(theParent, theRow, theColumn) {}

private:

  Handle(SelectMgr_SensitiveEntitySet) myEntitySet; //!< the current sensitive entity
  Handle(SelectMgr_SelectableObject) mySelectableObject; //!< the current presentation
};

#endif

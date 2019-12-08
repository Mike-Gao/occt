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

#ifndef VInspector_ItemBVHTree_H
#define VInspector_ItemBVHTree_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <TopoDS_Shape.hxx>
#include <BVH_Tree.hxx>
#include <Standard_OStream.hxx>

class VInspector_ItemBVHTree;
typedef QExplicitlySharedDataPointer<VInspector_ItemBVHTree> VInspector_ItemBVHTreePtr;

//! \class VInspector_ItemBVHTree
//! Parent item, that corresponds Folder under the AIS_InteractiveContext
//! Children of the item are: none
class VInspector_ItemBVHTree : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemBVHTreePtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemBVHTreePtr (new VInspector_ItemBVHTree (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemBVHTree() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return myTree; }

  //! Returns current drawer, initialize the drawer if it was not initialized yet
  opencascade::handle<BVH_Tree<Standard_Real, 3> > GetTree() const
  { return opencascade::handle<BVH_Tree<Standard_Real, 3> >::DownCast (GetObject()); }

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void GetStream (Standard_OStream& theOStream) const Standard_OVERRIDE;

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

  //! Build presentation shape
  //! \return generated shape of the item parameters
  virtual TopoDS_Shape buildPresentationShape() Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Set V3d viewer selector into the current field
  //! \param theTree a viewer selector
  void setTree (const opencascade::handle<BVH_Tree<Standard_Real, 3> >& theTree) { myTree = theTree; }

private:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemBVHTree(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn);

private:

  opencascade::handle<BVH_Tree<Standard_Real, 3> > myTree; //!< the current tree
  TCollection_AsciiString myName; //!< the name
};

#endif

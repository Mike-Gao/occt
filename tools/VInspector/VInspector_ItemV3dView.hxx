// Created on: 2018-08-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef VInspector_ItemV3dView_H
#define VInspector_ItemV3dView_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>

class VInspector_ItemV3dView;
typedef QExplicitlySharedDataPointer<VInspector_ItemV3dView> VInspector_ItemV3dViewPtr;

//! \class VInspector_ItemV3dView
//! Parent item, that corresponds to AIS_InteractiveContext
//! Children of the item are:
//! - "Property" item to show context attributes such as selection filters and drawer properties
//! - presentation items to show all interactive elements displayed/erased in the context
class VInspector_ItemV3dView : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemV3dViewPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemV3dViewPtr (new VInspector_ItemV3dView (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemV3dView() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return myView; }

  //! Returns current drawer, initialize the drawer if it was not initialized yet
  Standard_EXPORT Handle(V3d_View) GetView() const
  { return Handle(V3d_View)::DownCast (GetObject()); }

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

protected:

  //! Build presentation shape
  //! \return generated shape of the item parameters
  virtual TopoDS_Shape buildPresentationShape() Standard_OVERRIDE { return buildPresentationShape (myView); }

    //! Creates shape for the 3d view parameters
  //! \param theView current view
  //! \return shape or NULL
  static TopoDS_Shape buildPresentationShape (const Handle(V3d_View)& theView);

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Set V3d view into the current field
  //! \param theViewer a viewer
  void setView (const Handle(V3d_View)& theView) { myView = theView; }

private:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemV3dView(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}

protected:

  Handle(V3d_View) myView; //!< the current view
};

#endif

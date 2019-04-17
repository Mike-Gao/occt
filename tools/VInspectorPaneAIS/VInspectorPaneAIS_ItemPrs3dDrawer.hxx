// Created on: 2018-08-10
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

#ifndef VInspectorPaneAIS_ItemPrs3dDrawer_H
#define VInspectorPaneAIS_ItemPrs3dDrawer_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>
#include <inspector/VInspector_ItemPrs3dDrawer.hxx>

#include <TopoDS_Shape.hxx>

class Prs3d_BasicAspect;
class QItemSelectionModel;

class VInspectorPaneAIS_ItemPrs3dDrawer;
typedef QExplicitlySharedDataPointer<VInspectorPaneAIS_ItemPrs3dDrawer> VInspectorPaneAIS_ItemPrs3dDrawerPtr;

//! \class VInspectorPaneAIS_ItemPrs3dDrawer
//! Item presents information about AIS_InteractiveObject.
//! Parent is item context, children are item selections.
class VInspectorPaneAIS_ItemPrs3dDrawer : public VInspector_ItemPrs3dDrawer
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspectorPaneAIS_ItemPrs3dDrawerPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspectorPaneAIS_ItemPrs3dDrawerPtr (new VInspectorPaneAIS_ItemPrs3dDrawer (theParent, theRow, theColumn)); }
  //! Destructor
  virtual ~VInspectorPaneAIS_ItemPrs3dDrawer() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns number of table rows
  //! \return an integer value
  virtual int GetTableRowCount() const Standard_OVERRIDE;

  //! Returns table value for the row in form: <function name> <function value>
  //! \param theRow a model index row
  //! \param theColumn a model index column
  virtual QVariant GetTableData (const int theRow, const int theColumn, const int theRole) const Standard_OVERRIDE;

protected:

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  VInspectorPaneAIS_ItemPrs3dDrawer (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemPrs3dDrawer (theParent, theRow, theColumn) {}

protected:

  TopoDS_Shape myShape; //!< current shape

  friend class VInspectorPaneAIS_ColoredShape;
};

#endif

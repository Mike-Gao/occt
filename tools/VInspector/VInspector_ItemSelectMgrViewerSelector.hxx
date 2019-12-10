// Created on: 2019-02-04
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

#ifndef VInspector_ItemSelectMgrViewerSelector_H
#define VInspector_ItemSelectMgrViewerSelector_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>
#include <inspector/VInspector_ItemContainerAPI.hxx>

#include <TopoDS_Shape.hxx>
#include <SelectMgr_ViewerSelector.hxx>
#include <Standard_OStream.hxx>

class VInspector_ItemSelectMgrViewerSelector;
typedef QExplicitlySharedDataPointer<VInspector_ItemSelectMgrViewerSelector> VInspector_ItemSelectMgrViewerSelectorPtr;

//! \class VInspector_ItemSelectMgrViewerSelector
//! Parent item, that corresponds Folder under the AIS_InteractiveContext
//! Children of the item are: none
class VInspector_ItemSelectMgrViewerSelector : public VInspector_ItemBase, public VInspector_ItemContainerAPI
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemSelectMgrViewerSelectorPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemSelectMgrViewerSelectorPtr (new VInspector_ItemSelectMgrViewerSelector (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemSelectMgrViewerSelector() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return myViewerSelector; }

  //! Returns current drawer, initialize the drawer if it was not initialized yet
  Standard_EXPORT Handle(SelectMgr_ViewerSelector) GetViewerSelector() const
  { return Handle(SelectMgr_ViewerSelector)::DownCast (GetObject()); }

  //! Returns the span from the 0 row to the first item corresponded to the picked item
  //! the 0 item is SelectMgr_SelectingVolumeManager
  //! the 1 item is VInspector_ItemSelectMgrSelectableObjectSet
  //! the 2 item is VInspector_ItemContainer for SelectMgr_MapOfObjectSensitives
  Standard_Integer GetFirstChildOfPicked() const { return 3; }

  //! Returns entity set if possible from SelectMgr_MapOfObjectSensitives
  //! \param theRow row index
  //! \param theObject [out] object connected to the sensitive entity set
  Standard_EXPORT Handle(SelectMgr_SensitiveEntitySet) GetSensitiveEntitySet (const int theRow,
    Handle(SelectMgr_SelectableObject)& theObject);

  //! Returns number of item selected
  //! \return rows count
  virtual int GetContainerRowCount (const int theContainerRow) const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant GetContainerValue (const int theContainerRow, const int theItemRole) const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr CreateContainerChild (const TreeModel_ItemBasePtr& theParent, const int theContainerRow, int theRow, int theColumn) Standard_OVERRIDE;

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

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream (Standard_OStream& theOStream) const Standard_OVERRIDE;

  //! Build presentation shape
  //! \return generated shape of the item parameters
  virtual TopoDS_Shape buildPresentationShape() Standard_OVERRIDE { return buildPresentationShape (myViewerSelector); }

    //! Creates shape for the 3d viewer selector parameters
  //! \param theViewerSelector current viewer selector
  //! \return shape or NULL
  static TopoDS_Shape buildPresentationShape (const Handle(SelectMgr_ViewerSelector)& theViewerSelector);

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Set V3d viewer selector into the current field
  //! \param theViewerSelector a viewer selector
  void setViewerSelector (const Handle(SelectMgr_ViewerSelector)& theViewerSelector) { myViewerSelector = theViewerSelector; }

private:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemSelectMgrViewerSelector(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn);

private:

  Handle(SelectMgr_ViewerSelector) myViewerSelector; //!< the current viewer selector

  Standard_Integer myXPix; //!< cached value for picked X
  Standard_Integer myYPix; //!< cached value for picked Y

  Standard_Integer myXMinPix; //!< cached value for min value of picked X
  Standard_Integer myYMinPix; //!< cached value for min value of picked Y

  Standard_Integer myXMaxPix; //!< cached value for max value of picked X
  Standard_Integer myYMaxPix; //!< cached value for max value of picked Y
};

#endif

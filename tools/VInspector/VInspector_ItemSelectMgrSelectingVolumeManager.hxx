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

#ifndef VInspector_ItemSelectMgrSelectingVolumeManager_H
#define VInspector_ItemSelectMgrSelectingVolumeManager_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <TopoDS_Shape.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>

class SelectMgr_BaseFrustum;

class VInspector_ItemSelectMgrSelectingVolumeManager;
typedef QExplicitlySharedDataPointer<VInspector_ItemSelectMgrSelectingVolumeManager> VInspector_ItemSelectMgrSelectingVolumeManagerPtr;

//! \class VInspector_ItemSelectMgrSelectingVolumeManager
//! Parent item, that corresponds Folder under the AIS_InteractiveContext
//! Children of the item are: none
class VInspector_ItemSelectMgrSelectingVolumeManager : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemSelectMgrSelectingVolumeManagerPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemSelectMgrSelectingVolumeManagerPtr (new VInspector_ItemSelectMgrSelectingVolumeManager (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemSelectMgrSelectingVolumeManager() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return NULL; }

  //! Returns current drawer, initialize the drawer if it was not initialized yet
  Standard_EXPORT Standard_Boolean GetViewerSelector (SelectMgr_SelectingVolumeManager& theVolumeManager) const;

  //! Returns frustum Frustum for row = 0, FrustumSet for row = 1   
  Handle(SelectMgr_BaseFrustum) GetFrustum (const int theRow) const;

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

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream (Standard_OStream& theOStream) const Standard_OVERRIDE;

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
  VInspector_ItemSelectMgrSelectingVolumeManager(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}

};

#endif

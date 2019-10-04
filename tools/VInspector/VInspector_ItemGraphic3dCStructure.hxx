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

#ifndef VInspector_ItemGraphic3dCStructure_H
#define VInspector_ItemGraphic3dCStructure_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

class Graphic3d_CStructure;

class VInspector_ItemGraphic3dCStructure;
typedef QExplicitlySharedDataPointer<VInspector_ItemGraphic3dCStructure> VInspector_ItemGraphic3dCStructurePtr;

//! \class VInspector_ItemGraphic3dCStructure
//! Parent item, that corresponds to AIS_InteractiveContext
//! Children of the item are:
//! - "Property" item to show context attributes such as selection filters and drawer properties
//! - presentation items to show all interactive elements displayed/erased in the context
class VInspector_ItemGraphic3dCStructure : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemGraphic3dCStructurePtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemGraphic3dCStructurePtr (new VInspector_ItemGraphic3dCStructure (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemGraphic3dCStructure() Standard_OVERRIDE {};

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return myCStructure; }

  //! Returns the current C structure, init item if it was not initialized yet
  //! \return graphic C structure object
  Standard_EXPORT Handle(Graphic3d_CStructure) GetCStructure() const
    {  return Handle(Graphic3d_CStructure)::DownCast (GetObject()); }

  //! Returns group of the C structure
  //! \param theRowId a group index
  //! \return graphical group
  Standard_EXPORT Handle(Graphic3d_Group) GetGroup (const int theRowId) const;

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

protected:

  //! Initialize the current item. It creates a backup of the specific item information
  //! Do nothing as context has been already set into item
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of displayed presentations
  //! \return rows count
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

private:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemGraphic3dCStructure(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}

private:
  Handle(Graphic3d_CStructure) myCStructure;
};

#endif

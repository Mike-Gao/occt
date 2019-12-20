// Created on: 2019-03-15
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

#ifndef VInspector_ItemOpenGlLayerList_H
#define VInspector_ItemOpenGlLayerList_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <OpenGl_LayerList.hxx>

class VInspector_ItemOpenGlLayerList;
typedef QExplicitlySharedDataPointer<VInspector_ItemOpenGlLayerList> VInspector_ItemOpenGlLayerListPtr;

//! \class VInspector_ItemOpenGlLayerList
//! Parent item, that corresponds to OpenGl_LayerList
//! Children of the item are:
//! - OpenGl_Layer items 
class VInspector_ItemOpenGlLayerList : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemOpenGlLayerListPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemOpenGlLayerListPtr (new VInspector_ItemOpenGlLayerList (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemOpenGlLayerList() Standard_OVERRIDE {};

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return NULL; }

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns layer list if the View is OpenGl_View
  //! \param isDefault flag is true if the layer is absent and the default value of this class is used
  Standard_EXPORT const OpenGl_LayerList& GetLayerList (Standard_Boolean& isDefault) const;

  //! Returns layer list if the View is OpenGl_View
  //! \param theLayerId index of the layer
  Standard_EXPORT Handle(OpenGl_Layer) GetLayer (const int theRow, Graphic3d_ZLayerId& theLayerId) const;

protected:

  //! Initialize the current item. It creates a backup of the specific item information
  //! Do nothing as context has been already set into item
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
  Standard_EXPORT virtual void initStream (Standard_OStream& theOStream) const;

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
  VInspector_ItemOpenGlLayerList(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}
};

#endif

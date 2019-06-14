// Created on: 2018-12-14
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

#ifndef VInspector_ItemAspectWindow_H
#define VInspector_ItemAspectWindow_H

#include <AIS_InteractiveObject.hxx>
#include <Aspect_Window.hxx>
#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

class QItemSelectionModel;
class VInspector_ItemAspectWindow;

typedef QExplicitlySharedDataPointer<VInspector_ItemAspectWindow> VInspector_ItemAspectWindowPtr;

//! \class VInspector_ItemAspectWindow
//! The item shows information about SelectBasics_EntityOwner.
//! The parent is item selection, children are item entity owners
class VInspector_ItemAspectWindow : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemAspectWindowPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemAspectWindowPtr (new VInspector_ItemAspectWindow (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemAspectWindow() Standard_OVERRIDE {};

  //! Returns data object of the item.
  //! \return object
  virtual Handle(Standard_Transient) GetObject() const { initItem(); return myWindow; }

  //! \return the current sensitive entity
  Standard_EXPORT Handle(Aspect_Window) GetAspectWindow() const
  { return Handle(Aspect_Window)::DownCast (GetObject()); }

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

  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE { return 0; }

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE
  { (void)theRow; (void)theColumn; return TreeModel_ItemBasePtr(); }

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
  VInspector_ItemAspectWindow(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemBase(theParent, theRow, theColumn) {}

protected:
  Handle(Aspect_Window) myWindow; //!< aspect window
};

#endif

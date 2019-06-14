// Created on: 2019-04-14
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

#ifndef VInspectorPaneAIS_ColoredShape_H
#define VInspectorPaneAIS_ColoredShape_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <inspector/TreeModel_ItemProperties.hxx>

class AIS_ColoredShape;

//! \class VInspectorPaneAIS_ColoredShape
//! Item presents information about AIS_InteractiveObject.
//! Parent is item context, children are item selections.
class VInspectorPaneAIS_ColoredShape : public TreeModel_ItemProperties
{
public:
  //! Constructor
  VInspectorPaneAIS_ColoredShape (const TreeModel_ItemBasePtr& theItem) : TreeModel_ItemProperties (theItem) {}

  //! Destructor
  ~VInspectorPaneAIS_ColoredShape() {}

  //! Returns number of item children
  //! \return an integer value, ZERO by default
  Standard_EXPORT virtual int ChildItemCount() const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  Standard_EXPORT virtual TreeModel_ItemBasePtr CreateChildItem (int theRow, int theColumn) const Standard_OVERRIDE;

protected:
  //! Converts transient object to custom presentation type
  //! \return custom presentation
  Handle(AIS_ColoredShape) GetPresentation() const;
};

#endif

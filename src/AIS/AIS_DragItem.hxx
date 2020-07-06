// Created on: 2020-07-05
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _AIS_DragItem_HeaderFile
#define _AIS_DragItem_HeaderFile

#include <AIS_DragItem.hxx>

class V3d_View;

//! Interface to provide drag processing.
//! To use this interface, inherit AIS_InteractiveObject from it. After, implement functionality to
//! happen by drag action.
//! To perform transformation of an object use next code in your event processing chain:
//! @code
//! // catch mouse button down event
//! if (aDraggedObject->HasActiveMode())
//! {
//!   aDraggedObject->StartTransform (anXPix, anYPix, aV3dView);
//! }
//! ...
//! // or track mouse move event
//! if (aDraggedObject->HasActiveMode())
//! {
//!   aDraggedObject->Transform (anXPix, anYPix, aV3dView);
//!   aV3dView->Redraw();
//! }
//! ...
//! // or catch mouse button up event (apply) or escape event (cancel)
//! aDraggedObject->StopTransform(/*Standard_Boolean toApply*/);
//! @endcode
class AIS_DragItem
{
public:
  //! Constructs a drag item.
  Standard_EXPORT AIS_DragItem() {}

public:
  //! @return true if this drag item participates in dragging
  virtual Standard_Boolean HasActiveMode() const = 0;

  //! Init start (reference) transformation.
  //! @warning It is used in chain with StartTransform-Transform(gp_Trsf)-StopTransform
  virtual void StartTransform (const Standard_Integer theX,
                               const Standard_Integer theY,
                               const Handle(V3d_View)& theView) = 0;

  //! Reset start (reference) transformation.
  //! @param theToApply [in] option to apply or to cancel the started transformation.
  virtual void StopTransform (const Standard_Boolean theToApply = Standard_True) = 0;

  //! Apply transformation made from mouse moving from start position
  virtual gp_Trsf Transform (const Standard_Integer theX,
                             const Standard_Integer theY,
                             const Handle(V3d_View)& theView) = 0;
};

#endif // _AIS_DragItem_HeaderFile

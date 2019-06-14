// Created on: 2018-12-11
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

#ifndef View_View_CameraPositionPrs_H
#define View_View_CameraPositionPrs_H

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_Camera.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColgp_SequenceOfPnt.hxx>

//! \class View_CameraPositionPrs
//! \brief Displays contour of the camera position of another view
class View_CameraPositionPrs : public AIS_InteractiveObject
{
public:

  //! Constructor
  View_CameraPositionPrs (const Handle(AIS_InteractiveContext)& theContext,
                          const Standard_Integer theViewId)
  : myContext (theContext), myViewId (theViewId), myIsTransformationStarted (Standard_False) {}

  //! Destructor
  virtual ~View_CameraPositionPrs() {}

  Standard_EXPORT void StartTransformation (const int theX, const int theY);
  Standard_EXPORT void StopTransformation (const int theX, const int theY);
  Standard_EXPORT void Transform (const int theX, const int theY, const Handle(V3d_View) theView);

protected:
  Standard_EXPORT void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                const Handle(Prs3d_Presentation)& thePresentation,
                                const Standard_Integer theMode = 0) Standard_OVERRIDE;

  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                         const Standard_Integer theMode) Standard_OVERRIDE;

protected:
  //! Returns the camera of the viewer of the context by the view index
  //! \return camera instance or NULL
  Handle (V3d_View) GetView() const;

  //! Computes the camera bounding points
  //! \param theCamera source camera
  //! \return container of bound points
  TColgp_SequenceOfPnt getCameraPoints (const Handle (Graphic3d_Camera)& theCamera);

  //! Converts container of 3D points in the container of points in pixels on the parameter view
  //! \param thePoints container of source points
  //! \return container of projectd points
  TColgp_SequenceOfPnt2d projectToView (const TColgp_SequenceOfPnt& thePoints,
                                        const Handle(V3d_View)& theView);

protected:
  Handle(AIS_InteractiveContext) myContext; //!< interactive context
  Standard_Integer myViewId; //!< index of the view in the viewer
  Standard_Boolean myIsTransformationStarted;
  gp_Pnt2d myStartPosition;
};

#endif // View_CameraPositionPrs_H

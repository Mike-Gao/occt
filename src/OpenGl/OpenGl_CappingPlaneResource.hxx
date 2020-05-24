// Created on: 2013-08-15
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_CappingPlaneResource_H__
#define _OpenGl_CappingPlaneResource_H__

#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Resource.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_Matrix.hxx>
#include <Graphic3d_AspectFillCapping.hxx>
#include <Graphic3d_ClipPlane.hxx>

class OpenGl_CappingPlaneResource;
DEFINE_STANDARD_HANDLE (OpenGl_CappingPlaneResource, OpenGl_Resource)

//! Container of graphical resources for rendering capping plane
//! associated to graphical clipping plane.
//! This resource holds data necessary for OpenGl_CappingAlgo.
//! This object is implemented as OpenGl resource for the following reasons:
//! - one instance should be shared between contexts.
//! - instance associated to Graphic3d_AspectFillCapping data.
//! - should created and released within context (owns OpenGl elements and resources).
class OpenGl_CappingPlaneResource : public OpenGl_Resource
{
public:

  //! Create and assign style.
  Standard_EXPORT OpenGl_CappingPlaneResource (const Handle(Graphic3d_ClipPlane)& thePlane,
                                               const Handle(Graphic3d_AspectFillCapping)& theAspect);

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_CappingPlaneResource();

  //! Update resource data in the passed context.
  //! @param theContext   [in] the context
  //! @param theObjAspect [in] object aspect
  Standard_EXPORT void Update (const Handle(OpenGl_Context)& theContext,
                               const Handle(Graphic3d_Aspects)& theObjAspect);

//! Assign section style.
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_AspectFillCapping)& theAspect);

  //! Returns section style parameters.
  const Handle(Graphic3d_AspectFillCapping)& Aspect() const { return myGraphicAspect; }

  //! Release associated OpenGl resources.
  //! @param theContext [in] the resource context.
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage - not implemented.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE { return 0; }

  //! Return parent clipping plane structure.
  const Handle(Graphic3d_ClipPlane)& Plane() const { return myPlaneRoot; }

  //! @return primitive array of vertices to render infinite plane.
  static OpenGl_PrimitiveArray* BuildInfinitPlaneVertices();

  //! @return aspect face for rendering capping surface.
  inline const OpenGl_Aspects* AspectFace() const { return myAspect; }

//! Returns true if capping should draw hatch layer.
  Standard_Boolean ToDrawHatch() const 
  {
    return Aspect()->ToDrawHatch() 
       && (Aspect()->IsStippleHatch()
        || Aspect()->IsTextureHatch());
  }

  //! @return evaluated orientation matrix to transform infinite plane.
  inline const OpenGl_Matrix* Orientation() const { return &myOrientation; }

  //! Returns the shading aspect for drawing face of a clipping section itself.
  //! @param theObjectAspect [in] the aspect of an object if it requires combining.
  Standard_EXPORT  const OpenGl_Aspects* CappingFaceAspect (const OpenGl_Aspects* theObjectAspect) const;

  //! @return primitive array of vertices to render infinite plane.
  inline const OpenGl_PrimitiveArray& Primitives() const { return myPrimitives; }

  //! Returns the shading aspect for drawing hatch layer of a section.
  Standard_EXPORT const OpenGl_Aspects* HatchingFaceAspect() const;

private:

  //! Update precomputed plane orientation matrix.
  void updateTransform (const Handle(OpenGl_Context)& theCtx);

  //! Update resources.
  void updateAspect (const Handle(Graphic3d_Aspects)& theObjAspect);

private:

  OpenGl_PrimitiveArray       myPrimitives;    //!< vertices and texture coordinates for rendering
  Standard_Boolean            myPrimitivesUsed; //!< boolean flag if primitives are used
  OpenGl_Matrix               myOrientation;   //!< plane transformation matrix.
  OpenGl_Aspects*             myAspect;        //!< capping face aspect.
  Handle(Graphic3d_ClipPlane) myPlaneRoot;     //!< parent clipping plane structure.
  Handle(Graphic3d_Aspects)   myFillAreaAspect;//!< own capping aspect
  gp_XYZ                      myLocalOrigin;   //!< layer origin
  unsigned int                myEquationMod;   //!< modification counter for plane equation.
  unsigned int                myAspectMod;     //!< modification counter for aspect.

  Handle(Graphic3d_AspectFillCapping) myGraphicAspect;  //!< Section style settings from application's level.
  mutable OpenGl_Aspects              myCappingAspect;  //!< GL aspect for shading base layer of a capping section.
  mutable OpenGl_Aspects              myHatchingAspect; //!< GL aspect for shading hatching layer (additional to base) of a capping section.
  mutable Standard_Size               myHatchingState;

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_CappingPlaneResource,OpenGl_Resource) // Type definition

};

#endif

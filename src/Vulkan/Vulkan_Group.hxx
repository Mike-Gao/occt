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

#ifndef _Vulkan_Group_HeaderFile
#define _Vulkan_Group_HeaderFile

#include <Graphic3d_Group.hxx>

#include <NCollection_List.hxx>
#include <OpenGl_Element.hxx>

struct OpenGl_ElementNode
{
  OpenGl_Element*     elem;
  OpenGl_ElementNode* next;
  DEFINE_STANDARD_ALLOC
};


class Vulkan_Buffer;
struct Vulkan_PrimitiveArray
{
  Handle(Graphic3d_IndexBuffer) Indices;
  Handle(Graphic3d_Buffer)      Attribs;
  Handle(Graphic3d_BoundBuffer) Bounds;

  Handle(Vulkan_Buffer) VboAttribs;
  Handle(Vulkan_Buffer) VboIndices;
  Graphic3d_TypeOfPrimitiveArray Type;
};
typedef NCollection_List<Vulkan_PrimitiveArray> Vulkan_ListOfElements;

//! Implementation of low-level graphic group.
class Vulkan_Group : public Graphic3d_Group
{
  DEFINE_STANDARD_RTTIEXT(Vulkan_Group, Graphic3d_Group)
public:

  //! Create empty group.
  //! Will throw exception if not created by OpenGl_Structure.
  Standard_EXPORT Vulkan_Group (const Handle(Graphic3d_Structure)& theStruct);

  Standard_EXPORT virtual void Clear (const Standard_Boolean theToUpdateStructureMgr) Standard_OVERRIDE;

  //! Return line aspect.
  virtual Handle(Graphic3d_Aspects) Aspects() const Standard_OVERRIDE
  {
    return Handle(Graphic3d_Aspects)();
    /*return myAspects != NULL
         ? myAspects->Aspect()
         : Handle(Graphic3d_Aspects)();*/
  }

  //! Update aspect.
  Standard_EXPORT virtual void SetGroupPrimitivesAspect (const Handle(Graphic3d_Aspects)& theAspect) Standard_OVERRIDE;

  //! Append aspect as an element.
  Standard_EXPORT virtual void SetPrimitivesAspect (const Handle(Graphic3d_Aspects)& theAspect) Standard_OVERRIDE;

  //! Update presentation aspects after their modification.
  Standard_EXPORT virtual void SynchronizeAspects() Standard_OVERRIDE;

  //! Replace aspects specified in the replacement map.
  Standard_EXPORT virtual void ReplaceAspects (const Graphic3d_MapOfAspectsToAspects& theMap) Standard_OVERRIDE;

  //! Add primitive array element
  Standard_EXPORT virtual void AddPrimitiveArray (const Graphic3d_TypeOfPrimitiveArray theType,
                                                  const Handle(Graphic3d_IndexBuffer)& theIndices,
                                                  const Handle(Graphic3d_Buffer)&      theAttribs,
                                                  const Handle(Graphic3d_BoundBuffer)& theBounds,
                                                  const Standard_Boolean               theToEvalMinMax) Standard_OVERRIDE;

  //! Add text element
  Standard_EXPORT virtual void Text (const Standard_CString                  theTextUtf,
                                     const Graphic3d_Vertex&                 thePoint,
                                     const Standard_Real                     theHeight,
                                     const Standard_Real                     theAngle,
                                     const Graphic3d_TextPath                theTp,
                                     const Graphic3d_HorizontalTextAlignment theHta,
                                     const Graphic3d_VerticalTextAlignment   theVta,
                                     const Standard_Boolean                  theToEvalMinMax) Standard_OVERRIDE;

  //! Add text element in 3D space.
  Standard_EXPORT virtual void Text (const Standard_CString                  theTextUtf,
                                     const gp_Ax2&                           theOrientation,
                                     const Standard_Real                     theHeight,
                                     const Standard_Real                     theAngle,
                                     const Graphic3d_TextPath                theTp,
                                     const Graphic3d_HorizontalTextAlignment theHTA,
                                     const Graphic3d_VerticalTextAlignment   theVTA,
                                     const Standard_Boolean                  theToEvalMinMax,
                                     const Standard_Boolean                  theHasOwnAnchor = Standard_True) Standard_OVERRIDE;

  //! Add flipping element
  Standard_EXPORT virtual void SetFlippingOptions (const Standard_Boolean theIsEnabled,
                                                   const gp_Ax2&          theRefPlane) Standard_OVERRIDE;

  //! Add stencil test element
  Standard_EXPORT virtual void SetStencilTestOptions (const Standard_Boolean theIsEnabled) Standard_OVERRIDE;

public:

  //Vulkan_Structure* VkStruct() const { return (Vulkan_Structure* )(myStructure->CStructure().get()); }

  //Standard_EXPORT void AddElement (OpenGl_Element* theElem);

  //Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  //Standard_EXPORT virtual void Release (const Handle(OpenGl_Context)&   theGlCtx);

  //! Returns first OpenGL element node of the group.
  ///const OpenGl_ElementNode* FirstNode() const { return myFirst; }

  //! Returns OpenGL aspect.
  ///const OpenGl_Aspects* GlAspects() const { return myAspects; }

protected:

  Standard_EXPORT virtual ~Vulkan_Group();

///protected:
public:

  Vulkan_ListOfElements myElements;
  //OpenGl_Aspects*     myAspects;
  //OpenGl_ElementNode* myFirst;
  //OpenGl_ElementNode* myLast;

};

#endif // _Vulkan_Group_HeaderFile

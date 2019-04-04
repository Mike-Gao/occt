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

#ifndef _Vulkan_PrimitiveArray_HeaderFile
#define _Vulkan_PrimitiveArray_HeaderFile

#include <Graphic3d_TypeOfPrimitiveArray.hxx>
#include <Standard_Type.hxx>

class Graphic3d_Buffer;
class Graphic3d_BoundBuffer;
class Graphic3d_IndexBuffer;
class Vulkan_Context;
class Vulkan_Buffer;

class Vulkan_PrimitiveArray : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Vulkan_PrimitiveArray, Standard_Transient)
public:

  Standard_EXPORT Vulkan_PrimitiveArray (const Graphic3d_TypeOfPrimitiveArray theType,
                                         const Handle(Graphic3d_IndexBuffer)& theIndices,
                                         const Handle(Graphic3d_Buffer)&      theAttribs,
                                         const Handle(Graphic3d_BoundBuffer)& theBounds);

  Standard_EXPORT virtual ~Vulkan_PrimitiveArray();

  Standard_EXPORT virtual void Release();

  Standard_EXPORT virtual void Render (const Handle(Vulkan_Context)& theCtx);

protected:

  Handle(Graphic3d_IndexBuffer) myIndices;
  Handle(Graphic3d_Buffer)      myAttribs;
  Handle(Graphic3d_BoundBuffer) myBounds;

  Handle(Vulkan_Buffer) myVboAttribs;
  Handle(Vulkan_Buffer) myVboIndices;
  Graphic3d_TypeOfPrimitiveArray myType;

};

#endif // _Vulkan_PrimitiveArray_HeaderFile

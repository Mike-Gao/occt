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

#ifndef _Vulkan_RenderPass_HeaderFile
#define _Vulkan_RenderPass_HeaderFile

#include <Vulkan_ForwardDecl.hxx>
#include <Standard_Type.hxx>

class Vulkan_Device;
class Vulkan_Surface;

//! This class defines an Vulkan Render Pass.
class Vulkan_RenderPass : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Vulkan_RenderPass, Standard_Transient)
public:

  //! Constructor.
  Standard_EXPORT Vulkan_RenderPass();

  //! Destructor.
  Standard_EXPORT virtual ~Vulkan_RenderPass();

  //! Release the object, @sa vkDestroyRenderPass().
  Standard_EXPORT void Release (const Handle(Vulkan_Device)& theDevice);

  //! Create the object, @sa vkCreatePipelineCache().
  Standard_EXPORT bool Create (const Handle(Vulkan_Device)& theDevice,
                               const Handle(Vulkan_Surface)& theSurface);

  //! Return object.
  VkRenderPass RenderPass() const { return myVkRenderPass; }

protected:

  VkRenderPass myVkRenderPass;

};

#endif // _Vulkan_RenderPass_HeaderFile

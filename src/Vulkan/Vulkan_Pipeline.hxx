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

#ifndef _Vulkan_Pipeline_HeaderFile
#define _Vulkan_Pipeline_HeaderFile

#include <Graphic3d_Vec2.hxx>
#include <Vulkan_ForwardDecl.hxx>
#include <Standard_Type.hxx>

class Vulkan_Device;
class Vulkan_PipelineCache;
class Vulkan_PipelineLayout;
class Vulkan_RenderPass;
class Vulkan_Shader;

//! This class defines an Vulkan Pipeline.
class Vulkan_Pipeline : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Vulkan_Pipeline, Standard_Transient)
public:

  //! Constructor.
  Standard_EXPORT Vulkan_Pipeline();

  //! Destructor.
  Standard_EXPORT virtual ~Vulkan_Pipeline();

  //! Release the object, @sa vkDestroyPipeline().
  Standard_EXPORT void Release (const Handle(Vulkan_Device)& theDevice);

  //! Create the object, @sa vkCreateGraphicsPipelines().
  Standard_EXPORT bool Create (const Handle(Vulkan_Device)& theDevice,
                               const Handle(Vulkan_RenderPass)& theRenderPass,
                               const Handle(Vulkan_PipelineLayout)& theLayout,
                               const Handle(Vulkan_Shader)& theShaderVert,
                               const Handle(Vulkan_Shader)& theShaderFrag,
                               const Graphic3d_Vec2u& theViewport);

  //! Return object.
  VkPipeline Pipeline() const { return myVkPipeline; }

  const Handle(Vulkan_PipelineLayout)& PipelineLayout() const { return myPipelineLayout; }

protected:

  Handle(Vulkan_Shader) myShaderVert;
  Handle(Vulkan_Shader) myShaderFrag;
  Handle(Vulkan_PipelineCache)  myPipelineCache;
  Handle(Vulkan_PipelineLayout) myPipelineLayout;
  VkPipeline myVkPipeline;

};

#endif // _Vulkan_Pipeline_HeaderFile

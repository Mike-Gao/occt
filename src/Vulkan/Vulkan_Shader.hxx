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

#ifndef _Vulkan_Shader_HeaderFile
#define _Vulkan_Shader_HeaderFile

#include <Vulkan_ForwardDecl.hxx>
#include <Standard_Type.hxx>

class Vulkan_Device;
class TCollection_AsciiString;

//! This class defines an Vulkan shader.
class Vulkan_Shader : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Vulkan_Shader, Standard_Transient)
public:

  //! Constructor.
  Standard_EXPORT Vulkan_Shader();

  //! Destructor.
  Standard_EXPORT virtual ~Vulkan_Shader();

  //! Release the object, @sa vkDestroyShaderModule().
  Standard_EXPORT void Release (const Handle(Vulkan_Device)& theDevice);

  //! Create the object, @sa vkCreateShaderModule().
  Standard_EXPORT bool Create (const Handle(Vulkan_Device)& theDevice,
                               const uint32_t* theCode,
                               uint32_t theNbBytes);

  //! Create the object, @sa vkCreateShaderModule().
  Standard_EXPORT bool CreateFromFile (const Handle(Vulkan_Device)& theDevice,
                                       const TCollection_AsciiString& theFilePath);

  //! Return shader.
  VkShaderModule Shader() const { return myVkShader; }

protected:

  VkShaderModule myVkShader;

};

#endif // _Vulkan_Shader_HeaderFile

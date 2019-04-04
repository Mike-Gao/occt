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

#ifndef _Vulkan_Buffer_HeaderFile
#define _Vulkan_Buffer_HeaderFile

#include <Graphic3d_Vec.hxx>
#include <Vulkan_ForwardDecl.hxx>
#include <Standard_Type.hxx>

class Vulkan_Device;

enum Vulkan_BufferType
{
  Vulkan_BufferType_Uniform, //!< VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
  Vulkan_BufferType_Vertex,  //!< VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
  Vulkan_BufferType_Index,   //!< VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
};

//! This class defines an Vulkan buffer.
class Vulkan_Buffer : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Vulkan_Buffer, Standard_Transient)
public:

  //! Constructor.
  Standard_EXPORT Vulkan_Buffer();

  //! Destructor.
  Standard_EXPORT virtual ~Vulkan_Buffer();

  //! Return object.
  VkBuffer Buffer() const { return myVkBuffer; }

  //! Release the object, @sa vkDestroyBuffer().
  Standard_EXPORT void Release (const Handle(Vulkan_Device)& theDevice);

  //! Create the object, @sa vkCreateBuffer().
  Standard_EXPORT bool Create (const Handle(Vulkan_Device)& theDevice,
                               Standard_Size theSize,
                               Vulkan_BufferType theType);

  //! Init the object.
  bool Init (const Handle(Vulkan_Device)& theDevice,
             const Graphic3d_Vec2* theData,
             Standard_Size theLen)
  {
    const Standard_Size aSize = sizeof(*theData) * theLen;
    return init (theDevice, theData, aSize, Vulkan_BufferType_Vertex);
  }

  //! Init the object.
  bool Init (const Handle(Vulkan_Device)& theDevice,
             const Graphic3d_Vec3* theData,
             Standard_Size theLen)
  {
    const Standard_Size aSize = sizeof(*theData) * theLen;
    return init (theDevice, theData, aSize, Vulkan_BufferType_Vertex);
  }

//protected:

  //! Init the object.
  Standard_EXPORT bool init (const Handle(Vulkan_Device)& theDevice,
                             const void* theData,
                             Standard_Size theNbBytes,
                             Vulkan_BufferType theType);

protected:

  VkBuffer       myVkBuffer;
  VkDeviceMemory myVkMemory;
  Standard_Size  mySize;

};

#endif // _Vulkan_Buffer_HeaderFile
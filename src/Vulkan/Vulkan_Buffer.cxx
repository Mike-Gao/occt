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

#include <Vulkan_Buffer.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Vulkan_Device.hxx>

#include <vulkan/vulkan.h>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_Buffer, Standard_Transient)

// =======================================================================
// function : Vulkan_Buffer
// purpose  :
// =======================================================================
Vulkan_Buffer::Vulkan_Buffer()
: myVkBuffer (NULL),
  myVkMemory (NULL),
  mySize (0)
{
  //
}

// =======================================================================
// function : ~Vulkan_Buffer
// purpose  :
// =======================================================================
Vulkan_Buffer::~Vulkan_Buffer()
{
  Release (Handle(Vulkan_Device)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_Buffer::Release (const Handle(Vulkan_Device)& theDevice)
{
  mySize = 0;
  if (myVkBuffer != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_Buffer destroyed without Vulkan context",);
    vkDestroyBuffer (theDevice->Device(), myVkBuffer, theDevice->HostAllocator());
    myVkBuffer = NULL;
  }
  if (myVkMemory != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_Buffer destroyed without Vulkan context",);
    vkFreeMemory (theDevice->Device(), myVkMemory, theDevice->HostAllocator());
    myVkMemory = NULL;
  }
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool Vulkan_Buffer::Create (const Handle(Vulkan_Device)& theDevice,
                            Standard_Size theSize,
                            Vulkan_BufferType theType)
{
  if (myVkBuffer != NULL
   && mySize == theSize)
  {
    return true;
  }

  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL)
  {
    return false;
  }

  VkBufferCreateInfo aVkBuffInfo;
  aVkBuffInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  aVkBuffInfo.pNext = NULL;
  aVkBuffInfo.flags = 0;
  aVkBuffInfo.size = theSize;
  aVkBuffInfo.usage = 0;
  aVkBuffInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
  aVkBuffInfo.queueFamilyIndexCount = 0;
  aVkBuffInfo.pQueueFamilyIndices = NULL;
  switch (theType)
  {
    case Vulkan_BufferType_Uniform: aVkBuffInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
    case Vulkan_BufferType_Vertex:  aVkBuffInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;  break;
    case Vulkan_BufferType_Index:   aVkBuffInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;   break;
  }

  VkResult aRes = vkCreateBuffer (theDevice->Device(), &aVkBuffInfo, theDevice->HostAllocator(), &myVkBuffer);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Buffer, failed to create buffer: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  mySize = theSize;
  return true;
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
bool Vulkan_Buffer::init (const Handle(Vulkan_Device)& theDevice,
                          const void* theData,
                          Standard_Size theNbBytes,
                          Vulkan_BufferType theType)
{
  if (!Create (theDevice, theNbBytes, theType))
  {
    return false;
  }

  if (myVkMemory == NULL)
  {
    VkMemoryRequirements aVkMemReqs;
    vkGetBufferMemoryRequirements (theDevice->Device(), myVkBuffer, &aVkMemReqs);
    myVkMemory = theDevice->AllocateDeviceMemory (aVkMemReqs);
    if (myVkMemory == NULL)
    {
      return false;
    }
  }

  if (theData != NULL)
  {
    void* aDataPtr = NULL;
    VkResult aRes = vkMapMemory (theDevice->Device(), myVkMemory, 0, theNbBytes, 0, &aDataPtr);
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Buffer, failed to map device memory: ") + Vulkan_Device::FormatVkError (aRes));
      return false;
    }
    memcpy (aDataPtr, theData, theNbBytes);
    vkUnmapMemory(theDevice->Device(), myVkMemory);
  }

  VkResult aRes = vkBindBufferMemory (theDevice->Device(), myVkBuffer, myVkMemory, 0);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Buffer, failed to bind buffer memory: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  return true;
}
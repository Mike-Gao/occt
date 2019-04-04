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

#include <Vulkan_Fence.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Vulkan_Device.hxx>

#include <vulkan/vulkan.h>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_Fence, Standard_Transient)

// =======================================================================
// function : Vulkan_Fence
// purpose  :
// =======================================================================
Vulkan_Fence::Vulkan_Fence()
: myVkFence (NULL)
{
  //
}

// =======================================================================
// function : ~Vulkan_Fence
// purpose  :
// =======================================================================
Vulkan_Fence::~Vulkan_Fence()
{
  Release (Handle(Vulkan_Device)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_Fence::Release (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkFence != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_Fence destroyed without Vulkan context",);
    vkDestroyFence (theDevice->Device(), myVkFence, theDevice->HostAllocator());
    myVkFence = NULL;
  }
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool Vulkan_Fence::Create (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkFence != NULL)
  {
    return true;
  }

  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL)
  {
    return false;
  }

  VkFenceCreateInfo aVkFenceInfo;
  aVkFenceInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  aVkFenceInfo.pNext = NULL;
  aVkFenceInfo.flags = 0;

  VkResult aRes = vkCreateFence (theDevice->Device(), &aVkFenceInfo, theDevice->HostAllocator(), &myVkFence);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Fence, failed to create fence: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  return true;
}

// =======================================================================
// function : Wait
// purpose  :
// =======================================================================
bool Vulkan_Fence::Wait (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkFence == NULL)
  {
    return false;
  }

  VkResult aRes = vkWaitForFences (theDevice->Device(), 1, &myVkFence, VK_TRUE, UINT64_MAX);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Fence, failed to wait for fence: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }
  return true;
}

// =======================================================================
// function : Reset
// purpose  :
// =======================================================================
bool Vulkan_Fence::Reset (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkFence == NULL)
  {
    return false;
  }

  VkResult aRes = vkResetFences (theDevice->Device(), 1, &myVkFence);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Fence, failed to reset fence: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }
  return true;
}

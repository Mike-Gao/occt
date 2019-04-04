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

#include <Vulkan_PipelineCache.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Vulkan_Device.hxx>

#include <vulkan/vulkan.h>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_PipelineCache, Standard_Transient)

// =======================================================================
// function : Vulkan_PipelineCache
// purpose  :
// =======================================================================
Vulkan_PipelineCache::Vulkan_PipelineCache()
: myVkPipelineCache (NULL)
{
  //
}

// =======================================================================
// function : ~Vulkan_PipelineCache
// purpose  :
// =======================================================================
Vulkan_PipelineCache::~Vulkan_PipelineCache()
{
  Release (Handle(Vulkan_Device)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_PipelineCache::Release (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkPipelineCache != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_PipelineCache destroyed without Vulkan context",);
    vkDestroyPipelineCache (theDevice->Device(), myVkPipelineCache, theDevice->HostAllocator());
    myVkPipelineCache = NULL;
  }
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool Vulkan_PipelineCache::Create (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkPipelineCache != NULL)
  {
    return true;
  }

  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL)
  {
    return false;
  }

  VkPipelineCacheCreateInfo aVkPipelineCacheInfo;
  aVkPipelineCacheInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  aVkPipelineCacheInfo.pNext = NULL;
  aVkPipelineCacheInfo.flags = 0;
  aVkPipelineCacheInfo.initialDataSize = 0;
  aVkPipelineCacheInfo.pInitialData = NULL;

  VkResult aRes = vkCreatePipelineCache (theDevice->Device(), &aVkPipelineCacheInfo, theDevice->HostAllocator(), &myVkPipelineCache);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_PipelineCache, failed to create pipeline cache: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }
  return true;
}

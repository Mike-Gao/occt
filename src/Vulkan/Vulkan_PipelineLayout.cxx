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

#include <Vulkan_PipelineLayout.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Vulkan_Device.hxx>

#include <vulkan/vulkan.h>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_PipelineLayout, Standard_Transient)

// =======================================================================
// function : Vulkan_PipelineLayout
// purpose  :
// =======================================================================
Vulkan_PipelineLayout::Vulkan_PipelineLayout()
: myVkPipelineLayout (NULL)
{
  //
}

// =======================================================================
// function : ~Vulkan_PipelineLayout
// purpose  :
// =======================================================================
Vulkan_PipelineLayout::~Vulkan_PipelineLayout()
{
  Release (Handle(Vulkan_Device)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_PipelineLayout::Release (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkPipelineLayout != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_PipelineLayout destroyed without Vulkan context",);
    vkDestroyPipelineLayout (theDevice->Device(), myVkPipelineLayout, theDevice->HostAllocator());
    myVkPipelineLayout = NULL;
  }
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool Vulkan_PipelineLayout::Create (const Handle(Vulkan_Device)& theDevice,
                                    const VkDescriptorSetLayout& theDescSetLayout)
{
  if (myVkPipelineLayout != NULL)
  {
    return true;
  }

  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL)
  {
    return false;
  }

  VkPipelineLayoutCreateInfo aVkPipelineLayoutInfo;
  aVkPipelineLayoutInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  aVkPipelineLayoutInfo.pNext = NULL;
  aVkPipelineLayoutInfo.flags = 0;
  aVkPipelineLayoutInfo.setLayoutCount = theDescSetLayout != NULL ? 1 : 0;
  aVkPipelineLayoutInfo.pSetLayouts = &theDescSetLayout;
  aVkPipelineLayoutInfo.pushConstantRangeCount = 0;
  aVkPipelineLayoutInfo.pPushConstantRanges = NULL;

  VkResult aRes = vkCreatePipelineLayout (theDevice->Device(), &aVkPipelineLayoutInfo, theDevice->HostAllocator(), &myVkPipelineLayout);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_PipelineLayout, failed to create pipeline layout: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  return true;
}

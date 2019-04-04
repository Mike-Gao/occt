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

#include <Vulkan_RenderPass.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Vulkan_Device.hxx>
#include <Vulkan_Surface.hxx>

#include <vulkan/vulkan.h>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_RenderPass, Standard_Transient)

// =======================================================================
// function : Vulkan_RenderPass
// purpose  :
// =======================================================================
Vulkan_RenderPass::Vulkan_RenderPass()
: myVkRenderPass (NULL)
{
  //
}

// =======================================================================
// function : ~Vulkan_RenderPass
// purpose  :
// =======================================================================
Vulkan_RenderPass::~Vulkan_RenderPass()
{
  Release (Handle(Vulkan_Device)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_RenderPass::Release (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkRenderPass != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_RenderPass destroyed without Vulkan context",);
    vkDestroyRenderPass (theDevice->Device(), myVkRenderPass, theDevice->HostAllocator());
    myVkRenderPass = NULL;
  }
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool Vulkan_RenderPass::Create (const Handle(Vulkan_Device)& theDevice,
                                const Handle(Vulkan_Surface)& theSurface)
{
  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL)
  {
    return false;
  }

  VkAttachmentDescription aVkAttachDesc;
  aVkAttachDesc.flags          = 0;
  aVkAttachDesc.format         = !theSurface.IsNull() ? theSurface->SurfaceFormat().format : VK_FORMAT_UNDEFINED;
  aVkAttachDesc.samples        = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
  aVkAttachDesc.loadOp         = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
  aVkAttachDesc.storeOp        = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
  aVkAttachDesc.stencilLoadOp  = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  aVkAttachDesc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
  aVkAttachDesc.initialLayout  = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
  aVkAttachDesc.finalLayout    = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference aVkAttachRef;
  aVkAttachRef.attachment = 0;
  aVkAttachRef.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription aVkSubpassDesc;
  aVkSubpassDesc.flags = 0;
  aVkSubpassDesc.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
  aVkSubpassDesc.inputAttachmentCount = 0;
  aVkSubpassDesc.pInputAttachments = NULL;
  aVkSubpassDesc.colorAttachmentCount = 1;
  aVkSubpassDesc.pColorAttachments = &aVkAttachRef;
  aVkSubpassDesc.pResolveAttachments = NULL;
  aVkSubpassDesc.pDepthStencilAttachment = NULL;
  aVkSubpassDesc.preserveAttachmentCount = 0;
  aVkSubpassDesc.pPreserveAttachments = NULL;

  VkRenderPassCreateInfo aVkRenderPassInfo;
  aVkRenderPassInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  aVkRenderPassInfo.pNext = NULL;
  aVkRenderPassInfo.flags = 0;
  aVkRenderPassInfo.attachmentCount = 1;
  aVkRenderPassInfo.pAttachments = &aVkAttachDesc;
  aVkRenderPassInfo.subpassCount = 1;
  aVkRenderPassInfo.pSubpasses = &aVkSubpassDesc;
  aVkRenderPassInfo.dependencyCount = 0;
  aVkRenderPassInfo.pDependencies = NULL;

  VkResult aRes = vkCreateRenderPass (theDevice->Device(), &aVkRenderPassInfo, theDevice->HostAllocator(), &myVkRenderPass);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_RenderPass, failed to create render pass: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  return true;
}

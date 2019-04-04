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

#include <Vulkan_Pipeline.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Vulkan_Device.hxx>
#include <Vulkan_PipelineCache.hxx>
#include <Vulkan_PipelineLayout.hxx>
#include <Vulkan_RenderPass.hxx>
#include <Vulkan_Shader.hxx>

#include <vulkan/vulkan.h>

#include <vector>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_Pipeline, Standard_Transient)

// =======================================================================
// function : Vulkan_Pipeline
// purpose  :
// =======================================================================
Vulkan_Pipeline::Vulkan_Pipeline()
: myVkPipeline (NULL)
{
  //
}

// =======================================================================
// function : ~Vulkan_Pipeline
// purpose  :
// =======================================================================
Vulkan_Pipeline::~Vulkan_Pipeline()
{
  Release (Handle(Vulkan_Device)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_Pipeline::Release (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkPipeline != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_Pipeline destroyed without Vulkan context",);
    vkDestroyPipeline (theDevice->Device(), myVkPipeline, theDevice->HostAllocator());
    myVkPipeline = NULL;
  }
  if (!myPipelineLayout.IsNull())
  {
    myPipelineLayout->Release (theDevice);
    myPipelineLayout.Nullify();
  }
  if (!myPipelineCache.IsNull())
  {
    myPipelineCache->Release (theDevice);
    myPipelineCache.Nullify();
  }
  myShaderVert.Nullify();
  myShaderFrag.Nullify();
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool Vulkan_Pipeline::Create (const Handle(Vulkan_Device)& theDevice,
                              const Handle(Vulkan_RenderPass)& theRenderPass,
                              const Handle(Vulkan_PipelineLayout)& theLayout,
                              const Handle(Vulkan_Shader)& theShaderVert,
                              const Handle(Vulkan_Shader)& theShaderFrag,
                              const Graphic3d_Vec2u& theViewport)
{
  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL
   || theShaderVert.IsNull()
   || theShaderVert->Shader() == NULL
   || theShaderFrag.IsNull()
   || theShaderFrag->Shader() == NULL)
  {
    return false;
  }

  myShaderVert = theShaderVert;
  myShaderFrag = theShaderFrag;
  myPipelineLayout = theLayout;
  /*if (myPipelineLayout.IsNull())
  {
    myPipelineLayout = new Vulkan_PipelineLayout();
    if (!myPipelineLayout->Create (theDevice, NULL))
    {
      return false;
    }
  }*/
  if (myPipelineCache.IsNull())
  {
    myPipelineCache = new Vulkan_PipelineCache();
    if (!myPipelineCache->Create (theDevice))
    {
      return false;
    }
  }

  std::vector<VkPipelineShaderStageCreateInfo> aVkShaderStages (2);
  aVkShaderStages[0].sType	= VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  aVkShaderStages[0].pNext	= NULL;
  aVkShaderStages[0].flags	= 0;
  aVkShaderStages[0].stage	= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
  aVkShaderStages[0].module	=  theShaderVert->Shader();
  aVkShaderStages[0].pName	= "main";
  aVkShaderStages[0].pSpecializationInfo = NULL;

  aVkShaderStages[1].sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  aVkShaderStages[1].pNext = NULL;
  aVkShaderStages[1].flags = 0;
  aVkShaderStages[1].stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
  aVkShaderStages[1].module = theShaderFrag->Shader();
  aVkShaderStages[1].pName = "main";
  aVkShaderStages[1].pSpecializationInfo = NULL;

  VkVertexInputBindingDescription aVkVertexInputBinding;
  aVkVertexInputBinding.binding = 0;
  aVkVertexInputBinding.stride = sizeof(float) * 3 * 2; /// TODO Pos+Norm
  aVkVertexInputBinding.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;

  VkVertexInputAttributeDescription aVkVertexInputAttribute;
  aVkVertexInputAttribute.location = 0;
  aVkVertexInputAttribute.binding = 0;
  aVkVertexInputAttribute.format = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
  aVkVertexInputAttribute.offset = 0;

  VkPipelineVertexInputStateCreateInfo aVkVertexInput;
  aVkVertexInput.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  aVkVertexInput.pNext = NULL;
  aVkVertexInput.flags = 0;
  aVkVertexInput.vertexBindingDescriptionCount = 1;
  aVkVertexInput.pVertexBindingDescriptions = &aVkVertexInputBinding;
  aVkVertexInput.vertexAttributeDescriptionCount = 1;
  aVkVertexInput.pVertexAttributeDescriptions = &aVkVertexInputAttribute;

  VkPipelineInputAssemblyStateCreateInfo aVkInputAssembly;
  aVkInputAssembly.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  aVkInputAssembly.pNext = NULL;
  aVkInputAssembly.flags = 0;
  aVkInputAssembly.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  aVkInputAssembly.primitiveRestartEnable = VK_FALSE;

  const VkViewport aVkViewport
  {
    0.0f, 0.0f,
    (float)theViewport.x(), (float)theViewport.y(),
    0.0f, 1.0f
  };
  const VkRect2D aVkScissor
  {
    {0, 0},
    {theViewport.x(), theViewport.y()}
  };

  VkPipelineViewportStateCreateInfo aVkViewportState;
  aVkViewportState.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  aVkViewportState.pNext = NULL;
  aVkViewportState.flags = 0;
  aVkViewportState.viewportCount = 1;
  aVkViewportState.pViewports = &aVkViewport;
  aVkViewportState.scissorCount = 1;
  aVkViewportState.pScissors = &aVkScissor;

  VkPipelineRasterizationStateCreateInfo aVkRasterState;
  aVkRasterState.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  aVkRasterState.pNext = NULL;
  aVkRasterState.flags = 0;
  aVkRasterState.depthClampEnable = VK_FALSE;
  aVkRasterState.rasterizerDiscardEnable = VK_FALSE;
  aVkRasterState.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
  aVkRasterState.cullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
  aVkRasterState.frontFace = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
  aVkRasterState.depthBiasEnable = VK_FALSE;
  aVkRasterState.depthBiasConstantFactor = 0.0f;
  aVkRasterState.depthBiasClamp = 0.0f;
  aVkRasterState.depthBiasSlopeFactor = 0.0f;
  aVkRasterState.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo aVkMultisample;
  aVkMultisample.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  aVkMultisample.pNext = NULL;
  aVkMultisample.flags = 0;
  aVkMultisample.rasterizationSamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
  aVkMultisample.sampleShadingEnable = VK_FALSE;
  aVkMultisample.minSampleShading = 0;
  aVkMultisample.pSampleMask = NULL;
  aVkMultisample.alphaToCoverageEnable = VK_FALSE;
  aVkMultisample.alphaToOneEnable = VK_FALSE;

  VkPipelineDepthStencilStateCreateInfo aVkDepthStencil;
  aVkDepthStencil.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  aVkDepthStencil.pNext = NULL;
  aVkDepthStencil.flags = 0;
  aVkDepthStencil.depthTestEnable = VK_FALSE;
  aVkDepthStencil.depthWriteEnable = VK_FALSE;
  aVkDepthStencil.depthCompareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
  aVkDepthStencil.depthBoundsTestEnable = VK_FALSE;
  aVkDepthStencil.stencilTestEnable = VK_FALSE;
  aVkDepthStencil.front.failOp = VkStencilOp::VK_STENCIL_OP_KEEP;
  aVkDepthStencil.front.passOp = VkStencilOp::VK_STENCIL_OP_KEEP;
  aVkDepthStencil.front.depthFailOp = VkStencilOp::VK_STENCIL_OP_KEEP;
  aVkDepthStencil.front.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
  aVkDepthStencil.front.compareMask = 0;
  aVkDepthStencil.front.writeMask = 0;
  aVkDepthStencil.front.reference = 0;
  aVkDepthStencil.back.failOp = VkStencilOp::VK_STENCIL_OP_KEEP;
  aVkDepthStencil.back.passOp = VkStencilOp::VK_STENCIL_OP_KEEP;
  aVkDepthStencil.back.depthFailOp = VkStencilOp::VK_STENCIL_OP_KEEP;
  aVkDepthStencil.back.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
  aVkDepthStencil.back.compareMask = 0;
  aVkDepthStencil.back.writeMask = 0;
  aVkDepthStencil.back.reference = 0;
  aVkDepthStencil.minDepthBounds = 0.0f;
  aVkDepthStencil.maxDepthBounds = 0.0f;

  VkPipelineColorBlendAttachmentState aVkBlendAttachments;
  aVkBlendAttachments.blendEnable         = VK_FALSE;
  aVkBlendAttachments.srcColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
  aVkBlendAttachments.dstColorBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
  aVkBlendAttachments.colorBlendOp        = VkBlendOp::VK_BLEND_OP_ADD;
  aVkBlendAttachments.srcAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
  aVkBlendAttachments.dstAlphaBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
  aVkBlendAttachments.alphaBlendOp        = VkBlendOp::VK_BLEND_OP_ADD;
  aVkBlendAttachments.colorWriteMask = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;

  VkPipelineColorBlendStateCreateInfo aVkBlendState;
  aVkBlendState.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  aVkBlendState.pNext = NULL;
  aVkBlendState.flags = 0;
  aVkBlendState.logicOpEnable = VK_FALSE;
  aVkBlendState.logicOp = VkLogicOp::VK_LOGIC_OP_CLEAR;
  aVkBlendState.attachmentCount = 1;
  aVkBlendState.pAttachments = &aVkBlendAttachments;
  aVkBlendState.blendConstants;
  aVkBlendState.blendConstants[0] = 1.0f;
  aVkBlendState.blendConstants[1] = 1.0f;
  aVkBlendState.blendConstants[2] = 1.0f;
  aVkBlendState.blendConstants[3] = 1.0f;

  VkGraphicsPipelineCreateInfo aVkPipeInfo;
  aVkPipeInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  aVkPipeInfo.pNext = NULL;
  aVkPipeInfo.flags = 0;
  aVkPipeInfo.stageCount = (uint32_t )aVkShaderStages.size();
  aVkPipeInfo.pStages    = aVkShaderStages.data();
  aVkPipeInfo.pVertexInputState   = &aVkVertexInput;
  aVkPipeInfo.pInputAssemblyState = &aVkInputAssembly;
  aVkPipeInfo.pTessellationState  = VK_NULL_HANDLE;
  aVkPipeInfo.pViewportState      = &aVkViewportState;
  aVkPipeInfo.pRasterizationState = &aVkRasterState;
  aVkPipeInfo.pMultisampleState   = &aVkMultisample;
  aVkPipeInfo.pDepthStencilState  = &aVkDepthStencil;
  aVkPipeInfo.pColorBlendState    = &aVkBlendState;
  aVkPipeInfo.pDynamicState       = NULL;
  aVkPipeInfo.layout     = myPipelineLayout->PipelineLayout();
  aVkPipeInfo.renderPass = theRenderPass->RenderPass();
  aVkPipeInfo.subpass    = 0;
  aVkPipeInfo.basePipelineIndex = 0;
  aVkPipeInfo.basePipelineHandle = VK_NULL_HANDLE;

  VkResult aRes = vkCreateGraphicsPipelines (theDevice->Device(),
                                             !myPipelineCache.IsNull() ? myPipelineCache->PipelineCache() : NULL,
                                             1, &aVkPipeInfo,
                                             theDevice->HostAllocator(), &myVkPipeline);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Pipeline, failed to create pipeline: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  return true;
}

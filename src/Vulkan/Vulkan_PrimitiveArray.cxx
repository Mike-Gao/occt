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

#include <Vulkan_PrimitiveArray.hxx>

#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_GroupDefinitionError.hxx>
#include <Graphic3d_Structure.hxx>
#include <Vulkan_Buffer.hxx>
#include <Vulkan_CommandBuffer.hxx>
#include <Vulkan_Context.hxx>

#include <vulkan/vulkan.h>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_PrimitiveArray, Standard_Transient)

// =======================================================================
// function : Vulkan_PrimitiveArray
// purpose  :
// =======================================================================
Vulkan_PrimitiveArray::Vulkan_PrimitiveArray (const Graphic3d_TypeOfPrimitiveArray theType,
                                              const Handle(Graphic3d_IndexBuffer)& theIndices,
                                              const Handle(Graphic3d_Buffer)&      theAttribs,
                                              const Handle(Graphic3d_BoundBuffer)& theBounds)
: myIndices (theIndices),
  myAttribs (theAttribs),
  myBounds (theBounds),
  myType (theType)
{
  //
}

// =======================================================================
// function : ~Vulkan_PrimitiveArray
// purpose  :
// =======================================================================
Vulkan_PrimitiveArray::~Vulkan_PrimitiveArray()
{
  Release();
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_PrimitiveArray::Release()
{
  myVboAttribs.Nullify();
  myVboIndices.Nullify();
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void Vulkan_PrimitiveArray::Render (const Handle(Vulkan_Context)& theCtx)
{
  if (myVboAttribs.IsNull()
  && !myAttribs.IsNull())
  {
    myVboAttribs = new Vulkan_Buffer();
    myVboAttribs->init (theCtx->Device(), myAttribs->Data(), myAttribs->Size(), Vulkan_BufferType_Vertex);
  }
  if (myVboIndices.IsNull()
  && !myIndices.IsNull())
  {
    myVboIndices = new Vulkan_Buffer();
    myVboIndices->init (theCtx->Device(), myIndices->Data(), myIndices->Size(), Vulkan_BufferType_Index);
  }

  if (myVboAttribs.IsNull())
  {
    return;
  }

  Vulkun_PipelineCfg aCfg;
  aCfg.PrimType = myType;
  aCfg.NbAttributes = Min (myAttribs->NbAttributes, Vulkun_PipelineCfg::THE_MAX_NB_ATTRIBUTES);
  aCfg.Stride = myAttribs->IsInterleaved() ? myAttribs->Stride : 0;
  aCfg.ShadingModel = Graphic3d_TOSM_UNLIT;
  if (int(aCfg.PrimType) >= int(Graphic3d_TOPA_TRIANGLES))
  {
    aCfg.ShadingModel = aCfg.NbAttributes > 1 ? Graphic3d_TOSM_FRAGMENT : Graphic3d_TOSM_FACET;
  }
  uint32_t anOffset = 0;
  for (Standard_Integer anAttribIter = 0; anAttribIter < aCfg.NbAttributes; ++anAttribIter)
  {
    const Graphic3d_Attribute& anAttribSrc = myAttribs->Attribute (anAttribIter);
    Vulkun_VertexAttribute& anAttribDst = aCfg.Attributes[anAttribIter];
    anAttribDst.Location = anAttribSrc.Id;
    anAttribDst.DataType = anAttribSrc.DataType;
    anAttribDst.Offset   = anOffset;

    if (myAttribs->IsInterleaved())
    {
      anOffset += anAttribSrc.Stride();
    }
  }

  VkBuffer aVkVertBuffers[1] = { myVboAttribs->Buffer() };
  VkDeviceSize aVkVertOffsets[1] = {0};
  const VkCommandBuffer& aVkCmdBuffer = theCtx->CommandBuffer()->CommandBuffer();
  theCtx->ActivatePipeline (aCfg);
  vkCmdBindVertexBuffers (aVkCmdBuffer, 0, 1, aVkVertBuffers, aVkVertOffsets);
  if (!myVboIndices.IsNull())
  {
    vkCmdBindIndexBuffer (aVkCmdBuffer, myVboIndices->Buffer(), 0, myIndices->Stride == 4 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed (aVkCmdBuffer, myIndices->NbElements, 1, 0, 0, 0);
  }
  else
  {
    vkCmdDraw (aVkCmdBuffer, myAttribs->NbElements, 1, 0, 0);
  }
}

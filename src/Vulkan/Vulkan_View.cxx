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

#include <Vulkan_View.hxx>

#include <BVH_LinearBuilder.hxx>
#include <Graphic3d_FrameStats.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Vulkan_Buffer.hxx>
#include <Vulkan_Caps.hxx>
#include <Vulkan_Device.hxx>
#include <Vulkan_Fence.hxx>
#include <Vulkan_FrameStats.hxx>
#include <Vulkan_GraphicDriver.hxx>
#include <Vulkan_Group.hxx>
#include <Vulkan_Pipeline.hxx>
#include <Vulkan_PipelineLayout.hxx>
#include <Vulkan_RenderPass.hxx>
#include <Vulkan_Surface.hxx>
#include <Vulkan_Shader.hxx>
#include <Vulkan_Structure.hxx>

#include "../Graphic3d/Graphic3d_Structure.pxx"

#include "Vulkan_TestShader_fs_spv.pxx"
#include "Vulkan_TestShader_vs_spv.pxx"

#include <vulkan/vulkan.h>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_View, Graphic3d_CView)

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
Vulkan_View::Vulkan_View (const Handle(Graphic3d_StructureManager)& theMgr,
                          const Handle(Vulkan_GraphicDriver)& theDriver)
: Graphic3d_CView  (theMgr),
  myDriver         (theDriver.get()),
  myWasRedrawnGL   (Standard_False),
  myBackfacing     (Graphic3d_TOBM_AUTOMATIC),
  myLayer (Structure_MAX_PRIORITY - Structure_MIN_PRIORITY + 1, new BVH_LinearBuilder<Standard_Real, 3> (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth)),
  mySwapInterval (0),
  ///myZLayers        (Structure_MAX_PRIORITY - Structure_MIN_PRIORITY + 1),
  //myFboColorFormat       (GL_RGBA8),
  //myFboDepthFormat       (GL_DEPTH24_STENCIL8),
  myFrameCounter         (0),
  myHasFboBlit           (Standard_True),
  myToDisableOIT         (Standard_False),
  myToDisableOITMSAA     (Standard_False),
  myToDisableMSAA        (Standard_False),
  myTransientDrawToFront (Standard_True),
  myBackBufferRestored   (Standard_False),
  myIsImmediateDrawn     (Standard_False)
  //myTextureParams   (new OpenGl_Aspects()),
  //myBgGradientArray (new OpenGl_BackgroundArray (Graphic3d_TOB_GRADIENT)),
  //myBgTextureArray  (new OpenGl_BackgroundArray (Graphic3d_TOB_TEXTURE)),
{
  Handle(Graphic3d_CLight) aLight = new Graphic3d_CLight (Graphic3d_TOLS_AMBIENT);
  aLight->SetHeadlight (false);
  aLight->SetColor (Quantity_NOC_WHITE);
  myNoShadingLight = new Graphic3d_LightSet();
  myNoShadingLight->Add (aLight);

  /*myMainSceneFbos[0]         = new OpenGl_FrameBuffer();
  myMainSceneFbos[1]         = new OpenGl_FrameBuffer();
  myMainSceneFbosOit[0]      = new OpenGl_FrameBuffer();
  myMainSceneFbosOit[1]      = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[0]    = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[1]    = new OpenGl_FrameBuffer();
  myImmediateSceneFbosOit[0] = new OpenGl_FrameBuffer();
  myImmediateSceneFbosOit[1] = new OpenGl_FrameBuffer();
  myOpenGlFBO                = new OpenGl_FrameBuffer();
  myOpenGlFBO2               = new OpenGl_FrameBuffer();
  myRaytraceFBO1[0]          = new OpenGl_FrameBuffer();
  myRaytraceFBO1[1]          = new OpenGl_FrameBuffer();
  myRaytraceFBO2[0]          = new OpenGl_FrameBuffer();
  myRaytraceFBO2[1]          = new OpenGl_FrameBuffer();*/
}

// =======================================================================
// function : Destructor
// purpose  :
// =======================================================================
Vulkan_View::~Vulkan_View()
{
  ///ReleaseGlResources (NULL); // ensure ReleaseGlResources() was called within valid context
  ///OpenGl_Element::Destroy (NULL, myBgGradientArray);
  ///OpenGl_Element::Destroy (NULL, myBgTextureArray);
  ///OpenGl_Element::Destroy (NULL, myTextureParams);
}

// =======================================================================
// function : ReleaseGlResources
// purpose  :
// =======================================================================
/*void Vulkan_View::ReleaseGlResources (const Handle(OpenGl_Context)& theCtx)
{
  myGraduatedTrihedron.Release (theCtx.get());
  myFrameStatsPrs.Release (theCtx.get());

  if (!myTextureEnv.IsNull())
  {
    if (!theCtx.IsNull())
    {
      for (OpenGl_TextureSet::Iterator aTextureIter (myTextureEnv); aTextureIter.More(); aTextureIter.Next())
      {
        theCtx->DelayedRelease (aTextureIter.ChangeValue());
        aTextureIter.ChangeValue().Nullify();
      }
    }
    myTextureEnv.Nullify();
  }

  if (myTextureParams != NULL)
  {
    myTextureParams->Release (theCtx.get());
  }
  if (myBgGradientArray != NULL)
  {
    myBgGradientArray->Release (theCtx.get());
  }
  if (myBgTextureArray != NULL)
  {
    myBgTextureArray->Release (theCtx.get());
  }

  myMainSceneFbos[0]        ->Release (theCtx.get());
  myMainSceneFbos[1]        ->Release (theCtx.get());
  myMainSceneFbosOit[0]     ->Release (theCtx.get());
  myMainSceneFbosOit[1]     ->Release (theCtx.get());
  myImmediateSceneFbos[0]   ->Release (theCtx.get());
  myImmediateSceneFbos[1]   ->Release (theCtx.get());
  myImmediateSceneFbosOit[0]->Release (theCtx.get());
  myImmediateSceneFbosOit[1]->Release (theCtx.get());
  myOpenGlFBO               ->Release (theCtx.get());
  myOpenGlFBO2              ->Release (theCtx.get());
  myFullScreenQuad           .Release (theCtx.get());
  myFullScreenQuadFlip       .Release (theCtx.get());

  releaseRaytraceResources (theCtx);
}*/

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
void Vulkan_View::Remove()
{
  if (IsRemoved())
  {
    return;
  }

  myDriver->RemoveView (this);
  /// TODO
  myPlatformWindow.Nullify();

  Graphic3d_CView::Remove();
}

// =======================================================================
// function : SetLocalOrigin
// purpose  :
// =======================================================================
void Vulkan_View::SetLocalOrigin (const gp_XYZ& theOrigin)
{
  myLocalOrigin = theOrigin;
  /**const Handle(OpenGl_Context)& aCtx = myWorkspace->GetGlContext();
  if (!aCtx.IsNull())
  {
    aCtx->ShaderManager()->SetLocalOrigin (theOrigin);
  }*/
}

// =======================================================================
// function : SetTextureEnv
// purpose  :
// =======================================================================
void Vulkan_View::SetTextureEnv (const Handle(Graphic3d_TextureEnv)& )
{
  //
}

// =======================================================================
// function : SetImmediateModeDrawToFront
// purpose  :
// =======================================================================
Standard_Boolean Vulkan_View::SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer)
{
  const Standard_Boolean aPrevMode = myTransientDrawToFront;
  myTransientDrawToFront = theDrawToFrontBuffer;
  return aPrevMode;
}


// =======================================================================
// function : initSwapChain
// purpose  :
// =======================================================================
bool Vulkan_View::initSwapChain (const Handle(Aspect_Window)& theWindow,
                                 const Aspect_RenderingContext theContext)
{
  (void )theContext;
  myPlatformWindow = theWindow;
  mySwapInterval = myDriver->Options()->swapInterval;
  const Handle(Vulkan_Device)& aDevice = myDriver->Device();
  if (aDevice->Device() == NULL
  && !aDevice->Init (myDriver->Options()))
  {
    return false;
  }

  if (!mySurface.IsNull())
  {
    mySurface->Release (aDevice);
  }

  mySurface = new Vulkan_Surface();
  return mySurface->Init (aDevice, theWindow);
}

// =======================================================================
// function : SetWindow
// purpose  :
// =======================================================================
void Vulkan_View::SetWindow (const Handle(Aspect_Window)& theWindow,
                             const Aspect_RenderingContext theContext)
{
  if (!initSwapChain (theWindow, theContext))
  {
    throw Standard_ProgramError ("Vulkan_View::SetWindow() failed");
  }

  myHasFboBlit = Standard_True;
  Invalidate();
}

// =======================================================================
// function : Resized
// purpose  :
// =======================================================================
void Vulkan_View::Resized()
{
  if (myPlatformWindow.IsNull())
  {
    return;
  }

  /// TODO
  ///myWindow->Resize();
}

// =======================================================================
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean Vulkan_View::BufferDump (Image_PixMap& theImage, const Graphic3d_BufferType& theBufferType)
{
  (void )theImage;
  (void )theBufferType;
  return false;
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================
void Vulkan_View::AddZLayer (const Graphic3d_ZLayerId theLayerId)
{
  ///myZLayers.AddLayer (theLayerId);
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================
void Vulkan_View::RemoveZLayer (const Graphic3d_ZLayerId theLayerId)
{
  ///myZLayers.RemoveLayer (theLayerId);
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void Vulkan_View::SetZLayerSettings (const Graphic3d_ZLayerId        theLayerId,
                                     const Graphic3d_ZLayerSettings& theSettings)
{
  ///myZLayers.SetLayerSettings (theLayerId, theSettings);
}

//=======================================================================
//function : ZLayerMax
//purpose  :
//=======================================================================
Standard_Integer Vulkan_View::ZLayerMax() const
{
  Standard_Integer aLayerMax = Graphic3d_ZLayerId_Default;
  /*for (OpenGl_LayerSeqIds::Iterator aMapIt(myZLayers.LayerIDs()); aMapIt.More(); aMapIt.Next())
  {
    aLayerMax = Max (aLayerMax, aMapIt.Value());
  }*/

  return aLayerMax;
}

//=======================================================================
//function : InvalidateZLayerBoundingBox
//purpose  :
//=======================================================================
void Vulkan_View::InvalidateZLayerBoundingBox (const Graphic3d_ZLayerId theLayerId) const
{
  myLayer.InvalidateBoundingBox();
  /*if (myZLayers.LayerIDs().IsBound (theLayerId))
  {
    myZLayers.Layer (theLayerId).InvalidateBoundingBox();
  }
  else
  {
    const Standard_Integer aLayerMax = ZLayerMax();
    for (Standard_Integer aLayerId = Graphic3d_ZLayerId_Default; aLayerId < aLayerMax; ++aLayerId)
    {
      if (myZLayers.LayerIDs().IsBound (aLayerId))
      {
        const OpenGl_Layer& aLayer = myZLayers.Layer (aLayerId);
        if (aLayer.NbOfTransformPersistenceObjects() > 0)
        {
          aLayer.InvalidateBoundingBox();
        }
      }
    }
  }*/
}

//=======================================================================
//function : ZLayerBoundingBox
//purpose  :
//=======================================================================
Bnd_Box Vulkan_View::ZLayerBoundingBox (const Graphic3d_ZLayerId        theLayerId,
                                        const Handle(Graphic3d_Camera)& theCamera,
                                        const Standard_Integer          theWindowWidth,
                                        const Standard_Integer          theWindowHeight,
                                        const Standard_Boolean          theToIncludeAuxiliary) const
{
  Bnd_Box aBox;
  aBox = myLayer.BoundingBox (Identification(),
                              theCamera,
                              theWindowWidth,
                              theWindowHeight,
                              theToIncludeAuxiliary);
  /*if (myZLayers.LayerIDs().IsBound (theLayerId))
  {
    aBox = myZLayers.Layer (theLayerId).BoundingBox (Identification(),
                                                     theCamera,
                                                     theWindowWidth,
                                                     theWindowHeight,
                                                     theToIncludeAuxiliary);
  }

  // add bounding box of gradient/texture background for proper Z-fit
  if (theToIncludeAuxiliary
   && theLayerId == Graphic3d_ZLayerId_BotOSD
   && (myBgTextureArray->IsDefined()
    || myBgGradientArray->IsDefined()))
  {
    // Background is drawn using 2D transformation persistence
    // (e.g. it is actually placed in 3D coordinates within active camera position).
    // We add here full-screen plane with 2D transformation persistence
    // for simplicity (myBgTextureArray might define a little bit different options
    // but it is updated within ::Render())
    const Graphic3d_Mat4d& aProjectionMat = theCamera->ProjectionMatrix();
    const Graphic3d_Mat4d& aWorldViewMat  = theCamera->OrientationMatrix();
    Graphic3d_BndBox3d aBox2d (Graphic3d_Vec3d (0.0, 0.0, 0.0),
                               Graphic3d_Vec3d (double(theWindowWidth), double(theWindowHeight), 0.0));

    Graphic3d_TransformPers aTrsfPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER);
    aTrsfPers.Apply (theCamera,
                     aProjectionMat,
                     aWorldViewMat,
                     theWindowWidth,
                     theWindowHeight,
                     aBox2d);
    aBox.Add (gp_Pnt (aBox2d.CornerMin().x(), aBox2d.CornerMin().y(), aBox2d.CornerMin().z()));
    aBox.Add (gp_Pnt (aBox2d.CornerMax().x(), aBox2d.CornerMax().y(), aBox2d.CornerMax().z()));
  }*/

  return aBox;
}

//=======================================================================
//function : considerZoomPersistenceObjects
//purpose  :
//=======================================================================
void Vulkan_View::InvalidateBVHData (const Graphic3d_ZLayerId theLayerId)
{
  myLayer.InvalidateBVHData();
  //
}

//=======================================================================
//function : considerZoomPersistenceObjects
//purpose  :
//=======================================================================
Standard_Real Vulkan_View::considerZoomPersistenceObjects (const Graphic3d_ZLayerId        theLayerId,
                                                           const Handle(Graphic3d_Camera)& theCamera,
                                                           const Standard_Integer          theWindowWidth,
                                                           const Standard_Integer          theWindowHeight) const
{
  /*if (myZLayers.LayerIDs().IsBound (theLayerId))
  {
    return myZLayers.Layer (theLayerId).considerZoomPersistenceObjects (Identification(),
                                                                        theCamera,
                                                                        theWindowWidth,
                                                                        theWindowHeight);
  }*/

  return 1.0;
}

//=======================================================================
//function : FBO
//purpose  :
//=======================================================================
Handle(Standard_Transient) Vulkan_View::FBO() const
{
  return NULL; ///Handle(Standard_Transient)(myFBO);
}

//=======================================================================
//function : SetFBO
//purpose  :
//=======================================================================
void Vulkan_View::SetFBO (const Handle(Standard_Transient)& theFbo)
{
  ///myFBO = Handle(OpenGl_FrameBuffer)::DownCast (theFbo);
}

//=======================================================================
//function : FBOCreate
//purpose  :
//=======================================================================
Handle(Standard_Transient) Vulkan_View::FBOCreate (const Standard_Integer theWidth,
                                                   const Standard_Integer theHeight)
{
  return NULL;//return myWorkspace->FBOCreate (theWidth, theHeight);
}

//=======================================================================
//function : FBORelease
//purpose  :
//=======================================================================
void Vulkan_View::FBORelease (Handle(Standard_Transient)& theFbo)
{
  /*Handle(OpenGl_FrameBuffer) aFrameBuffer = Handle(OpenGl_FrameBuffer)::DownCast (theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

  myWorkspace->FBORelease (aFrameBuffer);
  theFbo.Nullify();*/
}

//=======================================================================
//function : FBOGetDimensions
//purpose  :
//=======================================================================
void Vulkan_View::FBOGetDimensions (const Handle(Standard_Transient)& theFbo,
                                    Standard_Integer& theWidth,
                                    Standard_Integer& theHeight,
                                    Standard_Integer& theWidthMax,
                                    Standard_Integer& theHeightMax)
{
  /*const Handle(OpenGl_FrameBuffer) aFrameBuffer = Handle(OpenGl_FrameBuffer)::DownCast (theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

  theWidth     = aFrameBuffer->GetVPSizeX(); // current viewport size
  theHeight    = aFrameBuffer->GetVPSizeY();
  theWidthMax  = aFrameBuffer->GetSizeX(); // texture size
  theHeightMax = aFrameBuffer->GetSizeY();*/
}

//=======================================================================
//function : FBOChangeViewport
//purpose  :
//=======================================================================
void Vulkan_View::FBOChangeViewport (const Handle(Standard_Transient)& theFbo,
                                     const Standard_Integer theWidth,
                                     const Standard_Integer theHeight)
{
  /*const Handle(OpenGl_FrameBuffer) aFrameBuffer = Handle(OpenGl_FrameBuffer)::DownCast (theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

  aFrameBuffer->ChangeViewport (theWidth, theHeight);*/
}

//=======================================================================
//function : displayStructure
//purpose  :
//=======================================================================
void Vulkan_View::displayStructure (const Handle(Graphic3d_CStructure)& theStructure,
                                    const Standard_Integer              thePriority)
{
  const Graphic3d_ZLayerId aZLayer = theStructure->ZLayer();
  myLayer.Add (theStructure.get(), thePriority);
  ///myZLayers.AddStructure (aStruct, aZLayer, thePriority);
}

//=======================================================================
//function : eraseStructure
//purpose  :
//=======================================================================
void Vulkan_View::eraseStructure (const Handle(Graphic3d_CStructure)& theStructure)
{
  int aPrior = 0;
  myLayer.Remove (theStructure.get(), aPrior);
  ///const OpenGl_Structure*  aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure.operator->());
  ///myZLayers.RemoveStructure (aStruct);
}

//=======================================================================
//function : changeZLayer
//purpose  :
//=======================================================================
void Vulkan_View::changeZLayer (const Handle(Graphic3d_CStructure)& theStructure,
                                const Graphic3d_ZLayerId theNewLayerId)
{
  const Graphic3d_ZLayerId anOldLayer = theStructure->ZLayer();
  ///const OpenGl_Structure* aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure.operator->());
  ///myZLayers.ChangeLayer (aStruct, anOldLayer, theNewLayerId);
  Update (anOldLayer);
  Update (theNewLayerId);
}

//=======================================================================
//function : changePriority
//purpose  :
//=======================================================================
void Vulkan_View::changePriority (const Handle(Graphic3d_CStructure)& theStructure,
                                  const Standard_Integer theNewPriority)
{
  const Graphic3d_ZLayerId aLayerId = theStructure->ZLayer();
  ///const OpenGl_Structure* aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure.operator->());
  ///myZLayers.ChangePriority (aStruct, aLayerId, theNewPriority);
}

//=======================================================================
//function : DiagnosticInformation
//purpose  :
//=======================================================================
void Vulkan_View::DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                         Graphic3d_DiagnosticInfo theFlags) const
{
  const Handle(Vulkan_Device)& aDevice = myDriver->Device();
  if (aDevice.IsNull())
  {
    return;
  }

  aDevice->DiagnosticInformation (theDict, theFlags);
  if ((theFlags & Graphic3d_DiagnosticInfo_FrameBuffer) != 0)
  {
    if (!mySurface.IsNull())
    {
      Graphic3d_Vec2u aSize = mySurface->CurrentSize (aDevice);
      TCollection_AsciiString aViewport = TCollection_AsciiString() + int(aSize.x()) + "x" + int(aSize.y());
      theDict.ChangeFromIndex (theDict.Add ("Viewport", aViewport)) = aViewport;
    }
    TCollection_AsciiString aResRatio (myRenderParams.ResolutionRatio());
    theDict.ChangeFromIndex (theDict.Add ("ResolutionRatio", aResRatio)) = aResRatio;
  }
}

//=======================================================================
//function : StatisticInformation
//purpose  :
//=======================================================================
void Vulkan_View::StatisticInformation (TColStd_IndexedDataMapOfStringString& theDict) const
{
  if (const Handle(Vulkan_Device)& aDevice = myDriver->Device())
  {
    const Handle(Vulkan_FrameStats)& aStats = aDevice->FrameStats();
    aStats->FormatStats (theDict, myRenderParams.CollectedStats);
  }
}

//=======================================================================
//function : StatisticInformation
//purpose  :
//=======================================================================
TCollection_AsciiString Vulkan_View::StatisticInformation() const
{
  if (const Handle(Vulkan_Device)& aDevice = myDriver->Device())
  {
    const Handle(Vulkan_FrameStats)& aStats = aDevice->FrameStats();
    return aStats->FormatStats (myRenderParams.CollectedStats);
  }
  return TCollection_AsciiString();
}

//=======================================================================
//function : Redraw
//purpose  :
//=======================================================================
void Vulkan_View::Redraw()
{
  const Handle(Vulkan_Device)& aDevice = myDriver->Device();
  if (aDevice->Device() == NULL
   || mySurface.IsNull()
   || mySurface->Surface() == NULL)
  {
    return;
  }

  Graphic3d_Vec2u aSurfSize = mySurface->CurrentSize (aDevice);
  if (aSurfSize.x() == 0
   || aSurfSize.y() == 0)
  {
    return;
  }

  static Handle(Vulkan_RenderPass) aRenderPass;
  if (aRenderPass.IsNull())
  {
    aRenderPass = new Vulkan_RenderPass();
    if (!aRenderPass->Create (aDevice, mySurface))
    {
      return;
    }
  }

  static Handle(Vulkan_Shader) aShaderVert, aShaderFrag;
  static Handle(Vulkan_Buffer) aShaderUniformBuffer;
  struct UniformsColors
  {
    Graphic3d_Mat4 occWorldViewMatrix;
    Graphic3d_Mat4 occProjectionMatrix;
    Graphic3d_Mat4 occModelWorldMatrix;
    Graphic3d_Vec4 uColor;
  } aUniformsColors;
  aUniformsColors.occProjectionMatrix = myCamera->ProjectionMatrixF();
  aUniformsColors.occWorldViewMatrix  = myCamera->OrientationMatrixF();

  aUniformsColors.uColor.SetValues (1.0f, 0.0f, 0.0f, 1.0f);
  if (aShaderVert.IsNull())
  {
    aUniformsColors.uColor.SetValues (1.0f, 1.0f, 1.0f, 1.0f);

    aShaderVert = new Vulkan_Shader();
    aShaderFrag = new Vulkan_Shader();
    aShaderUniformBuffer = new Vulkan_Buffer();
    aShaderVert->Create (aDevice, Vulkan_TestShader_vs_spv, sizeof(Vulkan_TestShader_vs_spv));
    aShaderFrag->Create (aDevice, Vulkan_TestShader_fs_spv, sizeof(Vulkan_TestShader_fs_spv));
   
    //aShaderVert->CreateFromFile (aDevice, "C:/work/test.vs.spv");
    //aShaderFrag->CreateFromFile (aDevice, "C:/work/test.fs.spv");
  }
  aShaderUniformBuffer->init (aDevice, &aUniformsColors, sizeof(aUniformsColors), Vulkan_BufferType_Uniform);

  std::vector<VkFramebuffer> aVkFrameBuffers (mySurface->SwapChainSize());
  for (uint32_t anImgIter = 0; anImgIter < mySurface->SwapChainSize(); ++anImgIter)
  {
    VkFramebufferCreateInfo aVkFboInfo;
    const VkImageView aVkImageView = mySurface->ImageViews()[anImgIter];
    aVkFboInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    aVkFboInfo.pNext = NULL;
    aVkFboInfo.flags = 0;
    aVkFboInfo.renderPass = aRenderPass->RenderPass();
    aVkFboInfo.attachmentCount = 1;
    aVkFboInfo.pAttachments = &aVkImageView;
    aVkFboInfo.width  = aSurfSize.x();
    aVkFboInfo.height = aSurfSize.y();
    aVkFboInfo.layers = 1;

    VkFramebuffer& aVkFrameBuffer = aVkFrameBuffers[anImgIter];
    VkResult aRes = vkCreateFramebuffer (aDevice->Device(), &aVkFboInfo, NULL, &aVkFrameBuffer);
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Window, failed to create framebuffer: ") + Vulkan_Device::FormatVkError (aRes));
      return;
    }
  }

  VkQueue aVkQueue = NULL;
  vkGetDeviceQueue (aDevice->Device(), 0, 0, &aVkQueue);

  VkCommandPool aVkCmdPool = NULL;
  {
    VkCommandPoolCreateInfo aVkCmdPoolInfo;
    aVkCmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    aVkCmdPoolInfo.pNext = NULL;
    aVkCmdPoolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    aVkCmdPoolInfo.queueFamilyIndex = 0;

    VkResult aRes = vkCreateCommandPool (aDevice->Device(), &aVkCmdPoolInfo, NULL, &aVkCmdPool);
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Window, failed to create command pool: ") + Vulkan_Device::FormatVkError (aRes));
      return;
    }

    aRes = vkResetCommandPool (aDevice->Device(), aVkCmdPool, VkCommandPoolResetFlagBits::VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_View, failed to reset command pool: ") + Vulkan_Device::FormatVkError (aRes));
      return;
    }
  }

  static VkDescriptorSetLayout aVkDesSetLayout = NULL;
  if (aVkDesSetLayout == NULL)
  {
    VkDescriptorSetLayoutBinding aVkDescSetLayoutBinding = {};
    aVkDescSetLayoutBinding.binding = 0;
    aVkDescSetLayoutBinding.descriptorCount = 1;
    aVkDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    aVkDescSetLayoutBinding.pImmutableSamplers = NULL;
    aVkDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    ///aVkDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo aVkDescLayoutInfo = {};
    aVkDescLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    aVkDescLayoutInfo.bindingCount = 1;
    aVkDescLayoutInfo.pBindings = &aVkDescSetLayoutBinding;
    VkResult aRes = vkCreateDescriptorSetLayout (aDevice->Device(), &aVkDescLayoutInfo, aDevice->HostAllocator(), &aVkDesSetLayout);
    if (aRes != VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_View, failed to create descriptor set layout: ") + Vulkan_Device::FormatVkError (aRes));
      return;
    }
  }

  VkDescriptorPool aVkDescriptorPool = NULL;
  std::vector<VkDescriptorSet> aVkDescriptorSets (mySurface->SwapChainSize(), NULL);
  {
    VkDescriptorPoolSize aVkPoolSize = {};
    aVkPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    aVkPoolSize.descriptorCount = mySurface->SwapChainSize();

    VkDescriptorPoolCreateInfo aVkPoolInfo = {};
    aVkPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    aVkPoolInfo.poolSizeCount = 1;
    aVkPoolInfo.pPoolSizes = &aVkPoolSize;
    aVkPoolInfo.maxSets = mySurface->SwapChainSize();
    VkResult aRes = vkCreateDescriptorPool (aDevice->Device(), &aVkPoolInfo, aDevice->HostAllocator(), &aVkDescriptorPool);
    if (aRes != VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_View, failed to create descriptor pool: ") + Vulkan_Device::FormatVkError (aRes));
      return;
    }

    std::vector<VkDescriptorSetLayout> aDescSetLayouts (mySurface->SwapChainSize(), aVkDesSetLayout);
    VkDescriptorSetAllocateInfo aDescSetAllocInfo = {};
    aDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    aDescSetAllocInfo.descriptorPool = aVkDescriptorPool;
    aDescSetAllocInfo.descriptorSetCount = mySurface->SwapChainSize();
    aDescSetAllocInfo.pSetLayouts = aDescSetLayouts.data();

    aRes = vkAllocateDescriptorSets (aDevice->Device(), &aDescSetAllocInfo, aVkDescriptorSets.data());
    if (aRes != VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_View, failed to allocate descriptor sets: ") + Vulkan_Device::FormatVkError (aRes));
      return;
    }

    for (uint32_t anImgIter = 0; anImgIter < mySurface->SwapChainSize(); ++anImgIter)
    {
      VkDescriptorBufferInfo aVkDescBuffInfo = {};
      aVkDescBuffInfo.buffer = aShaderUniformBuffer->Buffer();
      aVkDescBuffInfo.offset = 0;
      aVkDescBuffInfo.range  = VK_WHOLE_SIZE;

      VkWriteDescriptorSet aVkWriteDescSet = {};
      aVkWriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      aVkWriteDescSet.dstSet = aVkDescriptorSets[anImgIter];
      aVkWriteDescSet.dstBinding = 0;
      aVkWriteDescSet.dstArrayElement = 0;
      aVkWriteDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      aVkWriteDescSet.descriptorCount = 1;
      aVkWriteDescSet.pBufferInfo = &aVkDescBuffInfo;
      aVkWriteDescSet.pImageInfo = NULL;
      aVkWriteDescSet.pTexelBufferView = NULL;
      vkUpdateDescriptorSets (aDevice->Device(), 1, &aVkWriteDescSet, 0, NULL);
    }
  }

  std::vector<VkCommandBuffer> aVkCmdBuffers (mySurface->SwapChainSize(), NULL);
  {
    VkCommandBufferAllocateInfo aVkCmdBufferAllocInfo;
    aVkCmdBufferAllocInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    aVkCmdBufferAllocInfo.pNext = NULL;
    aVkCmdBufferAllocInfo.commandPool = aVkCmdPool;
    aVkCmdBufferAllocInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    aVkCmdBufferAllocInfo.commandBufferCount = mySurface->SwapChainSize();

    VkResult aRes = vkAllocateCommandBuffers (aDevice->Device(), &aVkCmdBufferAllocInfo, aVkCmdBuffers.data());
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Window, failed to allocate command buffers: ") + Vulkan_Device::FormatVkError (aRes));
      return;
    }

    for (uint32_t anImgIter = 0; anImgIter < mySurface->SwapChainSize(); ++anImgIter)
    {
      VkCommandBuffer&    aVkCmdBuffer   = aVkCmdBuffers[anImgIter];
      const VkImage       aVkImage       = mySurface->Images()[anImgIter];
      const VkFramebuffer aVkFrameBuffer = aVkFrameBuffers[anImgIter];
      aRes = vkResetCommandBuffer (aVkCmdBuffer, VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
      if (aRes != VkResult::VK_SUCCESS)
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Window, failed to reset command buffer: ") + Vulkan_Device::FormatVkError (aRes));
        return;
      }

      VkCommandBufferInheritanceInfo aVkCmdBufferInherInfo;
      aVkCmdBufferInherInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
      aVkCmdBufferInherInfo.pNext = NULL;
      aVkCmdBufferInherInfo.renderPass = aRenderPass->RenderPass();
      aVkCmdBufferInherInfo.subpass = 0;
      aVkCmdBufferInherInfo.framebuffer = aVkFrameBuffer;
      aVkCmdBufferInherInfo.occlusionQueryEnable = VK_FALSE;
      aVkCmdBufferInherInfo.queryFlags = 0;
      aVkCmdBufferInherInfo.pipelineStatistics = 0;

      VkCommandBufferBeginInfo aVkCmdBufferBeginInfo;
      aVkCmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      aVkCmdBufferBeginInfo.pNext = NULL;
      aVkCmdBufferBeginInfo.flags = 0;
      aVkCmdBufferBeginInfo.pInheritanceInfo = &aVkCmdBufferInherInfo;

      vkBeginCommandBuffer (aVkCmdBuffer, &aVkCmdBufferBeginInfo);
      {
        VkClearValue aClearValue;
        const Graphic3d_Vec4& aBgColor = myBgColor;
        aClearValue.color.float32[0] = aBgColor.r();
        aClearValue.color.float32[1] = aBgColor.g();
        aClearValue.color.float32[2] = aBgColor.b();
        aClearValue.color.float32[3] = aBgColor.a();

        VkRenderPassBeginInfo aVkRenderPassBeginInfo;
        aVkRenderPassBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        aVkRenderPassBeginInfo.pNext = NULL;
        aVkRenderPassBeginInfo.renderPass = aRenderPass->RenderPass();
        aVkRenderPassBeginInfo.framebuffer = aVkFrameBuffer;
        aVkRenderPassBeginInfo.renderArea = VkRect2D{
          VkOffset2D{0, 0},
          VkExtent2D{aSurfSize.x(), aSurfSize.y()}
        };
        aVkRenderPassBeginInfo.clearValueCount = 1;
        aVkRenderPassBeginInfo.pClearValues = &aClearValue;

        vkCmdBeginRenderPass (aVkCmdBuffer, &aVkRenderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
        {
          static Handle(Vulkan_Pipeline) aPipe;
          static Handle(Vulkan_PipelineLayout) aPipeLayout;
          if (aPipe.IsNull())
          {
            aPipeLayout = new Vulkan_PipelineLayout();
            aPipeLayout->Create (aDevice, aVkDesSetLayout);
            aPipe = new Vulkan_Pipeline();
            aPipe->Create (aDevice, aRenderPass, aPipeLayout, aShaderVert, aShaderFrag, aSurfSize);
          }
          vkCmdBindPipeline (aVkCmdBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, aPipe->Pipeline());

          vkCmdBindDescriptorSets (aVkCmdBuffer,
                                   VK_PIPELINE_BIND_POINT_GRAPHICS,
                                   aPipe->PipelineLayout()->PipelineLayout(),
                                   0, 1, &aVkDescriptorSets[anImgIter], 0, NULL);

          /**static Handle(Vulkan_Buffer) aVertBuffer;
          if (aVertBuffer.IsNull())
          {
            const Graphic3d_Vec3 aVertData[3*2] =
            {
              Graphic3d_Vec3 (-0.5f,  0.5f, 0.0f), Graphic3d_Vec3(),
              Graphic3d_Vec3 ( 0.5f,  0.5f, 0.0f), Graphic3d_Vec3(),
              Graphic3d_Vec3 ( 0.0f, -0.5f, 0.0f), Graphic3d_Vec3()
            };

            aVertBuffer = new Vulkan_Buffer();
            aVertBuffer->Init (aDevice, aVertData, 3);
          }

          {
            std::vector<VkBuffer> aVkVertBuffers = { aVertBuffer->Buffer() };
            std::vector<VkDeviceSize> aVkVertOffsets = {0};
            vkCmdBindVertexBuffers (aVkCmdBuffer, 0, 1, aVkVertBuffers.data(), aVkVertOffsets.data());
            vkCmdDraw (aVkCmdBuffer, 3, 1, 0, 0);
          }*/

          for (Graphic3d_ArrayOfIndexedMapOfStructure::Iterator aPriorIter (myLayer.ArrayOfStructures()); aPriorIter.More(); aPriorIter.Next())
          {
            for (Vulkan_Structure::StructIterator aStructIter (aPriorIter.Value()); aStructIter.More(); aStructIter.Next())
            {
              const Vulkan_Structure* aStruct = aStructIter.Value();
              for (Vulkan_Structure::GroupIterator aGroupIter (aStruct->Groups()); aGroupIter.More(); aGroupIter.Next())
              {
                Vulkan_Group* aGroup = aGroupIter.ChangeValue();
                for (Vulkan_ListOfElements::Iterator aPrimIter (aGroup->myElements); aPrimIter.More(); aPrimIter.Next())
                {
                  Vulkan_PrimitiveArray& aPrim = aPrimIter.ChangeValue();
                  if (aPrim.VboAttribs.IsNull()
                  && !aPrim.Attribs.IsNull())
                  {
                    aPrim.VboAttribs = new Vulkan_Buffer();
                    aPrim.VboAttribs->init (aDevice, aPrim.Attribs->Data(), aPrim.Attribs->Size(), Vulkan_BufferType_Vertex);
                  }
                  if (aPrim.VboIndices.IsNull()
                  && !aPrim.Indices.IsNull())
                  {
                    aPrim.VboIndices = new Vulkan_Buffer();
                    aPrim.VboIndices->init (aDevice, aPrim.Indices->Data(), aPrim.Indices->Size(), Vulkan_BufferType_Index);
                  }
                  if (!aPrim.VboAttribs.IsNull())
                  {
                    VkBuffer aVkVertBuffers[1] = { aPrim.VboAttribs->Buffer() };
                    VkDeviceSize aVkVertOffsets[1] = {0};
                    vkCmdBindVertexBuffers (aVkCmdBuffer, 0, 1, aVkVertBuffers, aVkVertOffsets);
                    if (!aPrim.VboIndices.IsNull())
                    {
                      vkCmdBindIndexBuffer (aVkCmdBuffer, aPrim.VboIndices->Buffer(), 0, aPrim.Indices->Stride == 4 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
                      vkCmdDrawIndexed (aVkCmdBuffer, aPrim.Indices->NbElements, 1, 0, 0, 0);
                    }
                    else
                    {
                      vkCmdDraw (aVkCmdBuffer, aPrim.Attribs->NbElements, 1, 0, 0);
                    }
                  }
                }
              }
            }
          }
        }
        vkCmdEndRenderPass (aVkCmdBuffer);

        VkImageMemoryBarrier aVkImgMemBarrier;
        aVkImgMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        aVkImgMemBarrier.pNext = NULL;
        aVkImgMemBarrier.srcAccessMask = 0;
        aVkImgMemBarrier.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
        aVkImgMemBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        aVkImgMemBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        aVkImgMemBarrier.srcQueueFamilyIndex = 0;
        aVkImgMemBarrier.dstQueueFamilyIndex = 0;
        aVkImgMemBarrier.image = aVkImage;
        aVkImgMemBarrier.subresourceRange = {VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        vkCmdPipelineBarrier (aVkCmdBuffer,
                              VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                              VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                              0,
                              0, NULL,
                              0, NULL,
                              1, &aVkImgMemBarrier);
      }
      vkEndCommandBuffer (aVkCmdBuffer);
    }
  }

///
  uint32_t aVKSwapChainIndex = 0;
	if (!mySurface->AcquireNextImage (aDevice, aVKSwapChainIndex))
	{
    return;
	}

  const VkCommandBuffer aVkCmdBuffer = aVkCmdBuffers[aVKSwapChainIndex];
	const VkPipelineStageFlags aVkWaitMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VkSubmitInfo aVkSubmitInfo;
  aVkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  aVkSubmitInfo.pNext = NULL;
  aVkSubmitInfo.waitSemaphoreCount = 0;
  aVkSubmitInfo.pWaitSemaphores = NULL;
  aVkSubmitInfo.pWaitDstStageMask = &aVkWaitMask;
  aVkSubmitInfo.commandBufferCount = 1;
  aVkSubmitInfo.pCommandBuffers = &aVkCmdBuffer;
  aVkSubmitInfo.signalSemaphoreCount = 0;
  aVkSubmitInfo.pSignalSemaphores = NULL;

  VkResult aRes = vkQueueSubmit (aVkQueue, 1, &aVkSubmitInfo, VK_NULL_HANDLE);
	if (aRes != VkResult::VK_SUCCESS)
	{
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Window, failed to submit command buffer: ") + Vulkan_Device::FormatVkError (aRes));
    return;
	}

  aRes = vkQueueWaitIdle (aVkQueue);
	if (aRes != VkResult::VK_SUCCESS)
	{
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Window, failed to wait for queuer: ") + Vulkan_Device::FormatVkError (aRes));
    return;
	}

  const VkSwapchainKHR aSwapChain = mySurface->SwapChain();
	VkResult aResults = VkResult::VK_SUCCESS;
	VkPresentInfoKHR aVkPresentInfo;
  aVkPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  aVkPresentInfo.pNext = NULL;
  aVkPresentInfo.waitSemaphoreCount = 0;
  aVkPresentInfo.pWaitSemaphores = NULL;
  aVkPresentInfo.swapchainCount = 1;
  aVkPresentInfo.pSwapchains = &aSwapChain;
  aVkPresentInfo.pImageIndices = &aVKSwapChainIndex;
  aVkPresentInfo.pResults = &aResults;

  aRes = vkQueuePresentKHR (aVkQueue, &aVkPresentInfo);
	if (aRes != VkResult::VK_SUCCESS
   || aResults != VkResult::VK_SUCCESS)
	{
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Window, failed to present swapchain: ")
                                     + Vulkan_Device::FormatVkError (aRes != VkResult::VK_SUCCESS ? aRes : aResults));
    return;
	}
}

//=======================================================================
//function : RedrawImmediate
//purpose  :
//=======================================================================
void Vulkan_View::RedrawImmediate()
{
  Redraw();
}

//=======================================================================
//function : Invalidate
//purpose  :
//=======================================================================
void Vulkan_View::Invalidate()
{
  //
}

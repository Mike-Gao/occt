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

#if defined(_WIN32)
  #include <windows.h>

  #define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <Vulkan_Surface.hxx>

#include <Aspect_Window.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Vulkan_Device.hxx>
#include <Vulkan_Fence.hxx>

#include <vulkan/vulkan.h>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_Surface, Standard_Transient)

// =======================================================================
// function : Vulkan_Surface
// purpose  :
// =======================================================================
Vulkan_Surface::Vulkan_Surface()
: mySwapFence (new Vulkan_Fence()),
  myVkSurface (NULL),
  myVkSwapChain (NULL),
  myVkFormat (new VkSurfaceFormatKHR())
{
  //
}

// =======================================================================
// function : ~Vulkan_Surface
// purpose  :
// =======================================================================
Vulkan_Surface::~Vulkan_Surface()
{
  Release (Handle(Vulkan_Device)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_Surface::Release (const Handle(Vulkan_Device)& theDevice)
{
  mySwapFence->Release (theDevice);
  for (size_t anImgIter = 0; anImgIter < myVkImageViews.size(); ++anImgIter)
  {
    VkImageView& aVkImageView = myVkImageViews[anImgIter];
    if (aVkImageView != NULL)
    {
      Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_Surface destroyed without Vulkan context",);
      vkDestroyImageView (theDevice->Device(), aVkImageView, theDevice->HostAllocator());
      aVkImageView = NULL;
    }
  }
  myVkImages.clear();
  myVkImageViews.clear();
  if (myVkSwapChain != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_Surface destroyed without Vulkan context",);
    vkDestroySwapchainKHR (theDevice->Device(), myVkSwapChain, theDevice->HostAllocator());
    myVkSwapChain = NULL;
  }
  if (myVkSurface != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_Surface destroyed without Vulkan context",);
    vkDestroySurfaceKHR (theDevice->Instance(), myVkSurface, theDevice->HostAllocator());
    myVkSurface = NULL;
  }
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool Vulkan_Surface::Init (const Handle(Vulkan_Device)& theDevice,
                           const Handle(Aspect_Window)& theWindow)
{
  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL
   || theWindow.IsNull())
  {
    return false;
  }

  if (!mySwapFence->Create (theDevice))
  {
    return false;
  }

#if defined(_WIN32)
  VkWin32SurfaceCreateInfoKHR aVkWin32SurfInfo;
  aVkWin32SurfInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  aVkWin32SurfInfo.pNext = NULL;
  aVkWin32SurfInfo.flags = 0;
  aVkWin32SurfInfo.hinstance = GetModuleHandleW (NULL);
  aVkWin32SurfInfo.hwnd = (HWND )theWindow->NativeHandle();

  VkResult aRes = vkCreateWin32SurfaceKHR (theDevice->Instance(), &aVkWin32SurfInfo, theDevice->HostAllocator(), &myVkSurface);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, unable to create surface: ") + Vulkan_Device::FormatVkError (aRes));
    Release (theDevice);
    return false;
  }
#else
  int NOT_IMPLEMENTED = 0;
  return false;
#endif

  VkBool32 isSupported = VK_FALSE;
  aRes = vkGetPhysicalDeviceSurfaceSupportKHR (theDevice->PhysicalDevice(), 0, myVkSurface, &isSupported);
  if (aRes != VkResult::VK_SUCCESS || isSupported == VK_FALSE)
  {
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, surface not supported: ") + Vulkan_Device::FormatVkError (aRes));
    }
    else
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, surface not supported"));
    }
    Release (theDevice);
    return false;
  }
  
  VkSurfaceCapabilitiesKHR aVkSurfCaps;
  aRes = vkGetPhysicalDeviceSurfaceCapabilitiesKHR (theDevice->PhysicalDevice(), myVkSurface, &aVkSurfCaps);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, failed to get surface capabilities: ") + Vulkan_Device::FormatVkError (aRes));
    Release (theDevice);
    return false;
  }

  if (!findFormat (theDevice, *myVkFormat))
  {
    Release (theDevice);
    return false;
  }

  {
    VkSwapchainCreateInfoKHR aSwapChainInfo;
    aSwapChainInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    aSwapChainInfo.pNext = NULL;
    aSwapChainInfo.flags = 0;
    aSwapChainInfo.surface = myVkSurface;
    aSwapChainInfo.minImageCount = 2;
    aSwapChainInfo.imageFormat = myVkFormat->format;
    aSwapChainInfo.imageColorSpace = myVkFormat->colorSpace;
    aSwapChainInfo.imageExtent = VkExtent2D { aVkSurfCaps.currentExtent.width, aVkSurfCaps.currentExtent.height };
    aSwapChainInfo.imageArrayLayers = 1;
    aSwapChainInfo.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    aSwapChainInfo.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
    aSwapChainInfo.queueFamilyIndexCount = 0;
    aSwapChainInfo.pQueueFamilyIndices = NULL;
    aSwapChainInfo.preTransform = VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    aSwapChainInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    aSwapChainInfo.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
    aSwapChainInfo.clipped = VK_TRUE;
    aSwapChainInfo.oldSwapchain = VK_NULL_HANDLE;

    aRes = vkCreateSwapchainKHR (theDevice->Device(), &aSwapChainInfo, theDevice->HostAllocator(), &myVkSwapChain);
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, failed to create swapchain: ") + Vulkan_Device::FormatVkError (aRes));
      Release (theDevice);
      return false;
    }
  }

  {
    uint32_t aNbSwapChainImages = 0;
    aRes = vkGetSwapchainImagesKHR (theDevice->Device(), myVkSwapChain, &aNbSwapChainImages, NULL);
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, failed to get swapchain images count: ") + Vulkan_Device::FormatVkError (aRes));
      return false;
    }

    myVkImages.resize (aNbSwapChainImages, NULL);
    aRes = vkGetSwapchainImagesKHR (theDevice->Device(), myVkSwapChain, &aNbSwapChainImages, myVkImages.data());
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, failed to get swapchain images: ") + Vulkan_Device::FormatVkError (aRes));
      Release (theDevice);
      return false;
    }
  }

  myVkImageViews.resize (myVkImages.size(), NULL);
  for (uint32_t anImgIter = 0; anImgIter < myVkImages.size(); ++anImgIter)
  {
    VkImageView& aVkImageView = myVkImageViews[anImgIter];

    VkImageViewCreateInfo aVkImgViewInfo;
    aVkImgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    aVkImgViewInfo.pNext = NULL;
    aVkImgViewInfo.flags = 0;
    aVkImgViewInfo.image = myVkImages[anImgIter];
    aVkImgViewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
    aVkImgViewInfo.format = myVkFormat->format;
    aVkImgViewInfo.components =
    {
      VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R,
      VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G,
      VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B,
      VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A
    };
    aVkImgViewInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
    aVkImgViewInfo.subresourceRange.baseMipLevel = 0;
    aVkImgViewInfo.subresourceRange.levelCount = 1;
    aVkImgViewInfo.subresourceRange.baseArrayLayer = 0;
    aVkImgViewInfo.subresourceRange.layerCount = 1;

    aRes = vkCreateImageView (theDevice->Device(), &aVkImgViewInfo, theDevice->HostAllocator(), &aVkImageView);
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, failed to create image view: ") + Vulkan_Device::FormatVkError (aRes));
      Release (theDevice);
      return false;
    }
  }

  return true;
}

// =======================================================================
// function : findFormat
// purpose  :
// =======================================================================
bool Vulkan_Surface::findFormat (const Handle(Vulkan_Device)& theDevice,
                                 VkSurfaceFormatKHR& theFormat)
{
  if (myVkSurface == NULL)
  {
    return false;
  }

  uint32_t aNbSurfFormats = 0;
  VkResult aRes = vkGetPhysicalDeviceSurfaceFormatsKHR (theDevice->PhysicalDevice(), myVkSurface, &aNbSurfFormats, NULL);
  if (aRes != VkResult::VK_SUCCESS
   || aNbSurfFormats == 0)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, failed to get surface formats count: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  std::vector<VkSurfaceFormatKHR> aVkSurfFormats (aNbSurfFormats);
  aRes = vkGetPhysicalDeviceSurfaceFormatsKHR (theDevice->PhysicalDevice(), myVkSurface, &aNbSurfFormats, aVkSurfFormats.data());
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, failed to get surface formats: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  theFormat = aVkSurfFormats[0];
  return true;
}

// =======================================================================
// function : CurrentSize
// purpose  :
// =======================================================================
Graphic3d_Vec2u Vulkan_Surface::CurrentSize (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkSurface == NULL)
  {
    return Graphic3d_Vec2u (0, 0);
  }

  VkSurfaceCapabilitiesKHR aVkSurfCaps;
  VkResult aRes = vkGetPhysicalDeviceSurfaceCapabilitiesKHR (theDevice->PhysicalDevice(), myVkSurface, &aVkSurfCaps);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Window, failed to get surface capabilities: ") + Vulkan_Device::FormatVkError (aRes));
    return Graphic3d_Vec2u (0, 0);
  }

  return Graphic3d_Vec2u (aVkSurfCaps.currentExtent.width, aVkSurfCaps.currentExtent.height);
}

// =======================================================================
// function : AcquireNextImage
// purpose  :
// =======================================================================
bool Vulkan_Surface::AcquireNextImage (const Handle(Vulkan_Device)& theDevice,
                                       uint32_t& theSwapChainIndex)
{
  theSwapChainIndex = 0;
  VkResult aRes = vkAcquireNextImageKHR (theDevice->Device(), myVkSwapChain, UINT64_MAX, VK_NULL_HANDLE, mySwapFence->Fence(), &theSwapChainIndex);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Surface, failed to get next swapchain image: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  return mySwapFence->Wait (theDevice)
      && mySwapFence->Reset(theDevice);
}

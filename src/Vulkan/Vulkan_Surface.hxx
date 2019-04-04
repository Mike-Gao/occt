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

#ifndef _Vulkan_Surface_HeaderFile
#define _Vulkan_Surface_HeaderFile

#include <Graphic3d_Vec2.hxx>
#include <Vulkan_ForwardDecl.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <memory>
#include <vector>

class Aspect_Window;
class Vulkan_Device;
class Vulkan_Fence;

//! This class defines an Vulkan surface.
class Vulkan_Surface : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Vulkan_Surface, Standard_Transient)
public:

  //! Constructor.
  Standard_EXPORT Vulkan_Surface();

  //! Destructor.
  Standard_EXPORT virtual ~Vulkan_Surface();

  //! Release default context.
  Standard_EXPORT void Release (const Handle(Vulkan_Device)& theDevice);

  //! Perform initialization of default context.
  Standard_EXPORT bool Init (const Handle(Vulkan_Device)& theDevice,
                             const Handle(Aspect_Window)& theWindow);

  //! Return surface.
  VkSurfaceKHR Surface() const { return myVkSurface; }

  //! Return swap chain.
  VkSwapchainKHR SwapChain() const { return myVkSwapChain; }

  //! Return the swap chain length.
  uint32_t SwapChainSize() const { return (uint32_t )myVkImageViews.size(); }

  //! Return images within swap chain.
  const std::vector<VkImage>& Images() const { return myVkImages; }

  //! Return image views within swap chain.
  const std::vector<VkImageView>& ImageViews() const { return myVkImageViews; }

  //! Return surface format.
  const VkSurfaceFormatKHR& SurfaceFormat() const { return *myVkFormat; }

  //! Fetch actual surface size.
  Graphic3d_Vec2u CurrentSize (const Handle(Vulkan_Device)& theDevice);

  //! Acquire next image from swap chain, @sa vkAcquireNextImageKHR().
  Standard_EXPORT bool AcquireNextImage (const Handle(Vulkan_Device)& theDevice,
                                         uint32_t& theSwapChainIndex);

protected:

  //! Find supported format.
  Standard_EXPORT bool findFormat (const Handle(Vulkan_Device)& theDevice,
                                   VkSurfaceFormatKHR& theFormat);

protected:

  Handle(Vulkan_Fence) mySwapFence;
  VkSurfaceKHR   myVkSurface;
  VkSwapchainKHR myVkSwapChain;
  std::vector<VkImage>     myVkImages;
  std::vector<VkImageView> myVkImageViews;
  std::shared_ptr<VkSurfaceFormatKHR> myVkFormat;

};

#endif // _Vulkan_Surface_HeaderFile

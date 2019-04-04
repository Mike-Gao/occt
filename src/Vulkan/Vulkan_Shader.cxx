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

#include <Vulkan_Shader.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_OpenFile.hxx>
#include <Vulkan_Device.hxx>

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_Shader, Standard_Transient)

// =======================================================================
// function : Vulkan_Shader
// purpose  :
// =======================================================================
Vulkan_Shader::Vulkan_Shader()
: myVkShader (NULL)
{
  //
}

// =======================================================================
// function : ~Vulkan_Shader
// purpose  :
// =======================================================================
Vulkan_Shader::~Vulkan_Shader()
{
  Release (Handle(Vulkan_Device)());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_Shader::Release (const Handle(Vulkan_Device)& theDevice)
{
  if (myVkShader != NULL)
  {
    Standard_ASSERT_RETURN (!theDevice.IsNull(), "Vulkan_Shader destroyed without Vulkan context",);
    vkDestroyShaderModule (theDevice->Device(), myVkShader, theDevice->HostAllocator());
    myVkShader = NULL;
  }
}

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
bool Vulkan_Shader::Create (const Handle(Vulkan_Device)& theDevice,
                            const uint32_t* theCode,
                            uint32_t theNbBytes)
{
  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL)
  {
    return false;
  }

  VkShaderModuleCreateInfo aVkShaderInfo;
  aVkShaderInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  aVkShaderInfo.pNext = NULL;
  aVkShaderInfo.flags = 0;
  aVkShaderInfo.pCode = theCode;
  aVkShaderInfo.codeSize = theNbBytes;

  VkResult aRes = vkCreateShaderModule (theDevice->Device(), &aVkShaderInfo, theDevice->HostAllocator(), &myVkShader);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Shader, failed to create shader: ") + Vulkan_Device::FormatVkError (aRes));
    return false;
  }

  return true;
}

namespace
{
  //! File sentry.
  struct FileSentry
  {
    FILE* File;

    operator FILE*() const { return File; }

    FileSentry (const TCollection_AsciiString& theFilePath)
    : File (OSD_OpenFile (theFilePath.ToCString(), "rb")) {}
    
    ~FileSentry()
    {
      if (File != NULL)
      {
        fclose (File);
      }
    }
  };
}

// =======================================================================
// function : CreateFromFile
// purpose  :
// =======================================================================
bool Vulkan_Shader::CreateFromFile (const Handle(Vulkan_Device)& theDevice,
                                    const TCollection_AsciiString& theFilePath)
{
  Release (theDevice);
  if (theDevice.IsNull()
   || theDevice->Device() == NULL)
  {
    return false;
  }

  FileSentry aFile (theFilePath.ToCString());
  if (aFile.File == NULL)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Shader, unable to open file '") + theFilePath + "'");
    return false;
  }

  uint32_t aFileLen = 0;
  if (fseek (aFile, 0, SEEK_END) == 0)
  {
    aFileLen = (uint32_t )ftell (aFile);
    if (fseek (aFile, 0, SEEK_SET) != 0)
    {
      aFileLen = 0;
    }
  }
  if (aFileLen % 4 != 0
   || aFileLen == 0)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Shader, file '") + theFilePath + "' has wrong length " + (int )aFileLen);
    return false;
  }

  std::vector<uint32_t> aCode (aFileLen / 4);
  if (fread (aCode.data(), 1, aFileLen, aFile) != aFileLen)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Shader, unable to read file '") + theFilePath + "'");
    return false;
  }
  
  return Create (theDevice, aCode.data(), aFileLen);
}

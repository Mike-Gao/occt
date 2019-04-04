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

#include <Vulkan_Device.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_Map.hxx>
#include <Vulkan_Caps.hxx>
#include <Vulkan_FrameStats.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

#include <vulkan/vulkan.h>

#include <vector>

IMPLEMENT_STANDARD_RTTIEXT(Vulkan_Device, Standard_Transient)

namespace
{
  //! Return object type.
  static TCollection_AsciiString debugVkObjectType (VkDebugReportObjectTypeEXT theObjectType)
  {
    switch (theObjectType)
    {
      case VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT:
        return "UNKNOWN";
      case VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT:
        return "Instance";
      case VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT:
        return "Physical device";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT:
        return "Device";
      case VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT:
        return "Queue";
      case VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT:
        return "Semaphore";
      case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT:
        return "Command Buffer";
      case VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT:
        return "Fence";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT:
        return "Device memory";
      case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT:
        return "Buffer";
      case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT:
        return "Image";
      case VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT:
        return "Event";
      case VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT:
        return "Query pool";
      case VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT:
        return "Buffer view";
      case VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT:
        return "Image view";
      case VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT:
        return "Shader module";
      case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT:
        return "Pipeline cache";
      case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT:
        return "Pipeline layout";
      case VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT:
        return "Render pass";
      case VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT:
        return "Pipeline";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT:
        return "Descriptor set layout";
      case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT:
        return "Sampler";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT:
        return "Descriptor pool";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT:
        return "Descriptor set";
      case VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT:
        return "Framebuffer";
      case VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT:
        return "Command pool";
      case VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT:
        return "Surface";
      case VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT:
        return "Swapchain";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT_EXT:
        return "Debug report";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_KHR_EXT:
        return "Display";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DISPLAY_MODE_KHR_EXT:
        return "Display mode";
      case VK_DEBUG_REPORT_OBJECT_TYPE_OBJECT_TABLE_NVX_EXT:
        return "Object table";
      case VK_DEBUG_REPORT_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX_EXT:
        return "Indirect commands layout";
      case VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT_EXT:
        return "Validation cache";
      case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_EXT:
        return "Sampler YCBCR conversion";
      case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_EXT:
        return "Descriptor update template";
      case VK_DEBUG_REPORT_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV_EXT:
        return "Acceleration structure";
      default:
        return TCollection_AsciiString ("Unknown #") + (int )theObjectType;
    }
  }

  //! Format physical device type.
  static const char* formatVkDeviceType (VkPhysicalDeviceType theType)
  {
    switch (theType)
    {
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "Discrete GPU";
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "Virtual GPU";
      case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "CPU";
      default:
        return "UNKNOWN";
    }
  }

  //! Return physical device priority.
  static int fastestVkDeviceType (VkPhysicalDeviceType theType)
  {
    switch (theType)
    {
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return 2;
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return 3;
      case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return 1;
      case VK_PHYSICAL_DEVICE_TYPE_CPU:            return 0;
      default:
        return 0;
    }
  }

  //! Return message type.
  static const char* debugVkMessageType (VkDebugReportFlagsEXT theFlags)
  {
    if ((theFlags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0)
    {
      return "Error";
    }
    if ((theFlags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0)
    {
      return "Warning";
    }
    if ((theFlags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0)
    {
      return "Info";
    }
    if ((theFlags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) != 0)
    {
      return "Performance";
    }
    if ((theFlags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0)
    {
      return "Debug";
    }
    return "";
  }

  //! Vulkan debug callback redirection to Message::DefaultMessenger().
  static VkBool32 VKAPI_CALL debugVkCallback (VkDebugReportFlagsEXT theFlags,
                                              VkDebugReportObjectTypeEXT theObjectType,
                                              uint64_t theObject,
                                              size_t   theLocation,
                                              int32_t  theMessageCode,
                                              const char* theLayerPrefix,
                                              const char* theMessage,
                                              void* theUserData)
  {
    (void )theUserData;
    (void )theObject;
    (void )theLocation;
    Message_Gravity aGrav = (theFlags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0
                          ? Message_Alarm
                          : ((theFlags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0
                           ? Message_Warning
                           : Message_Info);
    const TCollection_AsciiString aLayer = theLayerPrefix;
    if (aLayer == "Loader Message"
     && theObjectType == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT
     && (theFlags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0)
    {
      return VK_FALSE;
    }

    TCollection_AsciiString aMsg;
    aMsg += "TKVulkan.";        aMsg += aLayer;
    aMsg += " | Type: ";        aMsg += debugVkMessageType (theFlags);
    aMsg += " | ID: ";          aMsg += theMessageCode;
    //aMsg += " | Location: ";    aMsg += (int )theLocation;
    aMsg += " | Object: ";      aMsg += debugVkObjectType (theObjectType);
    aMsg += " | Message:\n  ";
    aMsg += theMessage;
    Message::DefaultMessenger()->Send (aMsg, aGrav);
    return VK_FALSE;
  }

  //! Add key-value pair to the dictionary.
  static void addInfo (TColStd_IndexedDataMapOfStringString& theDict,
                       const TCollection_AsciiString& theKey,
                       const TCollection_AsciiString& theValue)
  {
    theDict.ChangeFromIndex (theDict.Add (theKey, theValue)) = theValue;
  }
}

// =======================================================================
// function : FormatVkError
// purpose  :
// =======================================================================
TCollection_AsciiString Vulkan_Device::FormatVkError (int theErr)
{
  switch (theErr)
  {
    case VK_SUCCESS:
      return "Success";
    case VK_NOT_READY:
      return "Not ready";
    case VK_TIMEOUT:
      return "Timeout";
    case VK_EVENT_SET:
      return "Event set";
    case VK_EVENT_RESET:
      return "Event reset";
    case VK_INCOMPLETE:
      return "Incomplete";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return "Error, out of host memory";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      return "Error, out of device memory";
    case VK_ERROR_INITIALIZATION_FAILED:
      return "Error, initialization failed";
    case VK_ERROR_DEVICE_LOST:
      return "Error, device lost";
    case VK_ERROR_MEMORY_MAP_FAILED:
      return "Error, memory map failed";
    case VK_ERROR_LAYER_NOT_PRESENT:
      return "Error, layer not present";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      return "Error, extension not present";
    case VK_ERROR_FEATURE_NOT_PRESENT:
      return "Error, feature not present";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      return "Error, incompatible driver";
    case VK_ERROR_TOO_MANY_OBJECTS:
      return "Error, too many objects";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      return "Error, format not supported";
    case VK_ERROR_FRAGMENTED_POOL:
      return "Error, fragmented pool";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      return "Error, out of pool memory";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
      return "Error, invalid external handle";
    case VK_ERROR_SURFACE_LOST_KHR:
      return "Error, surface lost";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
      return "Error, native window in use";
    case VK_SUBOPTIMAL_KHR:
      return "Error, suboptimal";
    case VK_ERROR_OUT_OF_DATE_KHR:
      return "Error, out of date";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
      return "Error, incompatible display";
    case VK_ERROR_VALIDATION_FAILED_EXT:
      return "Error, validation failed";
    case VK_ERROR_INVALID_SHADER_NV:
      return "Error, invalid shader";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
      return "Error, invalid DRM format";
    case VK_ERROR_FRAGMENTATION_EXT:
      return "Error, fragmentation";
    case VK_ERROR_NOT_PERMITTED_EXT:
      return "Error, not permitted";
    case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
      return "Error, invalid device address";
    default:
      return TCollection_AsciiString("Error #") + theErr;
  }
}

// =======================================================================
// function : Vulkan_Device
// purpose  :
// =======================================================================
Vulkan_Device::Vulkan_Device (const TCollection_AsciiString& theAppName,
                              const uint32_t theAppVersion,
                              const TCollection_AsciiString& theEngineName,
                              const uint32_t theEngineVersion)
: myAppName (theAppName),
  myEngineName (theEngineName),
  myAppVersion (theAppVersion),
  myEngineVersion (theEngineVersion),
  myVkInstance (NULL),
  myVkDevice (NULL),
  myVkDeviceMemory (new VkPhysicalDeviceMemoryProperties()),
  myFrameStats (new Vulkan_FrameStats()),
  myIsOwnContext (false)
{
  memset (myVkDeviceMemory.get(), 0, sizeof(VkPhysicalDeviceMemoryProperties));
}

// =======================================================================
// function : ~Vulkan_Device
// purpose  :
// =======================================================================
Vulkan_Device::~Vulkan_Device()
{
  Release();
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Vulkan_Device::Release()
{
  myIsOwnContext = Standard_False;
  memset (myVkDeviceMemory.get(), 0, sizeof(VkPhysicalDeviceMemoryProperties));
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool Vulkan_Device::Init (const Handle(Vulkan_Caps)& theCaps)
{
  Release();
  myIsOwnContext = true;

  Handle(Vulkan_Caps) aCaps = theCaps;
  if (aCaps.IsNull())
  {
    aCaps = new Vulkan_Caps();
  }

  NCollection_Map<TCollection_AsciiString, TCollection_AsciiString> anAllLayers;
  {
    uint32_t aNbLayers = 0;
    if (vkEnumerateInstanceLayerProperties (&aNbLayers, NULL) == VK_SUCCESS
     && aNbLayers != 0)
    {
      std::vector<VkLayerProperties> aLayers (aNbLayers);
      if (vkEnumerateInstanceLayerProperties (&aNbLayers, aLayers.data()) == VK_SUCCESS)
      {
        for (uint32_t aLayerIter = 0; aLayerIter < aNbLayers; ++aLayerIter)
        {
          const VkLayerProperties& aLayerProps = aLayers[aLayerIter];
          anAllLayers.Add (aLayerProps.layerName);
        }
      }
    }
  }

  NCollection_Map<TCollection_AsciiString, TCollection_AsciiString> anAllInstExtensions;
  {
    uint32_t aNbExts = 0;
    if (vkEnumerateInstanceExtensionProperties (NULL, &aNbExts, NULL) == VK_SUCCESS
     && aNbExts != 0)
    {
      std::vector<VkExtensionProperties> anExts (aNbExts);
      if (vkEnumerateInstanceExtensionProperties (NULL, &aNbExts, anExts.data()) == VK_SUCCESS)
      {
        for (uint32_t anExtIter = 0; anExtIter < aNbExts; ++anExtIter)
        {
          const VkExtensionProperties& anExtProps = anExts[anExtIter];
          anAllInstExtensions.Add (anExtProps.extensionName);
        }
      }
    }
  }

  // layers should be the same for instance and for device, while extensions may differ
  std::vector<const char*> aLayers, anInstExtensions;
  anInstExtensions.push_back (VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
  anInstExtensions.push_back (VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
  if (aCaps->contextDebug)
  {
    if (anAllInstExtensions.Contains (VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
    {
      anInstExtensions.push_back (VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    // VK_LAYER_LUNARG_standard_validation is a meta layer enabling several standard validation layers (LunarG SDK):
    // - VK_LAYER_GOOGLE_threading, VK_LAYER_GOOGLE_unique_objects,
    //   VK_LAYER_LUNARG_parameter_validation, VK_LAYER_LUNARG_core_validation,
    //   VK_LAYER_LUNARG_device_limits, VK_LAYER_LUNARG_image,
    //   VK_LAYER_LUNARG_object_tracker, VK_LAYER_LUNARG_swapchain
    if (anAllLayers.Contains ("VK_LAYER_LUNARG_standard_validation"))
    {
      aLayers.push_back ("VK_LAYER_LUNARG_standard_validation");
    }
    //aLayers.push_back ("VK_LAYER_GOOGLE_threading"); // validate usage from multiple threads
    //aLayers.push_back ("VK_LAYER_GOOGLE_unique_objects");
    //aLayers.push_back ("VK_LAYER_LUNARG_parameter_validation"); // validate API parameter values
    //aLayers.push_back ("VK_LAYER_LUNARG_core_validation");
    //aLayers.push_back ("VK_LAYER_LUNARG_device_limits"); // detects missing device limits checks
    //aLayers.push_back ("VK_LAYER_LUNARG_image"); // validate texture/render formats
    //aLayers.push_back ("VK_LAYER_LUNARG_object_tracker"); // detect usage of invalid objects
    //aLayers.push_back ("VK_LAYER_LUNARG_swapchain"); // validate usage of swapchain extensions

    // trace API calls
    //if (anAllLayers.Contains ("VK_LAYER_LUNARG_api_dump"))
    //{
    //  aLayers.push_back ("VK_LAYER_LUNARG_api_dump");
    //}
    // dump frames to image files
    //if (anAllLayers.Contains ("VK_LAYER_LUNARG_screenshot"))
    //{
    //  aLayers.push_back ("VK_LAYER_LUNARG_screenshot");
    //}
  }
  
  //aLayers.push_back ("VK_LAYER_NV_optimus");
  //aLayers.push_back ("VK_LAYER_VALVE_steam_overlay");

  VkApplicationInfo anAppInfo;
  anAppInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
  anAppInfo.pNext = NULL;
  anAppInfo.pApplicationName = myAppName.ToCString();
  anAppInfo.applicationVersion = myAppVersion;
  anAppInfo.pEngineName = myEngineName.ToCString();
  anAppInfo.engineVersion = myEngineVersion;
  anAppInfo.apiVersion = VK_MAKE_VERSION(1, 0, 21);

  VkInstanceCreateInfo anInstInfo;
  anInstInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  anInstInfo.pNext = NULL;
  anInstInfo.flags = 0;
  anInstInfo.pApplicationInfo = &anAppInfo;
  anInstInfo.enabledLayerCount   = (uint32_t )aLayers.size();
  anInstInfo.ppEnabledLayerNames = aLayers.data();
  anInstInfo.enabledExtensionCount   = (uint32_t )anInstExtensions.size();
  anInstInfo.ppEnabledExtensionNames = anInstExtensions.data();

  VkResult aRes = vkCreateInstance (&anInstInfo, NULL, &myVkInstance);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Device, unable creating Vulkan instance: ") + FormatVkError (aRes), Message_Fail);
    return false;
  }

  if (aCaps->contextDebug)
  {
    if (PFN_vkCreateDebugReportCallbackEXT aVkCreateDebugCallback = (PFN_vkCreateDebugReportCallbackEXT )vkGetInstanceProcAddr (myVkInstance, "vkCreateDebugReportCallbackEXT"))
    {
      VkDebugReportCallbackCreateInfoEXT aCallbackInfo;
      aCallbackInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
      aCallbackInfo.pNext = NULL;
      aCallbackInfo.flags = VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT
                          | VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT
                          | VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
                          | VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT
                          | VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT;
      aCallbackInfo.pfnCallback = debugVkCallback;
      aCallbackInfo.pUserData = NULL;

      VkDebugReportCallbackEXT aDebReportObj = NULL;
      aRes = aVkCreateDebugCallback (myVkInstance, &aCallbackInfo, NULL, &aDebReportObj);
      if (aRes != VkResult::VK_SUCCESS)
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Device, unable registering debug report callback: ") + FormatVkError (aRes), Message_Warning);
      }
    }
  }

  {
    uint32_t aNbDevices = 0;
    aRes = vkEnumeratePhysicalDevices (myVkInstance, &aNbDevices, NULL);
    if (aRes != VkResult::VK_SUCCESS
     || aNbDevices == 0)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Device, unable to list physical devices: ") + FormatVkError (aRes), Message_Warning);
      return false;
    }

    std::vector<VkPhysicalDevice> aDevices (aNbDevices);
    aRes = vkEnumeratePhysicalDevices (myVkInstance, &aNbDevices, aDevices.data());
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Device, unable to list physical devices: ") + FormatVkError (aRes), Message_Warning);
      return false;
    }
    myVkPhysDevice = aDevices[0];
    int aSelPriority = aCaps->contextNoAccel ? IntegerLast() : -IntegerLast();
    TCollection_AsciiString aDevNameToFind = aCaps->contextDevice;
    aDevNameToFind.LowerCase();
    if (aDevNameToFind.IsIntegerValue()
     && aDevNameToFind.IntegerValue() >= 0
     && aDevNameToFind.IntegerValue() < (int )aNbDevices)
    {
      myVkPhysDevice = aDevices[aDevNameToFind.IntegerValue()];
    }
    else
    {
      uint32_t aVendorToFind = 0;
      if (aDevNameToFind == "amd")
      {
        aVendorToFind = VendorId_AMD;
        aDevNameToFind.Clear();
      }
      else if (aDevNameToFind == "nvidia")
      {
        aVendorToFind = VendorId_NVIDIA;
        aDevNameToFind.Clear();
      }
      else if (aDevNameToFind == "intel")
      {
        aVendorToFind = VendorId_INTEL;
        aDevNameToFind.Clear();
      }

      for (uint32_t aDevIter = 0; aDevIter < aNbDevices; ++aDevIter)
      {
        const VkPhysicalDevice& aPhysDev = aDevices[aDevIter];

        VkPhysicalDeviceProperties aDevProps;
        memset (&aDevProps, 0, sizeof(aDevProps));
        vkGetPhysicalDeviceProperties (aPhysDev, &aDevProps);
        if (!aDevNameToFind.IsEmpty())
        {
          TCollection_AsciiString aName (aDevProps.deviceName);
          aName.LowerCase();
          if (aName.Search (aDevNameToFind) != -1)
          {
            myVkPhysDevice = aDevices[aDevIter];
            break;
          }
        }
        else if (aVendorToFind != 0
              && aDevProps.vendorID == aVendorToFind)
        {
          myVkPhysDevice = aDevices[aDevIter];
          break;
        }

        const int aPriority = fastestVkDeviceType (aDevProps.deviceType);
        if (aCaps->contextNoAccel)
        {
          if (aPriority < aSelPriority)
          {
            aSelPriority   = aPriority;
            myVkPhysDevice = aDevices[aDevIter];
          }
        }
        else
        {
          if (aPriority > aSelPriority)
          {
            aSelPriority   = aPriority;
            myVkPhysDevice = aDevices[aDevIter];
          }
        }
      }
    }
  }

  NCollection_Map<TCollection_AsciiString, TCollection_AsciiString> anAllDevExtensions;
  {
    uint32_t aNbExts = 0;
    if (vkEnumerateDeviceExtensionProperties (myVkPhysDevice, NULL, &aNbExts, NULL) == VK_SUCCESS
     && aNbExts != 0)
    {
      std::vector<VkExtensionProperties> anExts (aNbExts);
      if (vkEnumerateDeviceExtensionProperties (myVkPhysDevice, NULL, &aNbExts, anExts.data()) == VK_SUCCESS)
      {
        for (uint32_t anExtIter = 0; anExtIter < aNbExts; ++anExtIter)
        {
          const VkExtensionProperties& anExtProps = anExts[anExtIter];
          anAllDevExtensions.Add (anExtProps.extensionName);
        }
      }
    }
  }

  std::vector<const char*> aDevExtensions;
  aDevExtensions.push_back (VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  std::vector<VkQueueFamilyProperties> aPhysDevQueueFamilyProps;
  {
    uint32_t aNbProps = 0;
    vkGetPhysicalDeviceQueueFamilyProperties (myVkPhysDevice, &aNbProps, NULL);
    if (aNbProps != 0)
    {
      aPhysDevQueueFamilyProps.resize (aNbProps);
      vkGetPhysicalDeviceQueueFamilyProperties (myVkPhysDevice, &aNbProps, aPhysDevQueueFamilyProps.data());
    }
  }

  {
    std::vector<VkDeviceQueueCreateInfo> aDevQueueInfos (1);
    std::vector< std::vector<float> > aDevQueuesPriorities (aDevQueueInfos.size(), std::vector<float> (1, 0.0f));
    for (uint32_t aQueueIter = 0; aQueueIter < (uint32_t )aDevQueueInfos.size(); ++aQueueIter)
    {
      std::vector<float>& aDevQueuePriorities = aDevQueuesPriorities[aQueueIter];
      VkDeviceQueueCreateInfo& aDevQueueInfo = aDevQueueInfos[aQueueIter];
      aDevQueueInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      aDevQueueInfo.pNext = NULL;
      aDevQueueInfo.flags = 0;
      aDevQueueInfo.queueFamilyIndex = aQueueIter;
      aDevQueueInfo.queueCount       = (uint32_t )aDevQueuePriorities.size();
      aDevQueueInfo.pQueuePriorities = aDevQueuePriorities.data();
    }

    VkPhysicalDeviceFeatures aPhysDevFeatures;
    memset (&aPhysDevFeatures, 0, sizeof(aPhysDevFeatures));

    VkDeviceCreateInfo aDevInfo;
    aDevInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    aDevInfo.pNext = NULL;
    aDevInfo.flags = 0;
    aDevInfo.queueCreateInfoCount = (uint32_t )aDevQueueInfos.size();
    aDevInfo.pQueueCreateInfos    = aDevQueueInfos.data();
    aDevInfo.enabledLayerCount    = (uint32_t )aLayers.size();
    aDevInfo.ppEnabledLayerNames  = aLayers.data();
    aDevInfo.enabledExtensionCount   = (uint32_t )aDevExtensions.size();
    aDevInfo.ppEnabledExtensionNames = aDevExtensions.data();
    aDevInfo.pEnabledFeatures = &aPhysDevFeatures;
    aRes = vkCreateDevice (myVkPhysDevice, &aDevInfo, NULL, &myVkDevice);
    if (aRes != VkResult::VK_SUCCESS)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Device, unable to create device: ") + FormatVkError (aRes), Message_Warning);
      return false;
    }
  }

  vkGetPhysicalDeviceMemoryProperties (myVkPhysDevice, myVkDeviceMemory.get());
  return true;
}

// =======================================================================
// function : AllocateDeviceMemory
// purpose  :
// =======================================================================
VkDeviceMemory Vulkan_Device::AllocateDeviceMemory (const VkMemoryRequirements& theReqs)
{
  if (myVkInstance == NULL)
  {
    return NULL;
  }

  VkMemoryAllocateInfo aVkAllocInfo;
  aVkAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  aVkAllocInfo.pNext = NULL;
  aVkAllocInfo.allocationSize = theReqs.size;
  aVkAllocInfo.memoryTypeIndex = uint32_t(-1);
  for (uint32_t aTypeIter = 0; aTypeIter < myVkDeviceMemory->memoryTypeCount; ++aTypeIter)
  {
    const uint32_t aBit = (uint32_t(1) << aTypeIter);
    if ((theReqs.memoryTypeBits & aBit) != 0)
    {
      if ((myVkDeviceMemory->memoryTypes[aTypeIter].propertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
      {
        aVkAllocInfo.memoryTypeIndex = aTypeIter;
        break;
      }
    }
  }
  if (aVkAllocInfo.memoryTypeIndex == uint32_t(-1))
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Device, failed to get correct memory type"), Message_Fail);
    return NULL;
  }

  VkDeviceMemory aVkDevMem = NULL;
  VkResult aRes = vkAllocateMemory (myVkDevice, &aVkAllocInfo, myVkHostAllocator.get(), &aVkDevMem);
  if (aRes != VkResult::VK_SUCCESS)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Vulkan_Device, failed to allocate device memory [")
                                     + int(theReqs.size) + "]:" + FormatVkError (aRes), Message_Fail);
    return NULL;
  }
  return aVkDevMem;
}

// =======================================================================
// function : DiagnosticInformation
// purpose  :
// =======================================================================
void Vulkan_Device::DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                           Graphic3d_DiagnosticInfo theFlags) const
{
  if (myVkInstance == NULL)
  {
    return;
  }

  uint32_t aNbDevices = 0;
  if (vkEnumeratePhysicalDevices (myVkInstance, &aNbDevices, NULL) == VkResult::VK_SUCCESS
   && aNbDevices > 1)
  {
    std::vector<VkPhysicalDevice> aDevices (aNbDevices);
    if (vkEnumeratePhysicalDevices (myVkInstance, &aNbDevices, aDevices.data()) == VkResult::VK_SUCCESS)
    {
      TCollection_AsciiString aDevListStr;
      for (uint32_t aDevIter = 0; aDevIter < aNbDevices; ++aDevIter)
      {
        const VkPhysicalDevice& aPhysDev = aDevices[aDevIter];
        VkPhysicalDeviceProperties aDevProps;
        memset (&aDevProps, 0, sizeof(aDevProps));
        vkGetPhysicalDeviceProperties (aPhysDev, &aDevProps);
        aDevListStr += aDevProps.deviceName;
        aDevListStr += " ";
      }
      addInfo (theDict, "VKDeviceList", aDevListStr);
    }
  }

  if (myVkPhysDevice != NULL
   && (theFlags & Graphic3d_DiagnosticInfo_Device) != 0)
  {
    VkPhysicalDeviceProperties aDevProps;
    memset (&aDevProps, 0, sizeof(aDevProps));
    vkGetPhysicalDeviceProperties (myVkPhysDevice, &aDevProps);
    addInfo (theDict, "VKDevice",     aDevProps.deviceName);
    addInfo (theDict, "VKDeviceType", formatVkDeviceType (aDevProps.deviceType));

    TCollection_AsciiString aVendor (int(aDevProps.vendorID));
    switch ((int )aDevProps.vendorID)
    {
      case VendorId_AMD:    aVendor = "AMD";    break;
      case VendorId_NVIDIA: aVendor = "NVIDIA"; break;
      case VendorId_INTEL:  aVendor = "INTEL";  break;
    }
    addInfo (theDict, "VKDeviceVendor", aVendor);
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
  {
    TCollection_AsciiString aLayerStr;
    uint32_t aNbLayers = 0;
    if ((theFlags & Graphic3d_DiagnosticInfo_NativePlatform) != 0
     && vkEnumerateInstanceLayerProperties (&aNbLayers, NULL) == VK_SUCCESS
     && aNbLayers != 0)
    {
      std::vector<VkLayerProperties> aLayers (aNbLayers);
      if (vkEnumerateInstanceLayerProperties (&aNbLayers, aLayers.data()) == VK_SUCCESS)
      {
        for (uint32_t aLayerIter = 0; aLayerIter < aNbLayers; ++aLayerIter)
        {
          const VkLayerProperties& aLayerProps = aLayers[aLayerIter];
          aLayerStr += aLayerProps.layerName;
          aLayerStr += " ";
        }
        addInfo (theDict, "VKLayers", aLayerStr);
      }
    }

    TCollection_AsciiString anInstExtsStr;
    uint32_t aNbExts = 0;
    if ((theFlags & Graphic3d_DiagnosticInfo_NativePlatform) != 0
     && vkEnumerateInstanceExtensionProperties (NULL, &aNbExts, NULL) == VK_SUCCESS
     && aNbExts != 0)
    {
      std::vector<VkExtensionProperties> anExts (aNbExts);
      if (vkEnumerateInstanceExtensionProperties (NULL, &aNbExts, anExts.data()) == VK_SUCCESS)
      {
        for (uint32_t anExtIter = 0; anExtIter < aNbExts; ++anExtIter)
        {
          const VkExtensionProperties& anExtProps = anExts[anExtIter];
          anInstExtsStr += anExtProps.extensionName;
          anInstExtsStr += " ";
        }
        addInfo (theDict, "VKExtensions", anInstExtsStr);
      }
    }

    TCollection_AsciiString aDevExtsStr;
    if (myVkPhysDevice != NULL
     && (theFlags & Graphic3d_DiagnosticInfo_Device) != 0
     && vkEnumerateDeviceExtensionProperties (myVkPhysDevice, NULL, &aNbExts, NULL) == VK_SUCCESS
     && aNbExts != 0)
    {
      std::vector<VkExtensionProperties> anExts (aNbExts);
      if (vkEnumerateDeviceExtensionProperties (myVkPhysDevice, NULL, &aNbExts, anExts.data()) == VK_SUCCESS)
      {
        for (uint32_t anExtIter = 0; anExtIter < aNbExts; ++anExtIter)
        {
          const VkExtensionProperties& anExtProps = anExts[anExtIter];
          aDevExtsStr += anExtProps.extensionName;
          aDevExtsStr += " ";
        }
        addInfo (theDict, "VKDeviceExtensions", aDevExtsStr);
      }
    }
  }

  if (myVkPhysDevice != NULL
   && (theFlags & Graphic3d_DiagnosticInfo_Limits) != 0)
  {
    VkPhysicalDeviceProperties aProps;
    memset (&aProps, 0, sizeof(aProps));
    vkGetPhysicalDeviceProperties (myVkPhysDevice, &aProps);
    const VkPhysicalDeviceLimits& aLimits = aProps.limits;
    addInfo (theDict, "Max texture size", TCollection_AsciiString((int )aLimits.maxImageDimension2D));
    addInfo (theDict, "Max FBO dump size", TCollection_AsciiString() + int(aLimits.maxFramebufferWidth) + "x" + int(aLimits.maxFramebufferHeight));
  }

  if (myVkPhysDevice != NULL
   && (theFlags & Graphic3d_DiagnosticInfo_Memory) != 0)
  {
    uint64_t aDedicated = 0;
    int aNbDedicatedHeaps = 0;
    TCollection_AsciiString aDedicatedHeaps;
    TColStd_PackedMapOfInteger aDedicatedHeapsMap;
    for (uint32_t aTypeIter = 0; aTypeIter < myVkDeviceMemory->memoryTypeCount; ++aTypeIter)
    {
      const VkMemoryType& aMemInfo = myVkDeviceMemory->memoryTypes[aTypeIter];
      if ((aMemInfo.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == 0)
      {
        continue;
      }

      if (aDedicatedHeapsMap.Add (aMemInfo.heapIndex))
      {
        const VkMemoryHeap& aHeap = myVkDeviceMemory->memoryHeaps[aMemInfo.heapIndex];
        aDedicated += aHeap.size;
        if (!aDedicatedHeaps.IsEmpty())
        {
          aDedicatedHeaps += ", ";
        }
        aDedicatedHeaps += TCollection_AsciiString(int(aHeap.size / (1024 * 1024))) + " MiB";
        ++aNbDedicatedHeaps;
      }
    }
    if (aDedicated != 0)
    {
      addInfo (theDict, "GPU memory", TCollection_AsciiString() + int(aDedicated / (1024 * 1024)) + " MiB");
      if (aNbDedicatedHeaps > 1)
      {
        addInfo (theDict, "GPU heaps", aDedicatedHeaps);
      }
    }
  }
}

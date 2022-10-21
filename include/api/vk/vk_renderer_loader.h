#ifndef VK_RENDERER_LOADER_H
#define VK_RENDERER_LOADER_H

#define VK_FUNCTION(name) static PFN_##name name 

#define VK_Load_Global_Function(name) \
do \
{ \
name = (PFN_##name)vkGetInstanceProcAddr(NULL, #name); \
Bool_Warning_Check(name, Str8_Lit("Failed to load the global vulkan procedure %s"), #name); \
} while(0)

#define VK_Load_Instance_Function(name) \
do \
{ \
name = (PFN_##name)vkGetInstanceProcAddr(Instance, #name); \
Bool_Warning_Check(name, Str8_Lit("Failed to load the instance vulkan procedure %s"), #name); \
} while(0)

#define VK_Load_Device_Function(name) \
do \
{ \
name = (PFN_##name)vkGetDeviceProcAddr(Device, #name); \
Bool_Warning_Check(name, Str8_Lit("Failed to load the device vulkan procedure %s"), #name); \
} while(0)

//Platform-specific level functions
VK_FUNCTION(vkGetInstanceProcAddr);

//Global level functions
VK_FUNCTION(vkCreateInstance);
VK_FUNCTION(vkEnumerateInstanceExtensionProperties);
VK_FUNCTION(vkEnumerateInstanceLayerProperties);

//Instance level functions
VK_FUNCTION(vkDestroyInstance);
VK_FUNCTION(vkEnumeratePhysicalDevices);
VK_FUNCTION(vkEnumerateDeviceExtensionProperties);
VK_FUNCTION(vkGetPhysicalDeviceProperties);
VK_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
VK_FUNCTION(vkGetDeviceProcAddr);
VK_FUNCTION(vkCreateDevice);
VK_FUNCTION(vkDestroySurfaceKHR);
VK_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
VK_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
VK_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);

#ifdef _WIN32
VK_FUNCTION(vkCreateWin32SurfaceKHR);
#endif

//Device level functions
VK_FUNCTION(vkDestroyDevice);
VK_FUNCTION(vkGetDeviceQueue);
VK_FUNCTION(vkCreateSwapchainKHR);
VK_FUNCTION(vkMapMemory);
VK_FUNCTION(vkUnmapMemory);

bool VK_Load_Global_Functions();
bool VK_Load_Instance_Functions(VkInstance Instance);
bool VK_Load_Device_Functions(VkDevice Device);

#endif
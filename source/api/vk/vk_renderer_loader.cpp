bool VK_Load_Global_Functions()
{
    VK_Load_Global_Function(vkCreateInstance);
    VK_Load_Global_Function(vkEnumerateInstanceExtensionProperties);
    VK_Load_Global_Function(vkEnumerateInstanceLayerProperties);
    return true;
}

bool VK_Load_Instance_Functions(VkInstance Instance)
{
    VK_Load_Instance_Function(vkDestroyInstance);
    VK_Load_Instance_Function(vkEnumeratePhysicalDevices);
    VK_Load_Instance_Function(vkEnumerateDeviceExtensionProperties);
    VK_Load_Instance_Function(vkGetPhysicalDeviceProperties);
    VK_Load_Instance_Function(vkGetPhysicalDeviceQueueFamilyProperties);
    VK_Load_Instance_Function(vkGetDeviceProcAddr);
    VK_Load_Instance_Function(vkCreateDevice);
    
    VK_Load_Instance_Function(vkDestroySurfaceKHR);
    VK_Load_Instance_Function(vkGetPhysicalDeviceSurfaceSupportKHR);
    VK_Load_Instance_Function(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    VK_Load_Instance_Function(vkGetPhysicalDeviceSurfaceFormatsKHR);
    
#ifdef _WIN32
    VK_Load_Instance_Function(vkCreateWin32SurfaceKHR);
#endif
    
    return true;
}

bool VK_Load_Device_Functions(VkDevice Device)
{
    VK_Load_Device_Function(vkDestroyDevice);
    VK_Load_Device_Function(vkGetDeviceQueue);
    VK_Load_Device_Function(vkCreateSwapchainKHR);
    return true;
}
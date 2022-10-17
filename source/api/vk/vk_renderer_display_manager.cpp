#ifdef _WIN32
ak_handle<ak_display> vk_display_manager::Create_Display(HWND Window)
{
    VkSurfaceKHR Surface = VK_Win32_Create_Surface(Window);
    return Create_Display(Surface);
}
#endif

ak_handle<ak_display> vk_display_manager::Create_Display(VkSurfaceKHR Surface)
{
    arena* Scratch = Get_Thread_Context()->ScratchArena;
    
    vk_device* Device = DeviceContext->PhysicalDevice;
    uint32_t FrameCount = 3;
    
    VkSurfaceCapabilitiesKHR SurfaceCaps;
    VK_Warning_Check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device->PhysicalDevice, Surface, &SurfaceCaps), Str8_Lit("Failed to get the physical device surface capabilities"));
    
#ifdef _WIN32
    VkFormat TargetFormat = VK_FORMAT_B8G8R8A8_UNORM;
#else
    VkFormat TargetFormat = VK_FORMAT_R8G8R8A8_UNORM;
#endif
    
    
    VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
    SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.surface = Surface;
    SwapchainCreateInfo.minImageCount = FrameCount;
    SwapchainCreateInfo.imageFormat = TargetFormat;
    SwapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    SwapchainCreateInfo.imageExtent = SurfaceCaps.currentExtent;
    SwapchainCreateInfo.imageArrayLayers = 1;
    SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    
    VkSwapchainKHR Swapchain;
    if(vkCreateSwapchainKHR(DeviceContext->Device, &SwapchainCreateInfo, VK_NULL_HANDLE, &Swapchain) != VK_SUCCESS)
    {
        Warning_Log(Str8_Lit("Failed to create a vulkan swap chain with image count %d"), SwapchainCreateInfo.minImageCount);
        
        SwapchainCreateInfo.minImageCount = FrameCount-1;
        VK_Warning_Check(vkCreateSwapchainKHR(DeviceContext->Device, &SwapchainCreateInfo, VK_NULL_HANDLE, &Swapchain), Str8_Lit("Failed to create the vulkan swapchain"));
    }
    
    async_handle<vk_display> Handle = Displays.Allocate();
    vk_display* Display = Displays.Lock(Handle);
    {
        Display->Surface = Surface;
        Display->Swapchain = Swapchain;
    }
    Displays.Unlock(Handle);
    
    ak_handle<ak_display> Result = {Handle.ID.ID};
    return Result;
}

bool VK_Init_Display_Manager(vk_device_context* DeviceContext)
{
    vk_display_manager* DisplayManager = &DeviceContext->DisplayManager;
    DisplayManager->DeviceContext = DeviceContext;
    DisplayManager->Displays = Create_Async_Pool<vk_display>(Get_Thread_Context()->MainArena, 64);
    return true;
}
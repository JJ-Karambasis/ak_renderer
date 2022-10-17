#ifndef VK_RENDERER_DISPLAY_MANAGER_H
#define VK_RENDERER_DISPLAY_MANAGER_H

struct vk_display
{
    VkSurfaceKHR Surface;
    VkSwapchainKHR Swapchain;
};

struct vk_display_manager : public ak_display_manager
{
    vk_device_context*     DeviceContext;
    async_pool<vk_display> Displays;
    
#ifdef _WIN32
    ak_handle<ak_display> Create_Display(HWND Window) final;
#endif
    ak_handle<ak_display> Create_Display(VkSurfaceKHR Surface);
};

bool VK_Init_Display_Manager(vk_device_context* DeviceContext);

#endif
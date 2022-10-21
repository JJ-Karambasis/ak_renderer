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
    ak_handle<ak_texture> Get_Next_Display_Texture(ak_handle<ak_display> Display) final;
    void Present(const ak_handle<ak_display>* Displays, uint32_t DisplayCount) final;
};

bool VK_Init_Display_Manager(vk_device_context* DeviceContext);

#endif
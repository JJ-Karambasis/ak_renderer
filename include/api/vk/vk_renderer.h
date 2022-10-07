#ifndef VK_RENDERER_H
#define VK_RENDERER_H

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

struct vk_device;

#ifdef _WIN32
#include "vk_renderer_win32.h"
#endif

#include "vk_renderer_display_manager.h"
#include "vk_renderer_resource_manager.h"
#include "vk_renderer_core.h"
#include "vk_renderer_loader.h"

struct vk_context
{
    VkInstance        Instance;
    ak_device_list    Devices;
    vk_device_context DeviceContext;
};

static ak_renderer_init_result VK_Init_Renderer();
static ak_device_context* VK_Set_Device(ak_device* Device);
static vk_context* VK_Get_Context();
static bool VK_Load_Library();
static void VK_Unload_Library();
static bool VK_Begin_Presentation_Engine_Detection_Support();
static bool VK_Presentation_Engine_Queue_Family_Support(VkPhysicalDevice Device, uint32_t QueueFamilyIndex);
static void  VK_End_Presentation_Engine_Detection_Support();

#endif
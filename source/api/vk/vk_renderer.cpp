#define VK_Check(type, function, msg, ...) do \
{ \
if((function) != VK_SUCCESS) \
{ \
type##_Log(msg, __VA_ARGS__); \
return {}; \
} \
} \
while(0)

#define VK_Warning_Check(function, msg, ...) VK_Check(Warning, function, msg, __VA_ARGS__)
#define VK_Error_Check(function, msg, ...) VK_Check(Error, function, msg, __VA_ARGS__)

static ak_renderer_init_result VK_Init_Renderer()
{
    vk_context* Context = VK_Get_Context();
    
    Bool_Error_Check(VK_Load_Library(), Str8_Lit("Failed to load the vulkan library"));
    Bool_Error_Check(VK_Load_Global_Functions(), Str8_Lit("Failed to load the global vulkan functions"));
    Bool_Error_Check(VK_Create_Instance(), Str8_Lit("Failed to create the vulkan instance"));
    Bool_Error_Check(VK_Load_Instance_Functions(Context->Instance), Str8_Lit("Failed to load the instance vulkan functions"));
    Bool_Error_Check(VK_Load_Devices(), Str8_Lit("Failed to load the vulkan devices"));
    Bool_Error_Check(VK_Init_Glslang(), Str8_Lit("Failed to initialize glslang"));
    
    ak_renderer_init_result Result = {};
    Result.Initialized = Context->Devices.DeviceCount > 0;
    Result.Devices = Context->Devices;
    return Result;
}

static ak_device_context* VK_Set_Device(ak_device* Device)
{
    return VK_Create_Device_Context((vk_device*)Device);
}

static vk_context* VK_Get_Context()
{
    static vk_context Context;
    return &Context;
}

#include "vk_renderer_loader.cpp"
#include "vk_renderer_core.cpp"
#include "vk_renderer_shaders.cpp"

#ifdef _WIN32
#include "vk_renderer_win32.cpp"
#endif

#include "vk_renderer_display_manager.cpp"
#include "vk_renderer_resource_manager.cpp"
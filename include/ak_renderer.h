#ifndef AK_RENDERER_H
#define AK_RENDERER_H

#include <stdint.h>

#ifdef _MSC_VER
#define ak__shared_export __declspec(dllexport)
#endif

#ifdef _WIN32
#include <windows.h>
#endif

struct ak_device
{
    const char* Name;
};

struct ak_device_list
{
    uint32_t    DeviceCount;
    ak_device** Devices;
};

struct ak_resource_manager
{
};

struct ak_display
{
};

struct ak_display_manager
{
#if 0 
#ifdef _WIN32
    virtual ak_display* Create_Display(HWND Window) = 0;
#endif
    virtual void Delete_Display(ak_display* Display);
    virtual void Present(const ak_display** Displays, uint32_t DisplayCount) = 0;
#endif
};

struct ak_cmd_buffer
{
};

struct ak_device_context
{
    virtual ak_device* Get_Device() = 0;
    virtual ak_resource_manager* Get_Resource_Manager() = 0;
    virtual ak_display_manager* Get_Display_Manager() = 0;
#if 0 
    virtual ak_cmd_buffer* Begin_Command_Recording() = 0;
    virtual bool Execute(const ak_cmd_buffer** CmdBuffers, uint32_t CmdBufferCount) = 0;
#endif
};

typedef void* ak_renderer_memory_allocate(size_t Size, void* UserData);
typedef void  ak_renderer_memory_free(void* Memory, void* UserData);
typedef void ak_renderer_log_callback(const char* Message, void* UserData);

struct ak_renderer_init_parameters
{
    ak_renderer_memory_allocate* MemoryAllocate;
    ak_renderer_memory_free*     MemoryFree;
    void*                        MemoryUserData;
    
    ak_renderer_log_callback*    LogDebug;
    ak_renderer_log_callback*    LogInfo;
    ak_renderer_log_callback*    LogWarning;
    ak_renderer_log_callback*    LogError;
    void*                        LogUserData;
};

struct ak_renderer_init_result
{
    bool           Initialized;
    ak_device_list Devices;
};

struct ak_error
{
    const char* Message;
};

extern "C" ak__shared_export ak_renderer_init_result AK_Init_Renderer(const ak_renderer_init_parameters& Parameters); 
extern "C" ak__shared_export ak_device_context* AK_Set_Device(ak_device* Device);
extern "C" ak__shared_export ak_error AK_Get_Last_Error();

#endif
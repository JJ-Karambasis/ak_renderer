#ifndef AK_RENDERER_H
#define AK_RENDERER_H

#include <stdint.h>

#ifdef _MSC_VER
#define ak__shared_export __declspec(dllexport)
#endif

#ifdef _WIN32
#include <windows.h>
#endif

template <typename type>
struct ak_handle
{
    uint64_t ID;
    inline bool Is_Null() const { return ID == 0; }
};

enum ak_vertex_format
{
    AK_VERTEX_FORMAT_UNKNOWN,
    AK_VERTEX_FORMAT_P3_N3,
    AK_VERTEX_FORMAT_COUNT,
};

static const size_t G_AKVertexFormatSizes[AK_VERTEX_FORMAT_COUNT] = 
{
    0, sizeof(float[3])*2
};

enum ak_index_format
{
    AK_INDEX_FORMAT_UNKNOWN,
    AK_INDEX_FORMAT_16_BIT,
    AK_INDEX_FORMAT_32_BIT,
    AK_INDEX_FORMAT_COUNT
};

static const size_t G_AKIndexFormatSizes[AK_INDEX_FORMAT_COUNT] = 
{
    0, 2, 4
};

struct ak_device
{
    const char* Name;
};

struct ak_device_list
{
    uint32_t    DeviceCount;
    ak_device** Devices;
};

struct ak_display;
struct ak_material;
struct ak_mesh;
struct ak_texture;

struct ak_resource_manager
{
    virtual ak_handle<ak_mesh> Create_Mesh(ak_vertex_format VertexFormat, uint32_t VertexCount, ak_index_format IndexFormat, uint32_t IndexCount) = 0;
    virtual void Upload_Mesh(ak_handle<ak_mesh> Mesh, void* Vertices, uint32_t VertexCount, uint32_t VertexOffset, void* Indices, uint32_t IndexCount, uint32_t IndexOffset) = 0;
};

struct ak_display_manager
{
#ifdef _WIN32
    virtual ak_handle<ak_display> Create_Display(HWND Window) = 0;
#endif
    
    virtual ak_handle<ak_texture> Get_Next_Display_Texture(ak_handle<ak_display> Display) = 0;
    
    //virtual void Delete_Display(ak_display* Display);
    virtual void Present(const ak_handle<ak_display>* Displays, uint32_t DisplayCount) = 0;
};

struct ak_lighting_pass_begin_info
{
    ak_handle<ak_texture> RenderTarget;
    float*                View;
};

struct ak_cmd_lighting_pass
{
    virtual void Draw(ak_handle<ak_mesh> Mesh, uint32_t IndexCount, uint32_t IndexOffset, uint32_t VertexOffset, float* Transform, float* Color) = 0;
};

struct ak_cmd_buffer
{
    virtual void Reset() = 0;
    virtual ak_cmd_lighting_pass* Begin_Lighting_Pass(const ak_lighting_pass_begin_info* LightingPassBeginInfo) = 0;
};

struct ak_device_context
{
    virtual ak_device* Get_Device() = 0;
    virtual ak_resource_manager* Get_Resource_Manager() = 0;
    virtual ak_display_manager* Get_Display_Manager() = 0;
    virtual ak_cmd_buffer* Allocate_Command_Buffer() = 0;
    virtual bool Execute(const ak_cmd_buffer* const* CmdBuffers, uint32_t CmdBufferCount) = 0;
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
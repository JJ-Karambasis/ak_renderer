#include <ak_renderer.h>

#include <stdio.h>

#include "test_window.cpp"
#include "test_materials.cpp"

union v3f
{
    float Data[3];
    float x, y, z;
    
    inline float& operator[](uint32_t Index)
    {
        return Data[Index];
    }
    
    inline operator float*()
    {
        return Data;
    }
};

struct vertex_p3_n3
{
    v3f P;
    v3f N;
};

union m4x3f
{
    float Data[12];
    v3f   Rows[4];
    struct
    {
        v3f x;
        v3f y;
        v3f z;
        v3f t;
    };
    
    inline float& operator[](uint32_t Index)
    {
        return Data[Index];
    }
    
    inline operator float*()
    {
        return Data;
    }
};

v3f V3(float x, float y, float z)
{
    v3f Result;
    Result.x = x;
    Result.y = y;
    Result.z = z;
    return Result;
}

m4x3f Translate4x3(float x, float y, float z)
{
    m4x3f Result = {};
    Result.Rows[0][0] = 1;
    Result.Rows[1][1] = 1;
    Result.Rows[2][2] = 1;
    Result.t.x = x;
    Result.t.y = y;
    Result.t.z = z;
    return Result;
}

void Console_Log(const char* Message)
{
    printf("%s\n", Message);
}

int main(int ArgCount, const char** Arguments)
{
    ak_renderer_init_parameters InitParameters = {};
    ak_renderer_init_result InitResult = AK_Init_Renderer(InitParameters);
    if(!InitResult.Initialized)
    {
        Console_Log("Failed to initialize the renderer!");
        return 1;
    }
    
    ak_device_context* DeviceContext = AK_Set_Device(InitResult.Devices.Devices[0]);
    if(!DeviceContext)
    {
        Console_Log("Failed to initialize the device context!");
        return 1;
    }
    
    ak_display_manager* DisplayManager = DeviceContext->Get_Display_Manager();
    ak_resource_manager* ResourceManager = DeviceContext->Get_Resource_Manager();
    
    window Window = Create_Window(DisplayManager, 1920, 1080);
    
    if(Window.Display.Is_Null())
    {
        Console_Log("Failed to create the window!");
        return 1;
    }
    
    vertex_p3_n3 BoxVertices[24] = 
    {
        {V3(-0.5f, -0.5f, 0.5f), V3(0.0f, 0.0f, 1.0f)}, 
        {V3( 0.5f, -0.5f, 0.5f), V3(0.0f, 0.0f, 1.0f)}, 
        {V3( 0.5f,  0.5f, 0.5f), V3(0.0f, 0.0f, 1.0f)}, 
        {V3(-0.5f,  0.5f, 0.5f), V3(0.0f, 0.0f, 1.0f)},
        
        {V3( 0.5f, -0.5f,  0.5f), V3(1.0f, 0.0f, 0.0f)}, 
        {V3( 0.5f, -0.5f, -0.5f), V3(1.0f, 0.0f, 0.0f)},
        {V3( 0.5f,  0.5f, -0.5f), V3(1.0f, 0.0f, 0.0f)}, 
        {V3( 0.5f,  0.5f,  0.5f), V3(1.0f, 0.0f, 0.0f)},
        
        {V3( 0.5f, -0.5f, -0.5f), V3(0.0f, 0.0f, -1.0f)}, 
        {V3(-0.5f, -0.5f, -0.5f), V3(0.0f, 0.0f, -1.0f)}, 
        {V3(-0.5f,  0.5f, -0.5f), V3(0.0f, 0.0f, -1.0f)},
        {V3( 0.5f,  0.5f, -0.5f), V3(0.0f, 0.0f, -1.0f)},
        
        {V3(-0.5f, -0.5f, -0.5f), V3(-1.0f, 0.0f, 0.0f)}, 
        {V3(-0.5f, -0.5f,  0.5f), V3(-1.0f, 0.0f, 0.0f)}, 
        {V3(-0.5f,  0.5f,  0.5f), V3(-1.0f, 0.0f, 0.0f)}, 
        {V3(-0.5f,  0.5f, -0.5f), V3(-1.0f, 0.0f, 0.0f)},
        
        {V3(-0.5f, -0.5f, 0.5f), V3(0.0f, 0.0f, -1.0f)},
        {V3( 0.5f, -0.5f, 0.5f), V3(0.0f, 0.0f, -1.0f)}, 
        {V3( 0.5f, -0.5f, -0.5f), V3(0.0f, 0.0f, -1.0f)},
        {V3(-0.5f, -0.5f, -0.5f), V3(0.0f, 0.0f, -1.0f)},
        
        {V3(-0.5f, 0.5f,  0.5f), V3(0.0f, 0.0f, 1.0f)}, 
        {V3( 0.5f, 0.5f,  0.5f), V3(0.0f, 0.0f, 1.0f)}, 
        {V3( 0.5f, 0.5f, -0.5f), V3(0.0f, 0.0f, 1.0f)}, 
        {V3(-0.5f, 0.5f, -0.5f), V3(0.0f, 0.0f, 1.0f)}
    };
    
    uint16_t BoxIndices[36] = 
    {
        0, 1, 2, 
        2, 3, 0,
        
        4, 5, 6, 
        6, 7, 4,
        
        8, 9, 10, 
        10, 11, 8,
        
        12, 13, 14, 
        14, 15, 12
            
    };
    
    ak_handle<ak_mesh > BoxMesh = ResourceManager->Create_Mesh(AK_VERTEX_FORMAT_P3_N3, sizeof(BoxVertices)/sizeof(BoxVertices[0]), AK_INDEX_FORMAT_16_BIT, sizeof(BoxIndices)/sizeof(BoxIndices[0]));
    ResourceManager->Upload_Mesh(BoxMesh, BoxVertices, sizeof(BoxVertices)/sizeof(BoxVertices[0]), 0, BoxIndices, sizeof(BoxIndices)/sizeof(BoxIndices[0]), 0);
    
    ak_cmd_buffer* CommandBuffer = DeviceContext->Allocate_Command_Buffer();
    
    while(Poll_OS())
    {
        ak_handle<ak_texture> DisplayTexture = DisplayManager->Get_Next_Display_Texture(Window.Display);
        
        CommandBuffer->Reset();
        
        ak_lighting_pass_begin_info LightingPassInfo = {};
        LightingPassInfo.RenderTarget = DisplayTexture;
        LightingPassInfo.View = Translate4x3(0.0f, 0.0f, 3.0f);
        
        ak_cmd_lighting_pass* LightingPass = CommandBuffer->Begin_Lighting_Pass(&LightingPassInfo);
        LightingPass->Draw(BoxMesh, 36, 0, 0, Translate4x3(0.0f, 0.0f, 0.0f), V3(1.0f, 0.0f, 0.0f));
        
        DeviceContext->Execute(&CommandBuffer, 1);
        DisplayManager->Present(&Window.Display, 1);
    }
    
    return 0;
}
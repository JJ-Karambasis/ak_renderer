#include <ak_renderer.h>

#include <stdio.h>

#include "test_window.cpp"
#include "test_materials.cpp"

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
    
    window Window = Create_Window(DisplayManager, 1280, 720);
    
    ak_handle<ak_material> Material0 = ResourceManager->Create_Material(G_TestMaterial0, sizeof(G_TestMaterial0));
    
    ak_handle<ak_material> Material1 = ResourceManager->Create_Material(G_TestMaterial1, sizeof(G_TestMaterial1));
    
#if 0 
    if(!Window.Display)
    {
        Console_Log("Failed to create the window!");
        return 1;
    }
#endif
    
    while(Poll_OS())
    {
    }
    
    return 0;
}
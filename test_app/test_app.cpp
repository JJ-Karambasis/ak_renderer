#include <ak_renderer.h>

#include <stdio.h>

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
    
    return 0;
}
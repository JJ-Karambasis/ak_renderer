#include <ak_renderer.h>
#include <ak_renderer_base.h>

#include <stdio.h>

static void Default_Debug_Log(const char* Message, void* UserData)
{
    printf("DEBUG: %s\n", Message);
}

static void Default_Info_Log(const char* Message, void* UserData)
{
    printf("INFO: %s\n", Message);
}

static void Default_Warning_Log(const char* Message, void* UserData)
{
    printf("WARNING: %s\n", Message);
}

static void Default_Error_Log(const char* Message, void* UserData)
{
    printf("ERROR: %s\n", Message);
}

static void* Malloc_Allocate(size_t Size, void* UserData)
{
    return malloc(Size);
}

static void Malloc_Free(void* Memory, void* UserData)
{
    free(Memory);
}

static void* Memory_Allocate(size_t Size)
{
    GAllocator.MemoryAllocate(Size, GAllocator.UserData);
}

static void  Memory_Free(void* Memory)
{
    GAllocator.MemoryFree(Memory, GAllocator.UserData);
}

static void* Memory_Allocate_Aligned(size_t Size, size_t Alignment)
{
    void* P1;
    void** P2;
    
    size_t Offset = 0;
    if(Alignment) Offset = (Alignment-1) + sizeof(void*);
    
    P1 = Memory_Allocate(Size+Offset);
    if(!P1) return NULL;
    
    P2 = (void**)P1;
    if(Alignment) P2 = (void**)(((size_t)(P1) + Offset) & ~(Alignment-1));
    P2[-1] = P1;
    return P2;
}

static void  Memory_Free_Aligned(void* Memory)
{
    Memory_Free(Memory);
}

extern "C" ak__shared_export 
ak_renderer_init_result AK_Init_Renderer(const ak_renderer_init_parameters& Parameters)
{
    GAllocator.MemoryAllocate = Parameters.MemoryAllocate;
    GAllocator.MemoryFree = Parameters.MemoryFree;
    GAllocator.UserData = Parameters.MemoryUserData;
    
    if(!Parameters.MemoryAllocate || !Parameters.MemoryFree)
    {
        GAllocator.MemoryAllocate = Malloc_Allocate;
        GAllocator.MemoryFree = Malloc_Free;
    }
    
    GLogger.LogDebug = Parameters.LogDebug;
    if(!GLogger.LogDebug) GLogger.LogDebug = Default_Debug_Log;
    
    GLogger.LogInfo = Parameters.LogInfo;
    if(!GLogger.LogInfo) GLogger.LogInfo = Default_Info_Log;
    
    GLogger.LogWarning = Parameters.LogWarning;
    if(!GLogger.LogWarning) GLogger.LogWarning = Default_Warning_Log;
    
    GLogger.LogError = Parameters.LogError;
    if(!GLogger.LogError) GLogger.LogError = Default_Error_Log;
    
    GLogger.UserData = Parameters.LogUserData;
    
    //GMainArena = Create_Async_Arena(&GAllocator);
    //BOOL_CHECK(GMainArena, "Failed to create the renderer arena!");
    
    
    
#ifdef _WIN32
    return Win32_Init_Renderer();
#endif
}

extern "C" ak__shared_export 
ak_device_context* AK_Set_Device(ak_device* Device)
{
    return NULL;
}

#ifdef _WIN32
#include "os/win32/win32_renderer.cpp"
#endif

//#include "util/ak_renderer_string.cpp"
//#include "util/ak_renderer_memory.cpp"
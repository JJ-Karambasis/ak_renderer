#include <ak_renderer.h>
#include <ak_renderer_base.h>

#include <stdio.h>

#define Debug_Log(msg, ...) do \
{ \
thread_context* Internal__ThreadContext = Get_Thread_Context(); \
if(Internal__ThreadContext && Internal__ThreadContext->ScratchArena) \
{ \
arena* Internal__ScratchArena = Internal__ThreadContext->ScratchArena; \
str8 Message = Str8_Format(Internal__ScratchArena, msg, __VA_ARGS__); \
GLogger.LogDebug((const char*)Message.Str, GLogger.UserData); \
} \
else \
{ \
char TempBuffer[2048]; \
stbsp_snprintf(TempBuffer, 2048, (const char*)msg.Str, __VA_ARGS__); \
GLogger.LogDebug(TempBuffer, GLogger.UserData); \
} \
} while(0)

#define Info_Log(msg, ...) do \
{ \
thread_context* Internal__ThreadContext = Get_Thread_Context(); \
if(Internal__ThreadContext && Internal__ThreadContext->ScratchArena) \
{ \
arena* Internal__ScratchArena = Internal__ThreadContext->ScratchArena; \
str8 Message = Str8_Format(Internal__ScratchArena, msg, __VA_ARGS__); \
GLogger.LogInfo((const char*)Message.Str, GLogger.UserData); \
} \
else \
{ \
char TempBuffer[2048]; \
stbsp_snprintf(TempBuffer, 2048, (const char*)msg.Str, __VA_ARGS__); \
GLogger.LogInfo(TempBuffer, GLogger.UserData); \
} \
} while(0)

#define Warning_Log(msg, ...) do \
{ \
thread_context* Internal__ThreadContext = Get_Thread_Context(); \
if(Internal__ThreadContext && Internal__ThreadContext->ScratchArena) \
{ \
arena* Internal__ScratchArena = Internal__ThreadContext->ScratchArena; \
str8 Message = Str8_Format(Internal__ScratchArena, msg, __VA_ARGS__); \
GLogger.LogWarning((const char*)Message.Str, GLogger.UserData); \
} \
else \
{ \
char TempBuffer[2048]; \
stbsp_snprintf(TempBuffer, 2048, (const char*)msg.Str, __VA_ARGS__); \
GLogger.LogWarning(TempBuffer, GLogger.UserData); \
} \
} while(0)

#define Error_Log(msg, ...) do \
{ \
thread_context* Internal__ThreadContext = Get_Thread_Context(); \
if(Internal__ThreadContext && Internal__ThreadContext->ScratchArena) \
{ \
arena* Internal__ScratchArena = Internal__ThreadContext->ScratchArena; \
str8 Message = Str8_Format(Internal__ScratchArena, msg, __VA_ARGS__); \
GLogger.LogError((const char*)Message.Str, GLogger.UserData); \
} \
else \
{ \
char TempBuffer[2048]; \
stbsp_snprintf(TempBuffer, 2048, (const char*)msg.Str, __VA_ARGS__); \
GLogger.LogError(TempBuffer, GLogger.UserData); \
} \
} while(0)

#define Bool_Check(type, condition, msg, ...) do \
{ \
if(!(condition)) \
{ \
type##_Log(msg, __VA_ARGS__); \
return {}; \
} \
} \
while(0)

#define Bool_Warning_Check(condition, msg, ...) Bool_Check(Warning, condition, msg, __VA_ARGS__)
#define Bool_Error_Check(condition, msg, ...) Bool_Check(Error, condition, msg, __VA_ARGS__)

static void Default_Debug_Log(const char* Message, void* UserData)
{
    thread_context* ThreadContext = Get_Thread_Context();
    if(ThreadContext && ThreadContext->ScratchArena)
    {
        str8 UTF8 = Str8_Format(ThreadContext->ScratchArena, Str8_Lit("DEBUG: %s\n"), Message);
        str16 UTF16 = UTF8_To_UTF16(Get_Thread_Context()->ScratchArena, UTF8);
        wprintf((const wchar_t*)UTF16.Str);
    }
    else
    {
        printf("DEBUG: %s\n", Message);
    }
}

static void Default_Info_Log(const char* Message, void* UserData)
{
    thread_context* ThreadContext = Get_Thread_Context();
    if(ThreadContext && ThreadContext->ScratchArena)
    {
        str8 UTF8 = Str8_Format(ThreadContext->ScratchArena, Str8_Lit("INFO: %s\n"), Message);
        str16 UTF16 = UTF8_To_UTF16(Get_Thread_Context()->ScratchArena, UTF8);
        wprintf((const wchar_t*)UTF16.Str);
    }
    else
    {
        printf("INFO: %s\n", Message);
    }
}

static void Default_Warning_Log(const char* Message, void* UserData)
{
    thread_context* ThreadContext = Get_Thread_Context();
    if(ThreadContext && ThreadContext->ScratchArena)
    {
        str8 UTF8 = Str8_Format(ThreadContext->ScratchArena, Str8_Lit("WARNING: %s\n"), Message);
        str16 UTF16 = UTF8_To_UTF16(Get_Thread_Context()->ScratchArena, UTF8);
        wprintf((const wchar_t*)UTF16.Str);
    }
    else
    {
        printf("WARNING: %s\n", Message);
    }
}

static void Default_Error_Log(const char* Message, void* UserData)
{
    thread_context* ThreadContext = Get_Thread_Context();
    if(ThreadContext && ThreadContext->ScratchArena)
    {
        str8 UTF8 = Str8_Format(ThreadContext->ScratchArena, Str8_Lit("ERROR: %s\n"), Message);
        str16 UTF16 = UTF8_To_UTF16(Get_Thread_Context()->ScratchArena, UTF8);
        wprintf((const wchar_t*)UTF16.Str);
    }
    else
    {
        printf("ERROR: %s\n", Message);
    }
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

void Bind_Core(const ak_renderer_init_parameters& Parameters)
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
}

extern "C" ak__shared_export 
ak_renderer_init_result AK_Init_Renderer(const ak_renderer_init_parameters& Parameters)
{
    Bind_Core(Parameters);
    
    async_arena* MainArena = Create_Async_Arena(&GAllocator, Megabyte(1));
    Bool_Error_Check(MainArena, Str8_Lit("Failed to create the main async arena"));
    Bool_Error_Check(Create_Thread_Context(MainArena), Str8_Lit("Failed to create the thread context"));
    
#ifdef _WIN32
    return Win32_Init_Renderer();
#endif
}

extern "C" ak__shared_export 
ak_device_context* AK_Set_Device(ak_device* Device)
{
#ifdef _WIN32
    return Win32_Set_Device(Device);
#endif
}

#ifdef UNIT_TESTS
#include "ak_renderer_tests.cpp"
#endif

#ifdef _WIN32
#include "os/win32/win32_renderer.cpp"
#endif

#include "util/ak_renderer_util.cpp"

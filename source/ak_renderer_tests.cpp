#define UTEST_IMPLEMENTATION
#include <utest/utest.h>

THREAD_CALLBACK(AsyncArenaTestCallback)
{
    thread_context ThreadContext = {};
    ThreadContext.ScratchArena = Create_Arena(&GAllocator, Megabyte(32));
    Set_Thread_Context(&ThreadContext);
    
    async_arena* Arena = (async_arena*)UserData;
    
    for(uint32_t Index = 0; Index < 1000; Index++)
    {
        Arena->Push(256);
    }
    
    return 0;
}

#define THREAD_COUNT 4
UTEST(Memory, AsyncArena)
{
    Bind_Core({});
    async_arena* Arena = Create_Async_Arena(&GAllocator, Gigabyte(1));
    
    thread Threads[THREAD_COUNT];
    for(uint32_t ThreadIndex = 0; ThreadIndex < THREAD_COUNT; ThreadIndex++)
    {
        Threads[ThreadIndex] = Create_Thread(AsyncArenaTestCallback, Arena);
    }
    for(uint32_t ThreadIndex = 0; ThreadIndex < THREAD_COUNT; ThreadIndex++)
    {
        Wait_Thread(Threads[ThreadIndex]);
    }
    
    ASSERT_EQ(Arena->FirstBlock->Used, 1000*THREAD_COUNT*256);
    
    for(uint32_t ThreadIndex = 0; ThreadIndex < THREAD_COUNT; ThreadIndex++)
    {
        Delete_Thread(Threads[ThreadIndex]);
    }
}

THREAD_CALLBACK(AsyncPoolTestCallback)
{
    thread_context ThreadContext = {};
    ThreadContext.ScratchArena = Create_Arena(&GAllocator, Megabyte(1));
    Set_Thread_Context(&ThreadContext);
    
    async_pool<uint32_t>* Pool = (async_pool<uint32_t>*)UserData;
    async_handle<uint32_t> Handles[1000];
    
    random32 Random32 = Init_Random32();
    
    for(uint32_t i = 0; i < 1000; i++)
        Handles[i] = Pool->Allocate();
    
    Sleep(Random32.Random_Between(100, 1000));
    
    for(uint32_t i = 0; i < 1000; i++)
        Pool->Free(Handles[i]);
    
    Sleep(Random32.Random_Between(100, 1000));
    
    return 0;
}

UTEST(Memory, AsyncPool)
{
    Bind_Core({});
    async_pool<uint32_t> Pool = Create_Async_Pool<uint32_t>(&GAllocator, 512);
    
    thread Threads[THREAD_COUNT];
    for(uint32_t ThreadIndex = 0; ThreadIndex < THREAD_COUNT; ThreadIndex++)
    {
        Threads[ThreadIndex] = Create_Thread(AsyncPoolTestCallback, &Pool);
    }
    for(uint32_t ThreadIndex = 0; ThreadIndex < THREAD_COUNT; ThreadIndex++)
    {
        Wait_Thread(Threads[ThreadIndex]);
    }
    
    for(uint32_t ThreadIndex = 0; ThreadIndex < THREAD_COUNT; ThreadIndex++)
    {
        Delete_Thread(Threads[ThreadIndex]);
    }
}

UTEST_MAIN();
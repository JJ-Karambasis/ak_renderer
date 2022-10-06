/* date = October 6th 2022 0:00 am */

#ifndef AK_RENDERER_MEMORY_H
#define AK_RENDERER_MEMORY_H

struct allocator 
{
    ak_renderer_memory_allocate* MemoryAllocate;
    ak_renderer_memory_free*     MemoryFree;
    void*                        UserData;
};

static allocator GAllocator;

#endif //AK_RENDERER_MEMORY_H
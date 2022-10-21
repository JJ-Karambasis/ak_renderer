/* date = October 6th 2022 0:00 am */

#ifndef AK_RENDERER_MEMORY_H
#define AK_RENDERER_MEMORY_H

template <typename type>
struct auto_ptr
{
    type* Ptr;
    void* PtrUserData;
    
    auto_ptr(type* Pointer);
    auto_ptr(type* Pointer, void* UserData);
    
    bool operator!() const;
    operator type*() const;
    
    ~auto_ptr();
};

enum memory_clear_flag
{
    MEMORY_CLEAR,
    MEMORY_NO_CLEAR
};

struct allocator 
{
    ak_renderer_memory_allocate* MemoryAllocate;
    ak_renderer_memory_free*     MemoryFree;
    void*                        UserData;
};

struct async_allocator : public allocator
{
    spin_lock Lock;
};

struct arena_block
{
    uint8_t*     Memory;
    size_t       Used;
    size_t       Size;
    arena_block* Next;
};

struct arena;

struct arena_marker
{
    arena*       Arena;
    arena_block* Block;
    size_t       Marker;
};

struct arena : public allocator
{
    allocator*   Allocator;
    arena_block* FirstBlock;
    arena_block* CurrentBlock;
    arena_block* LastBlock;
    size_t       InitialBlockSize;
    
    void* Push(size_t Size, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    void* Push(size_t Size, size_t Alignment, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    
    template <typename type> type* Push_Struct(memory_clear_flag ClearFlag = MEMORY_CLEAR);
    template <typename type> type* Push_Struct(size_t Alignment, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    
    template <typename type> type* Push_Array(size_t Count, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    template <typename type> type* Push_Array(size_t Count, size_t Alignment, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    
    arena_marker Get_Marker();
    void Set_Marker(arena_marker Marker);
    
    void Clear(memory_clear_flag ClearFlag = MEMORY_CLEAR);
};

struct temp_arena
{
    arena_marker Marker;
    temp_arena();
    temp_arena(arena* TempArena);
    void Begin(arena* TempArena);
    void End();
    ~temp_arena();
};

struct async_arena : public async_allocator
{
    async_allocator* Allocator;
    arena_block*     FirstBlock;
    arena_block*     CurrentBlock;
    size_t           InitialBlockSize;
    
    void* Push(size_t Size, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    void* Push(size_t Size, size_t Alignment, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    
    template <typename type> type* Push_Struct(memory_clear_flag ClearFlag = MEMORY_CLEAR);
    template <typename type> type* Push_Struct(size_t Alignment, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    
    template <typename type> type* Push_Array(size_t Count, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    template <typename type> type* Push_Array(size_t Count, size_t Alignment, memory_clear_flag ClearFlag = MEMORY_CLEAR);
    
    void Clear(memory_clear_flag ClearFlag = MEMORY_CLEAR);
};

static async_allocator GAllocator;

static void* Memory_Copy(void* Dst, const void* Src, size_t Size);
static void* Memory_Clear(void* Dst, size_t Size);
static arena* Create_Arena(allocator* Allocator, size_t InitialBlockSize);
static async_arena* Create_Async_Arena(async_allocator* Allocator, size_t InitialBlockSize);

void* operator new(size_t Size, allocator* Arena);
void* operator new(size_t Size, async_allocator* Arena);

#endif //AK_RENDERER_MEMORY_H
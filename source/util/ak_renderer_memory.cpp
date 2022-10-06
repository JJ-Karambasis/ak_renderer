void* async_arena::Push(size_t Size, memory_clear_flag ClearFlag)
{
    if(!Size) return NULL;
    
    void* Result = NULL;
    temp_lock TempLock(&Lock);
    {
        arena_block* Block = CurrentBlock;
        while(Block && (Block->Used+Size > Block->Size))
        {
            Block = Block->Next;
        }
        
        if(!Block)
        {
            size_t BlockSize = Max(Size, InitialBlockSize);
            Block = (arena_block*)Allocator->MemoryAllocate(sizeof(arena_block)+BlockSize, Allocator->UserData);
            Bool_Warning_Check(Block, Str8_Lit("Failed to allocate async arena block!"));
            
            Memory_Clear(Block, sizeof(arena_block));
            Block->Memory = (uint8_t*)(Block+1);
            Block->Size = BlockSize;
            
            SLL_Stack_Push(FirstBlock, Block);
        }
        
        CurrentBlock = Block;
        
        Assert(CurrentBlock->Used + Size <= CurrentBlock->Size);
        Result = CurrentBlock->Memory + CurrentBlock->Used;
        CurrentBlock->Used += Size;
    }
    TempLock.End();
    
    if(ClearFlag == MEMORY_CLEAR)
        Memory_Clear(Result, Size);
    
    return Result;
}

void* async_arena::Push(size_t Size, size_t Alignment, memory_clear_flag ClearFlag)
{
    void* P1;
    void** P2;
    
    size_t Offset = 0;
    if(Alignment) Offset = (Alignment-1) + sizeof(void*);
    
    P1 = Push(Size+Offset, MEMORY_NO_CLEAR);
    Bool_Warning_Check(P1, Str8_Lit("Failed to allocate aligned async arena memory!"));
    
    P2 = (void**)P1;
    if(Alignment) P2 = (void**)(((size_t)(P1) + Offset) & ~(Alignment-1));
    P2[-1] = P1;
    
    if(ClearFlag == MEMORY_CLEAR)
        Memory_Clear(P2, Size);
    
    return P2;
}

template <typename type> type* 
async_arena::Push_Struct(memory_clear_flag ClearFlag)
{
    return (type*)Push(sizeof(type), alignof(type), ClearFlag);
}

template <typename type> type* 
async_arena::Push_Struct(size_t Alignment, memory_clear_flag ClearFlag)
{
    return (type*)Push(sizeof(type), Alignment, ClearFlag);
}

template <typename type> type* 
async_arena::Push_Array(size_t Count, memory_clear_flag ClearFlag)
{
    return (type*)Push(sizeof(type)*Count, alignof(type), ClearFlag);
}

template <typename type> type* 
async_arena::Push_Array(size_t Count, size_t Alignment, memory_clear_flag ClearFlag)
{
    return (type*)Push(sizeof(type)*Count, Alignment, ClearFlag);
}

void async_arena::Clear(memory_clear_flag ClearFlag)
{
    Lock.Lock();
    {
        for(arena_block* Block = FirstBlock; Block; Block = Block->Next)
        {
            if(ClearFlag == MEMORY_CLEAR) Memory_Clear(Block->Memory, Block->Used);
            Block->Used = 0;
        }
        CurrentBlock = FirstBlock;
    }
    Lock.Unlock();
}

static void* Memory_Clear(void* Dst, size_t Size)
{
    return memset(Dst, 0, Size);
}

static void* Arena_Allocate(size_t Size, void* UserData)
{
    arena* Arena = (arena*)UserData;
    return Arena->Push(Size, MEMORY_NO_CLEAR);
}

static void Arena_Free(void* Memory, void* UserData)
{
}

static void* Arena_Async_Allocate(size_t Size, void* UserData)
{
    async_arena* Arena = (async_arena*)UserData;
    return Arena->Push(Size, MEMORY_NO_CLEAR);
}

static void Arena_Async_Free(void* Memory, void* UserData)
{
}

static arena* Create_Arena(allocator* Allocator, size_t InitialBlockSize)
{
    arena* Arena = (arena*)Allocator->MemoryAllocate(sizeof(arena), Allocator->UserData);
    Memory_Clear(Arena, sizeof(arena));
    
    Arena->Allocator = Allocator;
    Arena->InitialBlockSize = InitialBlockSize;
    Arena->MemoryAllocate = Arena_Allocate;
    Arena->MemoryFree = Arena_Free;
    Arena->UserData = Arena;
    return Arena;
}

static async_arena* Create_Async_Arena(async_allocator* Allocator, size_t InitialBlockSize)
{
    async_arena* Arena = (async_arena*)Allocator->MemoryAllocate(sizeof(async_arena), Allocator->UserData);
    Memory_Clear(Arena, sizeof(async_arena));
    
    Arena->Allocator = Allocator;
    Arena->InitialBlockSize = InitialBlockSize;
    Arena->MemoryAllocate = Arena_Async_Allocate;
    Arena->MemoryFree = Arena_Async_Free;
    Arena->UserData = Arena;
    return Arena;
}
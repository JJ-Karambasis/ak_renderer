#ifndef AK_RENDERER_ASYNC_POOL_H
#define AK_RENDERER_ASYNC_POOL_H

union pool_id
{
    uint64_t ID;
    struct
    {
        union
        {
            struct
            {
                uint16_t PrevEntry;
                uint16_t NextEntry;
            };
            struct
            {
                uint16_t Unused;
                uint16_t Index;
            };
        };
        uint32_t Key;
    };
};

template <typename type>
struct async_handle
{
    pool_id ID;
    bool Is_Null() const;
};

template <typename type>
struct async_pool_entry
{
    spin_lock EntryLock;
    pool_id   ID;
    type      Entry;
};

template <typename type>
struct async_pool
{
    async_allocator*        Allocator;
    async_pool_entry<type>* Entries;
    uint32_t                NextKey;
    uint16_t                FirstFreeIndex;
    uint16_t                FirstIndex;
    uint16_t                MaxUsed;
    uint16_t                Length;
    uint16_t                Capacity;
    spin_lock               PoolLock;
    
    async_handle<type> Allocate();
    type* Lock(async_handle<type>& Handle);
    void  Unlock(async_handle<type>& Handle);
    void Free(async_handle<type>& Handle);
};

template <typename type> 
async_pool<type> Create_Async_Pool(async_allocator* Allocator, uint16_t InitialCapacity);

#endif
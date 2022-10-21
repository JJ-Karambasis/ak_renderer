template <typename type>
async_handle<type> async_pool<type>::Allocate()
{
    uint16_t Index;
    uint32_t Key;
    
    PoolLock.Lock();
    {
        if(FirstFreeIndex != (uint16_t)-1)
        {
            Index = FirstFreeIndex;
            FirstFreeIndex = Entries[FirstFreeIndex].ID.NextEntry;
        }
        else
        {
            if(MaxUsed == Capacity || !Entries)
            {
                uint16_t NewCapacity = Capacity;
                if(Entries) NewCapacity *= 2;
                
                async_pool_entry<type>* NewEntries = (async_pool_entry<type>*)Allocator->MemoryAllocate(sizeof(async_pool_entry<type>)*NewCapacity, Allocator->UserData);
                Memory_Clear(NewEntries, sizeof(async_pool_entry<type>)*NewCapacity);
                
                if(Entries) 
                {
                    Memory_Copy(NewEntries, Entries, sizeof(async_pool_entry<type>)*Capacity);
                    Allocator->MemoryFree(Entries, Allocator->UserData);
                }
                
                Entries = NewEntries;
                Capacity = NewCapacity;
            }
            
            Index = MaxUsed++;
        }
        
        if(FirstIndex != (uint16_t)-1)
            Entries[FirstIndex].ID.PrevEntry = Index;
        Key = NextKey++;
        
        if(NextKey == 0) NextKey = 1;
        Length++;
        
        async_pool_entry<type>* Entry = Entries + Index;
        Entry->ID.Key = Key;
        
        Entry->ID.NextEntry = FirstIndex;
        Entry->ID.PrevEntry = (uint16_t)-1;
        
        FirstIndex = Index;
        
        Memory_Clear(&Entry->Entry, sizeof(type));
        Entry->EntryLock = {};
    }
    PoolLock.Unlock();
    
    async_handle<type> Result = {};
    Result.ID.Key = Key;
    Result.ID.Index = Index;
    return Result;
}

template <typename type>
type* async_pool<type>::Lock(async_handle<type>& Handle)
{
    if(!Handle.ID.ID) return NULL;
    Assert(Handle.ID.Index < Capacity);
    
    async_pool_entry<type>* Entry = Entries + Handle.ID.Index;
    Entry->EntryLock.Lock();
    if(!Entry->ID.Key || (Entry->ID.Key != Handle.ID.Key)) 
    {
        Entry->EntryLock.Unlock();
        return NULL;
    }
    
    return &Entry->Entry;
}

template <typename type>
void async_pool<type>::Unlock(async_handle<type>& Handle)
{
    if(!Handle.ID.ID) return;
    Assert(Handle.ID.Index < Capacity);
    async_pool_entry<type>* Entry = Entries + Handle.ID.Index;
    Entry->EntryLock.Unlock();
}

template <typename type>
void async_pool<type>::Free(async_handle<type>& Handle)
{
    if(Handle.ID.ID)
    {
        Assert(Handle.ID.Index < Capacity);
        async_pool_entry<type>* Entry = Entries + Handle.ID.Index;
        PoolLock.Lock();
        Entry->EntryLock.Lock();
        {
            if(Entry->ID.Key && (Entry->ID.Key == Handle.ID.Key))
            {
                
                bool IsHead = Handle.ID.Index == FirstIndex;
                if(!IsHead)
                {
                    Assert(Entry->ID.PrevEntry != (uint16_t)-1);
                    Entries[Entry->ID.PrevEntry].ID.NextEntry = Entry->ID.NextEntry;
                    if(Entry->ID.NextEntry != (uint16_t)-1)
                        Entries[Entry->ID.NextEntry].ID.PrevEntry = Entry->ID.PrevEntry;
                }
                else
                {
                    Assert(Entry->ID.PrevEntry == (uint16_t)-1);
                    FirstIndex = Entry->ID.NextEntry;
                }
                
                if(Entry->ID.NextEntry != (uint16_t)-1)
                {
                    async_pool_entry<type>* NextEntry = Entries + Entry->ID.NextEntry;
                    NextEntry->ID.PrevEntry = Entry->ID.PrevEntry;
                }
                
                Entry->ID.ID = 0;
                Entry->ID.NextEntry = FirstFreeIndex;
                FirstFreeIndex = Handle.ID.Index;
                Length--;
            }
        }
        Entry->EntryLock.Unlock();
        PoolLock.Unlock();
    }
}

template <typename type> 
async_pool<type> Create_Async_Pool(async_allocator* Allocator, uint16_t InitialCapacity)
{
    async_pool<type> Result = {};
    Result.NextKey = 1;
    Result.Allocator = Allocator;
    Result.Capacity = InitialCapacity;
    Result.FirstFreeIndex = (uint16_t)-1;
    Result.FirstIndex = (uint16_t)-1;
    return Result;
}

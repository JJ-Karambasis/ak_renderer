#ifndef AK_RENDERER_HASH_H
#define AK_RENDERER_HASH_H

struct hash_slot
{
    int32_t  IsValid;
    uint32_t ItemIndex;
    uint32_t BaseCount;
    uint32_t Hash;
};

struct hash_slot_list
{
    uint32_t   SlotCapacity;
    hash_slot* Slots;
};

template <typename key,typename value>
struct hashmap;

template <typename key, typename value>
struct hashmap
{
    allocator*     Allocator;
    hash_slot_list SlotList;
    uint32_t       Length;
    uint32_t       ItemCapacity;
    uint32_t*      ItemSlots;
    key*           Keys;
    value*         Values;
    
    void Add(const key& Key, const value& Value);
    
    bool Contains_Key(const key& Key) const;
    value* Get(const key& Key);
    const value* Get(const key& Key) const;
    value& operator[](const key& Key);
    const value& operator[](const key& Key) const;
    
    void Remove(const key& Key);
    void Clear();
};

template <typename key>
struct hashset;

template <typename key>
struct hashset
{
    allocator*     Allocator;
    hash_slot_list SlotList;
    uint32_t       Length;
    uint32_t       ItemCapacity;
    uint32_t*      ItemSlots;
    key*           Keys;
    
    void Add(const key& Key);
    bool Contains(const key& Key) const;
    void Remove(const key& Key);
    void Clear();
};

template <typename key, typename value> hashmap<key, value> Create_Hashmap(allocator* Allocator, uint32_t SlotCapacity=128, uint32_t ItemCapacity=32);
template <typename key> hashset<key> Create_Hashset(allocator* Allocator, uint32_t SlotCapacity=128, uint32_t ItemCapacity=32);
uint32_t Hash_Function(uint32_t Key);
uint32_t Hash_Function(uint64_t Key);
uint32_t Hash_Function(const void* Ptr);
template <typename type, typename... args>
void Hash_Combine(uint32_t* Result, const type& Type, args... Args);

#endif
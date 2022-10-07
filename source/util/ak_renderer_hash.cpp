void Expand_Slot_List(hash_slot_list* SlotList, allocator* Allocator, uint32_t* ItemSlots, uint32_t SlotCapacity)
{
    SlotCapacity = Ceil_Pow2(SlotCapacity);
    uint32_t SlotMask = SlotCapacity-1;
    
    hash_slot* Slots = (hash_slot*)Allocator->MemoryAllocate(sizeof(hash_slot)*SlotCapacity, Allocator->UserData);
    Memory_Clear(Slots, sizeof(hash_slot)*SlotCapacity);
    
    for(uint32_t OldSlotIndex = 0; OldSlotIndex < SlotList->SlotCapacity; OldSlotIndex++)
    {
        if(SlotList->Slots[OldSlotIndex].IsValid)
        {
            uint32_t Hash = SlotList->Slots[OldSlotIndex].Hash;
            uint32_t BaseSlot = Hash & SlotMask;
            uint32_t Slot = BaseSlot;
            while(Slots[Slot].IsValid)
                Slot = (Slot+1) & SlotMask;
            Slots[Slot].Hash = Hash;
            uint32_t ItemIndex = SlotList->Slots[OldSlotIndex].ItemIndex;
            Slots[Slot].ItemIndex = ItemIndex;
            ItemSlots[ItemIndex] = Slot;            
            Slots[BaseSlot].BaseCount++;
            Slots[Slot].IsValid = true;
        }
    }
    
    SlotList->Slots = Slots;
    SlotList->SlotCapacity = SlotCapacity;
}

template <typename key, typename value>
void Expand_Item_List(hashmap<key, value>* Hashmap, uint32_t ItemCapacity)
{
    size_t DataSize = (sizeof(key)+sizeof(value)+sizeof(uint32_t)) * ItemCapacity;
    uint32_t* NewItemSlots = (uint32_t*)Hashmap->Allocator.Alloc(&Hashmap->Allocator, DataSize, 4, MEMORY_CLEAR);
    key* NewKeys = (key*)(NewItemSlots + ItemCapacity);
    value* NewValues = (value*)(NewKeys + ItemCapacity);
    
    if(ItemCapacity)
    {
        Memory_Copy(NewItemSlots, Hashmap->ItemSlots, Hashmap->Length*sizeof(uint32_t));
        Memory_Copy(NewKeys, Hashmap->Keys, Hashmap->Length*sizeof(key));
        Memory_Copy(NewValues, Hashmap->Values, Hashmap->Length*sizeof(value));
    }
    
    Hashmap->ItemSlots = NewItemSlots;
    Hashmap->Keys = NewKeys;
    Hashmap->Values = NewValues;
    Hashmap->ItemCapacity = ItemCapacity;
}

template <typename key>
void Expand_Item_List(hashset<key>* Hashset, uint32_t ItemCapacity)
{
    size_t DataSize = (sizeof(key)+sizeof(uint32_t)) * ItemCapacity;
    uint32_t* NewItemSlots = (uint32_t*)Hashset->Allocator->MemoryAllocate(DataSize, Hashset->Allocator->UserData);
    Memory_Clear(NewItemSlots, DataSize);
    key* NewKeys = (key*)(NewItemSlots + ItemCapacity);
    
    if(ItemCapacity)
    {
        Memory_Copy(NewItemSlots, Hashset->ItemSlots, Hashset->Length*sizeof(uint32_t));
        Memory_Copy(NewKeys, Hashset->Keys, Hashset->Length*sizeof(key));
    }
    
    Hashset->ItemSlots = NewItemSlots;
    Hashset->Keys = NewKeys;
    Hashset->ItemCapacity = ItemCapacity;
}

template <typename key>
int64_t Find_Slot(const hash_slot_list* SlotList, const key* Keys, const key& Key)
{
    uint32_t SlotMask = SlotList->SlotCapacity - 1;
    uint32_t Hash = Hash_Function(Key);
    
    uint32_t BaseSlot = Hash & SlotMask;
    uint32_t BaseCount = SlotList->Slots[BaseSlot].BaseCount;
    uint32_t Slot = BaseSlot;
    
    while(BaseCount > 0)
    {
        if(SlotList->Slots[Slot].IsValid)
        {
            uint32_t SlotHash = SlotList->Slots[Slot].Hash;
            uint32_t SlotBase = SlotHash & SlotMask;
            if(SlotBase == BaseSlot)
            {
                BaseCount--;
                if(SlotHash == Hash && (Keys[SlotList->Slots[Slot].ItemIndex] == Key))
                    return (int64_t)Slot;
            }
        }        
        
        Slot = (Slot+1) & SlotMask;
    }
    
    return -1;
}

template <typename key, typename value>
void hashmap<key, value>::Add(const key& Key, const value& Value)
{
    Assert(Find_Slot(&SlotList, Keys, Key) < 0);
    if(Length >= (SlotList.SlotCapacity-SlotList.SlotCapacity/3))
        Expand_Slot_List(&SlotList, &Allocator, ItemSlots, SlotList.SlotCapacity*2);
    
    uint32_t SlotMask = SlotList.SlotCapacity-1;
    uint32_t Hash = Hash_Function(Key);
    
    uint32_t BaseSlot = Hash & SlotMask;
    uint32_t BaseCount = SlotList.Slots[BaseSlot].BaseCount;
    uint32_t Slot = BaseSlot;
    uint32_t FirstFree = Slot;
    
    while(BaseCount)
    {
        uint32_t SlotHash = SlotList.Slots[Slot].Hash;
        if(!SlotList.Slots[Slot].IsValid && SlotList.Slots[FirstFree].IsValid) FirstFree = Slot;
        uint32_t SlotBase = SlotHash & SlotMask;
        if(SlotBase == BaseSlot)
            BaseCount--;
        Slot = (Slot+1) & SlotMask;
    }
    
    Slot = FirstFree;
    while(SlotList.Slots[Slot].IsValid)
        Slot = (Slot+1) & SlotMask;
    
    if(Length >= ItemCapacity)
        Expand_Item_List(this, ItemCapacity*2);
    
    Assert(!SlotList.Slots[Slot].IsValid && (Hash & SlotMask) == BaseSlot);
    Assert(Hash);
    
    SlotList.Slots[Slot].Hash = Hash;
    SlotList.Slots[Slot].ItemIndex = Length;
    SlotList.Slots[Slot].IsValid = true;
    SlotList.Slots[BaseSlot].BaseCount++;
    
    ItemSlots[Length] = Slot;
    Keys[Length] = Key;
    Values[Length] = Value;
    
    Length++;
}

template <typename key, typename value>
bool hashmap<key, value>::Contains_Key(const key& Key) const
{
    return Find_Slot(&SlotList, Keys, Key) >= 0;
}

template <typename key, typename value>
value* hashmap<key, value>::Get(const key& Key)
{
    int64_t SlotIndex = Find_Slot(&SlotList, Keys, Key);
    if(SlotIndex < 0) return NULL;
    return &Values[SlotList.Slots[SlotIndex].ItemIndex];
}

template <typename key, typename value>
const value* hashmap<key, value>::Get(const key& Key) const
{
    int64_t SlotIndex = Find_Slot(&SlotList, Keys, Key);
    if(SlotIndex < 0) return NULL;
    return &Values[SlotList.Slots[SlotIndex].ItemIndex];
}

template <typename key, typename value>
value& hashmap<key, value>::operator[](const key& Key)
{
    int64_t SlotIndex = Find_Slot(&SlotList, Keys, Key);
    Assert(SlotIndex >= 0);
    return Values[SlotList.Slots[SlotIndex].ItemIndex];
}

template <typename key, typename value>
const value& hashmap<key, value>::operator[](const key& Key) const
{
    int64_t SlotIndex = Find_Slot(&SlotList, Keys, Key);
    Assert(SlotIndex >= 0);
    return Values[SlotList.Slots[SlotIndex].ItemIndex];
}

template <typename key, typename value>
void hashmap<key, value>::Remove(const key& Key)
{
    int64_t Slot = Find_Slot(&SlotList, Keys, Key);
    Assert(Slot >= 0);
    Assert(SlotList.Slots[Slot].IsValid);
    
    if(Slot >= 0 && SlotList.Slots[Slot].IsValid)
    {
        uint32_t SlotMask = SlotList.SlotCapacity-1;
        uint32_t Hash = SlotList.Slots[Slot].Hash;
        uint32_t BaseSlot = Hash & SlotMask;
        SlotList.Slots[BaseSlot].BaseCount--;
        SlotList.Slots[Slot].Hash = 0;
        SlotList.Slots[Slot].IsValid = false;
        
        uint32_t Index = SlotList.Slots[Slot].ItemIndex;
        uint32_t LastIndex = Length-1;
        
        if(Index != LastIndex)
        {
            Keys[Index] = Keys[LastIndex];
            ItemSlots[Index] = ItemSlots[LastIndex];
            Values[Index] = Values[LastIndex];
            SlotList.Slots[ItemSlots[LastIndex]].ItemIndex = Index;
        }
        
        Length--;
    }
}

template <typename key, typename value>
void hashmap<key, value>::Clear()
{
    Memory_Clear(SlotList.Slots, SlotList.SlotCapacity*sizeof(hash_slot));
    Memory_Clear(ItemSlots, ItemCapacity*(sizeof(uint32_t)+sizeof(key)+sizeof(value)));
    Length = 0;
}

template <typename key>
void hashset<key>::Add(const key& Key)
{
    //Cannot add duplicate keys to hashset
    if(Find_Slot(&SlotList, Keys, Key) >= 0) return;
    
    if(Length >= (SlotList.SlotCapacity-SlotList.SlotCapacity/3))
        Expand_Slot_List(&SlotList, Allocator, ItemSlots, SlotList.SlotCapacity*2);
    
    uint32_t SlotMask = SlotList.SlotCapacity-1;
    uint32_t Hash = Hash_Function(Key);
    
    uint32_t BaseSlot = Hash & SlotMask;
    uint32_t BaseCount = SlotList.Slots[BaseSlot].BaseCount;
    uint32_t Slot = BaseSlot;
    uint32_t FirstFree = Slot;
    
    while(BaseCount)
    {
        uint32_t SlotHash = SlotList.Slots[Slot].Hash;
        if(!SlotList.Slots[Slot].IsValid && SlotList.Slots[FirstFree].IsValid) FirstFree = Slot;
        uint32_t SlotBase = SlotHash & SlotMask;
        if(SlotBase == BaseSlot)
            BaseCount--;
        Slot = (Slot+1) & SlotMask;
    }
    
    Slot = FirstFree;
    while(SlotList.Slots[Slot].IsValid)
        Slot = (Slot+1) & SlotMask;
    
    if(Length >= ItemCapacity)
        Expand_Item_List(this, ItemCapacity*2);
    
    Assert(!SlotList.Slots[Slot].IsValid && (Hash & SlotMask) == BaseSlot);
    Assert(Hash);
    
    SlotList.Slots[Slot].Hash = Hash;
    SlotList.Slots[Slot].ItemIndex = Length;
    SlotList.Slots[Slot].IsValid = true;
    SlotList.Slots[BaseSlot].BaseCount++;
    
    ItemSlots[Length] = Slot;
    Keys[Length] = Key;
    
    Length++;
}

template <typename key>
bool hashset<key>::Contains(const key& Key) const
{
    return Find_Slot(&SlotList, Keys, Key) >= 0;
}

template <typename key>
void hashset<key>::Remove(const key& Key)
{
    int64_t Slot = Find_Slot(&SlotList, Keys, Key);
    Assert(Slot >= 0);
    Assert(SlotList.Slots[Slot].IsValid);
    
    if(Slot >= 0 && SlotList.Slots[Slot].IsValid)
    {
        uint32_t SlotMask = SlotList.SlotCapacity-1;
        uint32_t Hash = SlotList.Slots[Slot].Hash;
        uint32_t BaseSlot = Hash & SlotMask;
        SlotList.Slots[BaseSlot].BaseCount--;
        SlotList.Slots[Slot].Hash = 0;
        SlotList.Slots[Slot].IsValid = false;
        
        uint32_t Index = SlotList.Slots[Slot].ItemIndex;
        uint32_t LastIndex = Length-1;
        
        if(Index != LastIndex)
        {
            Keys[Index] = Keys[LastIndex];
            ItemSlots[Index] = ItemSlots[LastIndex];
            SlotList.Slots[ItemSlots[LastIndex]].ItemIndex = Index;
        }
        
        Length--;
    }
}

template <typename key>
void hashset<key>::Clear()
{
    Memory_Clear(SlotList.Slots, SlotList.SlotCapacity*sizeof(hash_slot));
    Memory_Clear(ItemSlots, ItemCapacity*(sizeof(uint32_t)+sizeof(key)+sizeof(value)));
    Length = 0;
}

template <typename key, typename value>
hashmap<key, value> Create_Hashmap(allocator* Allocator, uint32_t SlotCapacity, uint32_t ItemCapacity)
{
    hashmap<key, value> Result = {};
    Result.Allocator = *Allocator;
    Expand_Slot_List(&Result.SlotList, &Result.Allocator, Result.ItemSlots, SlotCapacity);
    Expand_Item_List(&Result, ItemCapacity);
    return Result;
}

template <typename key>
hashset<key> Create_Hashset(allocator* Allocator, uint32_t SlotCapacity, uint32_t ItemCapacity)
{
    hashset<key> Result = {};
    Result.Allocator = Allocator;
    Expand_Slot_List(&Result.SlotList, Result.Allocator, Result.ItemSlots, SlotCapacity);
    Expand_Item_List(&Result, ItemCapacity);
    return Result;
}

uint32_t Hash_Function(uint32_t Key)
{
    Key = (Key+0x7ed55d16) + (Key<<12);
    Key = (Key^0xc761c23c) ^ (Key>>19);
    Key = (Key+0x165667b1) + (Key<<5);
    Key = (Key+0xd3a2646c) ^ (Key<<9);
    Key = (Key+0xfd7046c5) + (Key<<3);
    Key = (Key^0xb55a4f09) ^ (Key>>16);
    return Key;
}

uint32_t Hash_Function(uint64_t Key)
{
    Key = (~Key) + (Key << 18); // Key = (Key << 18) - Key - 1;
    Key = Key ^ (Key >> 31);
    Key = Key * 21; // Key = (Key + (Key << 2)) + (Key << 4);
    Key = Key ^ (Key >> 11);
    Key = Key + (Key << 6);
    Key = Key ^ (Key >> 22);
    return (uint32_t)Key;
}

uint32_t Hash_Function(const void* Ptr)
{
    return Hash_Function((size_t)Ptr);
}

template <typename type, typename... args>
void Hash_Combine(uint32_t* Result, const type& Type, args... Args)
{
    *Result = Hash_Function(Type) + 0x9e3779b9 + (*Result << 6) + (*Result >> 2);
    (Hash_Combine(Result, Args), ...);
}
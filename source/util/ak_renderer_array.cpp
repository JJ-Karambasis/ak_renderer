template <typename type>
type& array<type>::operator[](uint64_t Index)
{
    Assert(Index < Length);
    return Data[Index];
}

template <typename type>
const type& array<type>::operator[](uint64_t Index) const
{
    Assert(Index < Length);
    return Data[Index];
}


template <typename type>
size_t dynamic_array<type>::Add(const type& Value)
{
    if(Length == Capacity)
    {
        Reserve(Capacity*2);
    }
    
    size_t Result = Length;
    Data[Length++] = Value;
    return Result;
}

template <typename type>
size_t dynamic_array<type>::Add_Range(const type* Values, size_t Count)
{
    if(Length + Count > Capacity)
    {
        size_t NewCapacity = Max(Length+Count, Capacity*2);
        Reserve(NewCapacity);
    }
    
    Memory_Copy(Data+Length, Values, Count*sizeof(type));
    size_t Result = Length;
    Length += Count;
    return Result;
}

template <typename type>
type& dynamic_array<type>::operator[](size_t Index)
{
    Assert(Index < Length);
    return Data[Index];
}

template <typename type>
const type& dynamic_array<type>::operator[](size_t Index) const
{
    Assert(Index < Length);
    return Data[Index];
}

template <typename type>
type* dynamic_array<type>::Get(size_t Index)
{
    return Index < Length ? &Data[Index] : NULL;
}

template <typename type>
const type* dynamic_array<type>::Get(size_t Index) const
{
    return Index < Length ? &Data[Index] : NULL;
}

template <typename type>
void dynamic_array<type>::Reserve(size_t NewCapacity)
{
    if(NewCapacity < Length) NewCapacity = Length;
    
    type* NewData = (type*)Allocator->MemoryAllocate(NewCapacity*sizeof(type), Allocator->UserData);
    Memory_Clear(NewData, NewCapacity*sizeof(type));
    
    if(Data) 
    {
        size_t CopySize = Capacity > NewCapacity ? NewCapacity : Capacity;
        Memory_Copy(NewData, Data, CopySize*sizeof(type));
        Allocator->MemoryFree(Data, Allocator->UserData);
    }
    
    Data = NewData;
    Capacity = NewCapacity;
}

template <typename type>
void dynamic_array<type>::Resize(size_t NewLength)
{
    if(NewLength > Capacity)
        Reserve(NewLength);
    Length = NewLength;
}

template <typename type>
void dynamic_array<type>::Clear()
{
    Length = 0;
}

template <typename type>
bool dynamic_array<type>::Is_Empty() const
{
    return !Data || !Length;
}

template <typename type>
array<type> dynamic_array<type>::To_Array() const
{
    array<type> Result;
    Result.Data = Data;
    Result.Length = Length;
    return Result;
}


template <typename type>
bucket<type>* Get_Current_Bucket(bucket_array<type>* Array)
{
    size_t BucketIndex   = Array->Length / Array->BucketCapacity;
    if(Array->Buckets.Length <= BucketIndex)
    {
        size_t Capacity = BucketIndex ? BucketIndex*2 : 32;
        Array->Buckets.Resize(Capacity);
    }
    
    if(!Array->Buckets[BucketIndex])
    {
        size_t AllocationSize = sizeof(bucket<type>)+sizeof(type)*Array->BucketCapacity;
        bucket<type>* Bucket = (bucket<type>*)Array->Allocator->MemoryAllocate(AllocationSize, Array->Allocator->UserData);
        Memory_Clear(Bucket, AllocationSize);
        
        Bucket->Length = 0;
        Bucket->Data = (type*)(Bucket+1);
        Array->Buckets[BucketIndex] = Bucket;
    }
    
    return Array->Buckets[BucketIndex];
}

template <typename type>
void bucket_array<type>::Add(const type& Value)
{
    bucket<type>* CurrentBucket = Get_Current_Bucket(this);
    if(!CurrentBucket)
    {
        //TODO(JJ): Error logging
        return;
    }
    
    CurrentBucket->Data[CurrentBucket->Length++] = Value;
    Length++;
}

template <typename type>
void bucket_array<type>::Add_Range(const type* Values, size_t Count)
{
    while(Count--) Add(*Values++);
}

template <typename type>
type& bucket_array<type>::operator[](size_t Index)
{
    Assert(Index < Length);
    size_t BucketIndex = Index/BucketCapacity;
    size_t IndexInBucket = Index%BucketCapacity;
    return Buckets[BucketIndex]->Data[IndexInBucket];
}

template <typename type>
const type& bucket_array<type>::operator[](size_t Index) const
{
    Assert(Index < Length);
    size_t BucketIndex = Index/BucketCapacity;
    size_t IndexInBucket = Index%BucketCapacity;
    return Buckets[BucketIndex]->Data[IndexInBucket];
}

template <typename type>
type* bucket_array<type>::Get(size_t Index)
{
    if(Index >= Length) return NULL;
    size_t BucketIndex = Index/BucketCapacity;
    size_t IndexInBucket = Index%BucketCapacity;
    return Buckets[BucketIndex]->Data + IndexInBucket;
}

template <typename type>
const type* bucket_array<type>::Get(size_t Index) const
{
    if(Index >= Length) return NULL;
    size_t BucketIndex = Index/BucketCapacity;
    size_t IndexInBucket = Index%BucketCapacity;
    return Buckets[BucketIndex]->Data + IndexInBucket;
}

template <typename type>
type* bucket_array<type>::operator+(size_t Index)
{
    Assert(Index < Length);
    size_t BucketIndex = Index/BucketCapacity;
    size_t IndexInBucket = Index%BucketCapacity;
    return Buckets[BucketIndex]->Data + IndexInBucket;
}

template <typename type>
const type* bucket_array<type>::operator+(size_t Index) const
{
    Assert(Index < Length);
    size_t BucketIndex = Index/BucketCapacity;
    size_t IndexInBucket = Index%BucketCapacity;
    return Buckets[BucketIndex]->Data + IndexInBucket;
}

template <typename type>
type* bucket_array<type>::Pop()
{
    if(!Length) return NULL;
    Length--;
    size_t BucketIndex = Length/BucketCapacity;
    size_t IndexInBucket = Length%BucketCapacity;
    type* Result = Buckets[BucketIndex]->Data + IndexInBucket;
    Buckets[BucketIndex]->Length--;
    return Result;
}

template <typename type>
type* bucket_array<type>::Last()
{
    if(!Length) return NULL;
    return Get(Length-1);
}

template <typename type>
void bucket_array<type>::Clear()
{
    Length = 0;
    for(bucket<type>* Bucket : Buckets)
    {
        if(Bucket) Bucket->Length = 0;
    }
}

template <typename type>
bool bucket_array<type>::Is_Empty() const
{
    return !Length || Buckets.Is_Empty();
}

template <typename type> 
array<type> Allocate_Array(arena* Arena, uint64_t Count)
{
    array<type> Result = {};
    Result.Data = Arena->Push_Array<type>(Count);
    Result.Length = Count;
    return Result;
}

template <typename type> 
dynamic_array<type> Create_Dynamic_Array(allocator* Allocator, size_t InitialCapacity)
{
    dynamic_array<type> Result = {};
    Result.Allocator = Allocator;
    Result.Reserve(InitialCapacity);
    return Result;
}

template <typename type> 
bucket_array<type> Create_Bucket_Array(allocator* Allocator, size_t InitialBucketCapacity)
{
    bucket_array<type> Result = {};
    Result.Allocator = Allocator;
    Result.BucketCapacity = InitialBucketCapacity;
    Result.Buckets = Create_Dynamic_Array<bucket<type>*>(Result.Allocator, 32);
    return Result;
}
#ifndef AK_RENDERER_ARRAY_H
#define AK_RENDERER_ARRAY_H

template <typename type>
struct array
{
    type*    Data;
    uint64_t Length;
    
    type& operator[](uint64_t Index);
    const type& operator[](uint64_t Index) const;
    
    inline bool Is_Empty() const { return !Data || !Length; }
};

template <typename type>
struct dynamic_array
{
    allocator* Allocator;
    size_t     Capacity;
    size_t     Length;
    type*      Data;
    
    size_t Add(const type& Value);
    size_t Add_Range(const type* Values, size_t Count);
    
    type& operator[](size_t Index);
    const type& operator[](size_t Index) const;
    
    type* Get(size_t Index);
    const type* Get(size_t Index) const;
    
    void Reserve(size_t NewCapacity);
    void Resize(size_t NewLength);
    
    void Clear();
    bool Is_Empty() const;
    array<type> To_Array() const;
};

template <typename type>
struct async_dynamic_array
{
    async_allocator* Allocator;
    size_t           Capacity;
    size_t           Length;
    type*            Data;
    spin_lock        Lock;
    
    size_t Add(const type& Value);
    size_t Add_Range(const type* Values, size_t Count);
    
    type& operator[](size_t Index);
    const type& operator[](size_t Index) const;
    
    type* Get(size_t Index);
    const type* Get(size_t Index) const;
    
    void Reserve(size_t NewCapacity);
    void Resize(size_t NewLength);
    
    void Clear();
    bool Is_Empty() const;
    array<type> To_Array() const;
};

template <typename type>
struct bucket
{
    type*  Data;
    size_t Length;
};

template <typename type>
struct bucket_array
{
    allocator* Allocator;
    size_t     BucketCapacity;
    size_t     Length;
    dynamic_array<bucket<type>*> Buckets;
    
    void Add(const type& Value);
    void Add_Range(const type* Values, size_t Count);
    
    type& operator[](size_t Index);
    const type& operator[](size_t Index) const;
    
    type* Get(size_t Index);
    const type* Get(size_t Index) const;
    
    type* operator+(size_t Index);
    const type* operator+(size_t Index) const;
    
    type* Pop();
    type* Last();
    
    void Clear();
    bool Is_Empty() const;
};

template <typename type>
struct async_bucket_array
{
    async_allocator*                   Allocator;
    size_t                             BucketCapacity;
    size_t                             Length;
    async_dynamic_array<bucket<type>*> Buckets;
    spin_lock                          Lock;
    
    void Add(const type& Value);
    void Add_Range(const type* Values, size_t Count);
    
    type& operator[](size_t Index);
    const type& operator[](size_t Index) const;
    
    type* Get(size_t Index);
    const type* Get(size_t Index) const;
    
    type* operator+(size_t Index);
    const type* operator+(size_t Index) const;
    
    type* Pop();
    type* Last();
    
    void Clear();
    bool Is_Empty() const;
};

template <typename type> array<type> Allocate_Array(arena* Arena, uint64_t Count);
template <typename type> dynamic_array<type> Create_Dynamic_Array(allocator* Allocator, size_t InitialCapacity);
template <typename type> bucket_array<type> Create_Bucket_Array(allocator* Allocator, size_t InitialBucketCapacity);

#endif
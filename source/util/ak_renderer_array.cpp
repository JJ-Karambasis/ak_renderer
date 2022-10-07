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
array<type> Allocate_Array(arena* Arena, uint64_t Count)
{
    array<type> Result = {};
    Result.Data = Arena->Push_Array<type>(Count);
    Result.Length = Count;
    return Result;
}
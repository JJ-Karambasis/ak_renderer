#ifndef AK_RENDERER_ARRAY_H
#define AK_RENDERER_ARRAY_H

template <typename type>
struct array
{
    type*    Data;
    uint64_t Length;
    
    type& operator[](uint64_t Index);
    const type& operator[](uint64_t Index) const;
};
template <typename type> array<type> Allocate_Array(arena* Arena, uint64_t Count);

#endif
#ifndef VK_RENDERER_RESOURCE_MANAGER_H
#define VK_RENDERER_RESOURCE_MANAGER_H

struct vk_async_buffer_to_buffer_copy
{
    VkBuffer     SrcBuffer;
    VkBuffer     DstBuffer;
    VkBufferCopy Copy;
};

struct vk_memory_block
{
    VkDeviceMemory   Memory;
    VkDeviceSize     Size;
    vk_memory_block* Next;
};

struct vk_memory_entry
{
    vk_memory_block* Block;
    VkDeviceSize     Offset;
    VkDeviceSize     Size;
    vk_memory_entry* Next;
    vk_memory_entry* Prev;
};

struct vk_memory_allocation
{
    vk_memory_entry* MemoryEntry;
    
    VkDeviceMemory Get_Memory();
    VkDeviceSize   Get_Size();
    VkDeviceSize   Get_Offset();
};

struct vk_memory_heap
{
    vk_device_context* DeviceContext;
    
    VkMemoryHeap Heap;
    arena* BlockStorage;
    vk_memory_block* HeapBlocks;
    vk_memory_entry* StorageBlocks;
    vk_memory_entry* OrphanBlocks;
    vk_memory_entry* FreeBlocks;
    VkDeviceSize     InitialBlockSize;
    spin_lock Lock;
};

enum 
{
    VK_MEMORY_ALLOCATE_FLAG_CPU_MEMORY = 1,
    VK_MEMORY_ALLOCATE_FLAG_GPU_MEMORY = 2,
};

struct vk_memory_manager
{
    vk_device_context* DeviceContext;
    vk_memory_heap*    MemoryHeaps;
    
    vk_memory_allocation Allocate(const VkMemoryRequirements* MemoryRequirements);
    void Free(vk_memory_allocation* Allocation);
};

struct vk_buffer_heap_block
{
    VkBuffer     Buffer;
    VkDeviceSize Size;
    vk_buffer_heap_block* Next;
};

struct vk_buffer_heap_entry
{
    vk_buffer_heap_block* Block;
    VkDeviceSize          Offset;
    VkDeviceSize          Size;
    vk_buffer_heap_entry* Next;
    vk_buffer_heap_entry* Prev;
};

struct vk_buffer_heap_allocation
{
    vk_buffer_heap_entry* HeapEntry;
    VkDeviceMemory Get_Memory();
    VkDeviceSize   Get_Memory_Offset();
    VkBuffer       Get_Buffer();
    VkDeviceSize   Get_Size();
    VkDeviceSize   Get_Offset();
};

struct vk_async_buffer_heap
{
    vk_device_context* DeviceContext;
    
    arena* BlockStorage;
    vk_buffer_heap_block* HeapBlocks;
    
    vk_buffer_heap_entry* StorageBlocks;
    vk_buffer_heap_entry* OrphanBlocks;
    vk_buffer_heap_entry* FreeBlocks;
    VkBufferUsageFlags BlockUsage;
    VkDeviceSize DefaultBlockSize;
    spin_lock HeapLock;
    
    vk_buffer_heap_allocation Allocate(VkDeviceSize Size);
    void Free(vk_buffer_heap_allocation* Allocation);
};

struct vk_buffer_arena_block
{
    VkBuffer             Buffer;
    vk_memory_allocation MemoryAllocation;
    VkDeviceSize Used;
    VkDeviceSize Size;
    vk_buffer_arena_block* Next;
};

struct vk_buffer_arena_allocation
{
    vk_buffer_arena_block* Block;
    VkDeviceSize           Size;
    
    VkDeviceMemory Get_Memory();
    VkDeviceSize   Get_Memory_Offset();
    VkBuffer       Get_Buffer();
    VkDeviceSize   Get_Offset();
    VkDeviceSize   Get_Size();
};

struct vk_async_buffer_arena
{
    vk_device_context* DeviceContext;
    
    arena* BlockStorage;
    vk_buffer_arena_block* FirstBlock;
    vk_buffer_arena_block* CurrentBlock;
    VkBufferUsageFlags BlockUsage;
    VkDeviceSize DefaultBlockSize;
    spin_lock ArenaLock;
    
    vk_buffer_arena_allocation Push(VkDeviceSize Size);
    void Clear();
};

struct vk_mesh
{
    vk_buffer_heap_allocation VertexBuffer;
    vk_buffer_heap_allocation IndexBuffer;
    ak_vertex_format          VertexFormat;
    ak_index_format           IndexFormat;
    uint32_t                  VertexCount;
    uint32_t                  IndexCount;
};

struct vk_resource_manager : public ak_resource_manager
{
    vk_device_context*                                  DeviceContext;
    vk_memory_manager                                   MemoryManager;
    vk_async_buffer_arena*                              UploadArenas;
    async_bucket_array<vk_async_buffer_to_buffer_copy>* AsyncBufferCopies;
    vk_async_buffer_heap                                VertexBufferHeap;
    vk_async_buffer_heap                                IndexBufferHeap;
    async_pool<vk_mesh>                                 Meshes;
    
    ak_handle<ak_mesh> Create_Mesh(ak_vertex_format VertexFormat, uint32_t VertexCount, ak_index_format IndexFormat, uint32_t IndexCount) final;
    void Upload_Mesh(ak_handle<ak_mesh> Mesh, void* Vertices, uint32_t VertexCount, uint32_t VertexOffset, void* Indices, uint32_t IndexCount, uint32_t IndexOffset) final;
};

vk_async_buffer_arena Create_Async_Buffer_Arena(vk_device_context* DeviceContext, VkDeviceSize DefaultBlockSize, VkBufferUsageFlags Usage);
vk_async_buffer_heap Create_Async_Buffer_Heap(vk_device_context* DeviceContext, VkDeviceSize DefaultBlockSize, VkBufferUsageFlags Usage);

bool VK_Init_Memory_Manager(vk_device_context* DeviceContext);
bool VK_Init_Resource_Manager(vk_device_context* DeviceContext);


#endif
VkDeviceMemory vk_memory_allocation::Get_Memory()
{
    return MemoryEntry->Block->Memory;
}

VkDeviceSize vk_memory_allocation::Get_Size()
{
    return MemoryEntry->Size;
}

VkDeviceSize vk_memory_allocation::Get_Offset()
{
    return MemoryEntry->Offset;
}

vk_memory_allocation vk_memory_manager::Allocate(const VkMemoryRequirements* MemoryRequirements)
{
    
}

void vk_memory_manager::Free(vk_memory_allocation* Allocation)
{
}

ak_handle<ak_mesh> vk_resource_manager::Create_Mesh(ak_vertex_format VertexFormat, uint32_t VertexCount, ak_index_format IndexFormat, uint32_t IndexCount)
{
#if 0 
    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = GVertexFormatSizes[VertexFormat]*VertexCount;
    BufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkBuffer VertexBuffer;
    VK_Warning_Check(vkCreateBuffer(DeviceContext->Device, &BufferCreateInfo, VK_NULL_HANDLE, &Buffer))
#endif
    
    vk_buffer_heap_allocation VertexBuffer = VertexBufferHeap.Allocate(G_AKVertexFormatSizes[VertexFormat]*VertexCount);
    Bool_Warning_Check(VertexBuffer.Get_Buffer(), Str8_Lit("Failed to allocate the vertex buffer"));
    
    vk_buffer_heap_allocation IndexBuffer = IndexBufferHeap.Allocate(G_AKIndexFormatSizes[IndexFormat]*IndexCount);
    Bool_Warning_Check(IndexBuffer.Get_Buffer(), Str8_Lit("Failed to allocate the index buffer"));
    
    async_handle<vk_mesh> Handle = Meshes.Allocate();
    
    vk_mesh* Mesh = Meshes.Lock(Handle);
    Mesh->VertexBuffer = VertexBuffer;
    Mesh->IndexBuffer  = IndexBuffer;
    Mesh->VertexFormat = VertexFormat;
    Mesh->IndexFormat  = IndexFormat;
    Mesh->VertexCount  = VertexCount;
    Mesh->IndexCount   = IndexCount;
    Meshes.Unlock(Handle);
    
    ak_handle<ak_mesh> Result = {Handle.ID.ID};
    return Result;
}

void vk_resource_manager::Upload_Mesh(ak_handle<ak_mesh> Mesh, void* Vertices, uint32_t VertexCount, uint32_t VertexOffset, void* Indices, uint32_t IndexCount, uint32_t IndexOffset)
{
    async_handle<vk_mesh> Handle = {};
    Handle.ID.ID = Mesh.ID;
    
    vk_mesh* VKMesh = Meshes.Lock(Handle);
    if(VKMesh)
    {
        size_t VerticesSize = VertexCount*G_AKVertexFormatSizes[VKMesh->VertexFormat];
        size_t IndicesSize = IndexCount*G_AKIndexFormatSizes[VKMesh->IndexFormat];
        
        vk_async_buffer_arena*     UploadArena = UploadArenas + DeviceContext->ClientFrameIndex;
        vk_buffer_arena_allocation UploadAllocation = UploadArena->Push(VerticesSize+IndicesSize);
        
        uint8_t* MappedMemory;
        if(vkMapMemory(DeviceContext->Device, UploadAllocation.Get_Memory(), UploadAllocation.Get_Memory_Offset(), UploadAllocation.Get_Size(), 0, (void**)&MappedMemory) == VK_SUCCESS)
        {
            Memory_Copy(MappedMemory, Vertices, VerticesSize);
            Memory_Copy(MappedMemory+VerticesSize, Indices, IndicesSize);
            
            vkUnmapMemory(DeviceContext->Device, UploadAllocation.Get_Memory());
            
            vk_async_buffer_to_buffer_copy Copies[2] = {};
            
            Copies[0].SrcBuffer = UploadAllocation.Get_Buffer();
            Copies[0].DstBuffer = VKMesh->VertexBuffer.Get_Buffer();
            Copies[0].Copy.srcOffset = UploadAllocation.Get_Offset();
            Copies[0].Copy.dstOffset = VKMesh->VertexBuffer.Get_Offset();
            Copies[0].Copy.size = VerticesSize;
            
            Copies[1].SrcBuffer = UploadAllocation.Get_Buffer();
            Copies[1].DstBuffer = VKMesh->IndexBuffer.Get_Buffer();
            Copies[1].Copy.srcOffset = UploadAllocation.Get_Offset()+VerticesSize;
            Copies[1].Copy.dstOffset = VKMesh->IndexBuffer.Get_Offset();
            Copies[1].Copy.size = IndicesSize;
            
            AsyncBufferCopies[DeviceContext->ClientFrameIndex].Add_Range(Copies, 2);
        }
        else
        {
            Warning_Log(Str8_Lit("Failed to map the mesh upload memory for writing"));
        }
    }
    Meshes.Unlock(Handle);
}

vk_async_buffer_heap Create_Async_Buffer_Heap(vk_device_context* DeviceContext, VkDeviceSize DefaultBlockSize, VkBufferUsageFlags Usage)
{
    vk_async_buffer_heap Result = {};
    Result.DeviceContext = DeviceContext;
    Result.BlockStorage = Create_Arena(Get_Thread_Context()->MainArena, Kilobyte(8));
    Result.BlockUsage = Usage;
    Result.DefaultBlockSize = DefaultBlockSize;
    return Result;
}

bool VK_Init_Memory_Manager(vk_device_context* DeviceContext)
{
    thread_context* ThreadContext = Get_Thread_Context();
    
    vk_memory_manager* MemoryManager = &DeviceContext->ResourceManager.MemoryManager;
    MemoryManager->DeviceContext = DeviceContext;
    MemoryManager->MemoryHeaps = ThreadContext->MainArena->Push_Array<vk_memory_heap>(DeviceContext->Device->MemoryProperties.memoryHeapCount);
    
    for(uint32_t MemoryHeapIndex = 0; MemoryHeapIndex < DeviceContext->Device->MemoryProperties.memoryHeapCount; MemoryHeapIndex++)
    {
        vk_memory_heap* Heap = MemoryManager->MemoryHeaps + MemoryHeapIndex;
        Heap->DeviceContext = DeviceContext;
        Heap->Heap = DeviceContext->Device->MemoryProperties.memoryHeaps[MemoryHeapIndex];
        Heap->BlockStorage = Create_Arena(ThreadContext->MainArena, Kilobyte(8));
        Heap->InitialBlockSize = Min(Megabyte(64), Heap->Heap.size);
    }
    
    return true;
}

bool VK_Init_Resource_Manager(vk_device_context* DeviceContext)
{
    vk_resource_manager* ResourceManager = &DeviceContext->ResourceManager;
    ResourceManager->DeviceContext = DeviceContext;
    Bool_Warning_Check(VK_Init_Memory_Manager(DeviceContext), Str8_Lit("Failed to initialize the vulkan memory manager"));
    
    ResourceManager->VertexBufferHeap = Create_Async_Buffer_Heap(DeviceContext, Megabyte(64), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    ResourceManager->Meshes = Create_Async_Pool<vk_mesh>(Get_Thread_Context()->MainArena, 64);
    return true;
}
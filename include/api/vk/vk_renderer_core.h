#ifndef VK_RENDERER_CORE_H
#define VK_RENDERER_CORE_H

struct vk_sharing_mode
{
    VkSharingMode Mode;
    uint32_t      QueueFamilyIndexCount;
    uint32_t      QueueFamilyIndices[4];
};

struct vk_extension_manager
{
    array<const char*> ExtensionsList;
    hashset<str8> ExtensionMap;
    
    void Init(arena* Arena, VkExtensionProperties* Extensions, uint32_t ExtensionCount);
    void Init(arena* Arena, VkLayerProperties* Layers, uint32_t LayerCount);
    bool Add_If_Exists(const str8& Extension);
};

struct vk_device : public ak_device
{
    VkPhysicalDevice PhysicalDevice;
    VkPhysicalDeviceProperties Properties;
    uint32_t GraphicsQueueFamilyIndex;
    uint32_t TransferQueueFamilyIndex;
    uint32_t ComputeQueueFamilyIndex;
    uint32_t PresentQueueFamilyIndex;
};

struct vk_device_context : public ak_device_context
{
    VkDevice Device;
    VkQueue  GraphicsQueue;
    VkQueue  PresentQueue;
    VkQueue  AsyncTransferQueue;
    VkQueue  AsyncComputeQueue;
    
    vk_device* PhysicalDevice;
    vk_resource_manager ResourceManager;
    vk_display_manager  DisplayManager;
    
    uint32_t FrameCount;
    uint32_t ClientFrameIndex;
    
    ak_device* Get_Device() final;
    ak_resource_manager* Get_Resource_Manager() final;
    ak_display_manager* Get_Display_Manager() final;
    
    ak_cmd_buffer* Allocate_Command_Buffer() final;
    bool Execute(const ak_cmd_buffer* const* CmdBuffers, uint32_t CmdBufferCount) final;
};

static bool VK_Create_Instance();
static bool VK_Load_Devices();
static void VK_Delete_Device_Context();
static vk_device_context* VK_Create_Device_Context(vk_device* Device);

template <typename... args>
vk_sharing_mode VK_Query_Sharing_Mode(args... Args);

#endif
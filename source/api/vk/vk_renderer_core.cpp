ak_device* vk_device_context::Get_Device()
{
    return PhysicalDevice;
}

ak_resource_manager* vk_device_context::Get_Resource_Manager()
{
    return &ResourceManager;
}

ak_display_manager* vk_device_context::Get_Display_Manager()
{
    return &DisplayManager;
}

ak_cmd_buffer* vk_device_context::Allocate_Command_Buffer() 
{
    return ::Allocate_Command_Buffer();
}

void vk_extension_manager::Init(arena* Arena, VkExtensionProperties* Extensions, uint32_t ExtensionCount)
{
    ExtensionsList = Allocate_Array<const char*>(Arena, ExtensionCount);
    ExtensionsList.Length = 0;
    ExtensionMap = Create_Hashset<str8>(Arena);
    
    for(uint32_t ExtensionIndex = 0; ExtensionIndex < ExtensionCount; ExtensionIndex++)
    {
        ExtensionMap.Add(Str8((const uint8_t*)Extensions[ExtensionIndex].extensionName));
    }
}

void vk_extension_manager::Init(arena* Arena, VkLayerProperties* Layers, uint32_t LayerCount)
{
    ExtensionsList = Allocate_Array<const char*>(Arena, LayerCount);
    ExtensionsList.Length = 0;
    ExtensionMap = Create_Hashset<str8>(Arena);
    
    for(uint32_t LayerIndex = 0; LayerIndex < LayerCount; LayerIndex++)
    {
        ExtensionMap.Add(Str8((const uint8_t*)Layers[LayerIndex].layerName));
    }
}

bool vk_extension_manager::Add_If_Exists(const str8& Extension)
{
    bool Result = ExtensionMap.Contains(Extension);
    if(Result) ExtensionsList[ExtensionsList.Length++] = (const char*)Extension.Str;
    return Result;
}

static bool VK_Get_Instance_Layers(array<const char*>* Results)
{
    arena* Scratch = Get_Thread_Context()->ScratchArena;
    
    uint32_t LayerCount;
    VK_Warning_Check(vkEnumerateInstanceLayerProperties(&LayerCount, NULL), Str8_Lit("Failed to retrieve the amount of instance layer properties"));
    
    VkLayerProperties* Layers = Scratch->Push_Array<VkLayerProperties>(LayerCount);
    VK_Warning_Check(vkEnumerateInstanceLayerProperties(&LayerCount, Layers), Str8_Lit("Failed to retrieve the instance layer properties"));
    
    vk_extension_manager Manager = {};
    Manager.Init(Scratch, Layers, LayerCount);
    
    Manager.Add_If_Exists(Str8_Lit("VK_LAYER_KHRONOS_validation"));
    
    *Results = Manager.ExtensionsList;
    
    return true;
}

static bool VK_Get_Instance_Extensions(array<const char*>* Results)
{
    arena* Scratch = Get_Thread_Context()->ScratchArena;
    
    uint32_t ExtensionCount;
    VK_Warning_Check(vkEnumerateInstanceExtensionProperties(NULL, &ExtensionCount, NULL), Str8_Lit("Failed to retrieve the amount of instance extension properties"));
    
    VkExtensionProperties* Extensions = Scratch->Push_Array<VkExtensionProperties>(ExtensionCount);
    VK_Warning_Check(vkEnumerateInstanceExtensionProperties(NULL, &ExtensionCount, Extensions), Str8_Lit("Failed to retrieve the instance extension properties"));
    
    vk_extension_manager Manager = {};
    Manager.Init(Scratch, Extensions, ExtensionCount);
    
    Manager.Add_If_Exists(Str8((const uint8_t*)VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
    
    Bool_Warning_Check(Manager.Add_If_Exists(Str8((const uint8_t*)VK_KHR_SURFACE_EXTENSION_NAME)), Str8_Lit("Invalid vulkan instance. Does not support the %s extension."), VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    Bool_Warning_Check(Manager.Add_If_Exists(Str8((const uint8_t*)VK_KHR_WIN32_SURFACE_EXTENSION_NAME)), Str8_Lit("Invalid vulkan instance. Does not support the %s extension."), VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
#error Not Implemented
#endif
    
    *Results = Manager.ExtensionsList;
    
    return true;
}

static bool VK_Get_Device_Extensions(VkPhysicalDevice Device, array<const char*>* Results)
{
    arena* Scratch = Get_Thread_Context()->ScratchArena;
    
    uint32_t ExtensionCount;
    VK_Warning_Check(vkEnumerateDeviceExtensionProperties(Device, NULL, &ExtensionCount, NULL), Str8_Lit("Failed to retrieve the amount of instance extension properties"));
    
    VkExtensionProperties* Extensions = Scratch->Push_Array<VkExtensionProperties>(ExtensionCount);
    VK_Warning_Check(vkEnumerateDeviceExtensionProperties(Device, NULL, &ExtensionCount, Extensions), Str8_Lit("Failed to retrieve the instance extension properties"));
    
    vk_extension_manager Manager = {};
    Manager.Init(Scratch, Extensions, ExtensionCount);
    Bool_Warning_Check(Manager.Add_If_Exists(Str8((const uint8_t*)VK_KHR_SWAPCHAIN_EXTENSION_NAME)), Str8_Lit("Invalid vulkan device. Does not support the %s extension."), VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    
    *Results = Manager.ExtensionsList;
    
    return true;
}

static bool VK_Create_Instance()
{
    array<const char*> Layers;
    Bool_Warning_Check(VK_Get_Instance_Layers(&Layers), Str8_Lit("Failed to get the instance layers"));
    
    array<const char*> Extensions;
    Bool_Warning_Check(VK_Get_Instance_Extensions(&Extensions), Str8_Lit("Failed to get the instance extensions"));
    
    VkApplicationInfo ApplicationInfo = {};
    ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ApplicationInfo.pApplicationName = "Origin_Engine";
    ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    ApplicationInfo.pEngineName = "Origin_Engine";
    ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    ApplicationInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo InstanceCreateInfo = {};
    InstanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
    InstanceCreateInfo.enabledLayerCount       = SafeU32(Layers.Length);
    InstanceCreateInfo.ppEnabledLayerNames     = Layers.Data;
    InstanceCreateInfo.enabledExtensionCount   = SafeU32(Extensions.Length);
    InstanceCreateInfo.ppEnabledExtensionNames = Extensions.Data;
    
    vk_context* Context = VK_Get_Context();
    VK_Warning_Check(vkCreateInstance(&InstanceCreateInfo, VK_NULL_HANDLE, &Context->Instance), 
                     Str8_Lit("Failed to create the vulkan instance"));
    
    return true;
}

static bool VK_Load_Devices()
{
    thread_context* ThreadContext = Get_Thread_Context();
    arena* Scratch = ThreadContext->ScratchArena;
    
    vk_context* Context = VK_Get_Context();
    
    uint32_t PhysicalDeviceCount;
    VK_Warning_Check(vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount, VK_NULL_HANDLE), 
                     Str8_Lit("Failed to get the physical device count"));
    
    VkPhysicalDevice* PhysicalDevices = Scratch->Push_Array<VkPhysicalDevice>(PhysicalDeviceCount);
    VK_Warning_Check(vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount, PhysicalDevices), 
                     Str8_Lit("Failed to get the physical devices count"));
    
    Context->Devices.Devices = ThreadContext->MainArena->Push_Array<ak_device*>(PhysicalDeviceCount);
    Context->Devices.DeviceCount = 0;
    
    Bool_Warning_Check(VK_Begin_Presentation_Engine_Detection_Support(), Str8_Lit("Failed to begin presentation engine detection support"));
    
    for(uint32_t PhysicalDeviceIndex = 0; PhysicalDeviceIndex < PhysicalDeviceCount; PhysicalDeviceIndex++)
    {
        VkPhysicalDeviceProperties Properties;
        vkGetPhysicalDeviceProperties(PhysicalDevices[PhysicalDeviceIndex], &Properties);
        
        uint32_t QueueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[PhysicalDeviceIndex], &QueueFamilyCount, VK_NULL_HANDLE);
        
        VkQueueFamilyProperties* QueueFamilies = Scratch->Push_Array<VkQueueFamilyProperties>(QueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevices[PhysicalDeviceIndex], &QueueFamilyCount, QueueFamilies);
        
        uint32_t GraphicsQueueFamilyIndex = (uint32_t)-1;
        uint32_t TransferQueueFamilyIndex = (uint32_t)-1;
        uint32_t ComputeQueueFamilyIndex  = (uint32_t)-1;
        
        uint32_t AsyncTransferQueueFamilyIndex = (uint32_t)-1;
        uint32_t AsyncComputeQueueFamilyIndex = (uint32_t)-1;
        
        for(uint32_t QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; QueueFamilyIndex++)
        {
            bool HasGraphics = QueueFamilies[QueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT;
            bool HasCompute = QueueFamilies[QueueFamilyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT;
            bool HasTransfer = QueueFamilies[QueueFamilyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT;
            
            if(HasGraphics) GraphicsQueueFamilyIndex = QueueFamilyIndex;
            if(HasTransfer && !HasCompute && !HasGraphics) AsyncTransferQueueFamilyIndex = QueueFamilyIndex;
            if(HasCompute && !HasGraphics) AsyncComputeQueueFamilyIndex = QueueFamilyIndex;
        }
        
        if(AsyncComputeQueueFamilyIndex == (uint32_t)-1)
        {
            //NOTE(EVERYONE): If we do not support async compute operations. Find the queue
            //family index that supports both compute and graphics operations
            for(uint32_t QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; QueueFamilyIndex++)
            {
                bool HasGraphics = QueueFamilies[QueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT;
                bool HasCompute = QueueFamilies[QueueFamilyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT;
                if(HasGraphics && HasCompute)
                {
                    GraphicsQueueFamilyIndex = QueueFamilyIndex;
                    ComputeQueueFamilyIndex = QueueFamilyIndex;
                    break;
                }
            }
        }
        else
            ComputeQueueFamilyIndex = AsyncComputeQueueFamilyIndex;
        
        if(AsyncTransferQueueFamilyIndex == (uint32_t)-1)
            TransferQueueFamilyIndex = GraphicsQueueFamilyIndex;
        else
            TransferQueueFamilyIndex = AsyncTransferQueueFamilyIndex;
        
        if(GraphicsQueueFamilyIndex == (uint32_t)-1 ||
           TransferQueueFamilyIndex == (uint32_t)-1 ||
           ComputeQueueFamilyIndex == (uint32_t)-1)
        {
            //TODO(JJ): Add warning logging
        }
        else
        {
            uint32_t PresentQueueFamilyIndex = (uint32_t)-1;
            if(VK_Presentation_Engine_Queue_Family_Support(PhysicalDevices[PhysicalDeviceIndex], GraphicsQueueFamilyIndex))
                PresentQueueFamilyIndex = GraphicsQueueFamilyIndex;
            
            if(PresentQueueFamilyIndex == (uint32_t)-1)
            {
                if(VK_Presentation_Engine_Queue_Family_Support(PhysicalDevices[PhysicalDeviceIndex], ComputeQueueFamilyIndex))
                    PresentQueueFamilyIndex = GraphicsQueueFamilyIndex;
            }
            
            if(PresentQueueFamilyIndex == (uint32_t)-1)
            {
                for(uint32_t QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; QueueFamilyIndex++)
                {
                    if(VK_Presentation_Engine_Queue_Family_Support(PhysicalDevices[PhysicalDeviceIndex], QueueFamilyIndex))
                    {
                        PresentQueueFamilyIndex = QueueFamilyIndex;
                        break;
                    }
                }
            }
            
            if(PresentQueueFamilyIndex == (uint32_t)-1)
            {
                //TODO(JJ): Add warning logging
            }
            else
            {
                vk_device* Device = new(ThreadContext->MainArena) vk_device;
                Device->PhysicalDevice = PhysicalDevices[PhysicalDeviceIndex];
                Device->Properties = Properties;
                
                Device->GraphicsQueueFamilyIndex = GraphicsQueueFamilyIndex;
                Device->TransferQueueFamilyIndex = TransferQueueFamilyIndex;
                Device->ComputeQueueFamilyIndex = ComputeQueueFamilyIndex;
                Device->PresentQueueFamilyIndex = PresentQueueFamilyIndex;
                Context->Devices.Devices[Context->Devices.DeviceCount++] = Device;
            }
        }
    }
    
    VK_End_Presentation_Engine_Detection_Support();
    
    return true;
}

static void VK_Delete_Device_Context()
{
}

static vk_device_context* VK_Create_Device_Context(vk_device* Device)
{
    vk_context* Context = VK_Get_Context();
    vk_device_context* DeviceContext = &Context->DeviceContext;
    
    if(DeviceContext->Device)
        VK_Delete_Device_Context();
    
    array<const char*> Extensions;
    Bool_Warning_Check(VK_Get_Device_Extensions(Device->PhysicalDevice, &Extensions), Str8_Lit("Failed to get the device extensions"));
    
    uint32_t QueueCreateInfoCount = 0;
    float Priority = 1.0f;
    
    VkDeviceQueueCreateInfo QueueCreateInfos[4] = {};
    
#define Add_Queue_Family_Index(index) \
do \
{ \
QueueCreateInfos[QueueCreateInfoCount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; \
QueueCreateInfos[QueueCreateInfoCount].queueFamilyIndex = index; \
QueueCreateInfos[QueueCreateInfoCount].queueCount = 1; \
QueueCreateInfos[QueueCreateInfoCount].pQueuePriorities = &Priority; \
QueueCreateInfoCount++; \
} while(0)
    
    Add_Queue_Family_Index(Device->GraphicsQueueFamilyIndex);
    
    if(Device->TransferQueueFamilyIndex != Device->GraphicsQueueFamilyIndex)
        Add_Queue_Family_Index(Device->TransferQueueFamilyIndex);
    
    if(Device->ComputeQueueFamilyIndex != Device->GraphicsQueueFamilyIndex)
        Add_Queue_Family_Index(Device->ComputeQueueFamilyIndex);
    
    if(Device->PresentQueueFamilyIndex != Device->GraphicsQueueFamilyIndex &&
       Device->PresentQueueFamilyIndex != Device->TransferQueueFamilyIndex &&
       Device->PresentQueueFamilyIndex != Device->ComputeQueueFamilyIndex)
    {
        Add_Queue_Family_Index(Device->PresentQueueFamilyIndex);
    }
#undef Add_Queue_Family_Index
    
    VkDeviceCreateInfo DeviceCreateInfo = {};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.queueCreateInfoCount = QueueCreateInfoCount;
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos;
    DeviceCreateInfo.enabledExtensionCount = SafeU32(Extensions.Length);
    DeviceCreateInfo.ppEnabledExtensionNames = Extensions.Data;
    
    VK_Warning_Check(vkCreateDevice(Device->PhysicalDevice, &DeviceCreateInfo, VK_NULL_HANDLE, &DeviceContext->Device), Str8_Lit("Failed to create the vulkan device"));
    
    Bool_Warning_Check(VK_Load_Device_Functions(DeviceContext->Device), Str8_Lit("Failed to load the device functions"));
    
    vkGetDeviceQueue(DeviceContext->Device, Device->GraphicsQueueFamilyIndex, 0, &DeviceContext->GraphicsQueue);
    
    if(Device->TransferQueueFamilyIndex != Device->GraphicsQueueFamilyIndex)
        vkGetDeviceQueue(DeviceContext->Device, Device->TransferQueueFamilyIndex, 0, &DeviceContext->AsyncTransferQueue);
    
    if(Device->ComputeQueueFamilyIndex != Device->GraphicsQueueFamilyIndex)
        vkGetDeviceQueue(DeviceContext->Device, Device->ComputeQueueFamilyIndex, 0, &DeviceContext->AsyncComputeQueue);
    
    if(Device->PresentQueueFamilyIndex == Device->GraphicsQueueFamilyIndex) DeviceContext->PresentQueue = DeviceContext->GraphicsQueue;
    else if(Device->PresentQueueFamilyIndex == Device->TransferQueueFamilyIndex) DeviceContext->PresentQueue = DeviceContext->AsyncTransferQueue;
    else if(Device->PresentQueueFamilyIndex == Device->ComputeQueueFamilyIndex) DeviceContext->PresentQueue = DeviceContext->AsyncComputeQueue;
    else vkGetDeviceQueue(DeviceContext->Device, Device->PresentQueueFamilyIndex, 0, &DeviceContext->PresentQueue);
    
    Bool_Warning_Check(VK_Init_Display_Manager(DeviceContext), Str8_Lit("Failed to initialize the vulkan display manager"));
    Bool_Warning_Check(VK_Init_Resource_Manager(DeviceContext), 
                       Str8_Lit("Failed to initialize the vulkan resource manager"));
    
    DeviceContext->PhysicalDevice = Device;
    
    return DeviceContext;
}
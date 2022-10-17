#ifndef VK_RENDERER_RESOURCE_MANAGER_H
#define VK_RENDERER_RESOURCE_MANAGER_H

struct vk_material
{
    VkPipeline Pipeline;
};

struct vk_resource_manager : public ak_resource_manager
{
    vk_device_context*      DeviceContext;
    async_pool<vk_material> Materials;
    
    ak_handle<ak_material> Create_Material(const char* Source, size_t SourceSize) final;
};

bool VK_Init_Resource_Manager(vk_device_context* DeviceContext);

#endif
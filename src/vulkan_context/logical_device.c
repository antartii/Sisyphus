#include "vulkan_context/logical_device.h"
static const char *ssp_vulkan_device_extensions_name[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
    VK_KHR_SPIRV_1_4_EXTENSION_NAME,
};

static const size_t ssp_vulkan_device_extensions_name_size = ARRAY_SIZE(ssp_vulkan_device_extensions_name);

enum SSP_ERROR_CODE ssp_vulkan_create_logical_device(
    struct SSPVulkanContextExtFunc *ext_func,
    struct SSPWindow *window,
    VkPhysicalDevice physical_device,
    VkDevice *logical_device,
    VkSurfaceKHR surface,
    struct SSPVulkanQueueFamiliesIndices *queue_family_indices,
    VkQueue *transfer_queue,
    VkQueue *present_queue,
    VkQueue *graphic_queue)
{
    enum SSP_ERROR_CODE error_code;
    
    uint32_t queue_family_properties_count;
    ext_func->vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, NULL);
    VkQueueFamilyProperties *queue_family_properties = malloc(sizeof(VkQueueFamilyProperties) * queue_family_properties_count);
    ext_func->vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_properties_count, queue_family_properties);

    if (error_code = ssp_vulkan_get_queue_families_indices(ext_func, physical_device, surface, queue_family_properties, queue_family_properties_count, queue_family_indices)) {
        free(queue_family_properties);
        return error_code;
    }

    VkDeviceQueueCreateInfo *queue_create_info = calloc(queue_family_indices->queue_count, sizeof(VkDeviceQueueCreateInfo));
    int queue_count = queue_family_indices->queue_count;
    float queue_priorities = 1.0f;

    for (int i = 0; i < queue_family_indices->queue_count; ++i) {
        queue_create_info[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info[i].queueCount = 1;
        queue_create_info[i].pQueuePriorities = &queue_priorities;
    }

    int index = 0;
    queue_create_info[index++].queueFamilyIndex = queue_family_indices->graphic;
    if (queue_family_indices->graphic != queue_family_indices->present)
        queue_create_info[index++].queueFamilyIndex = queue_family_indices->present;
    if (queue_family_indices->transfer != queue_family_indices->graphic)
        queue_create_info[index++].queueFamilyIndex = queue_family_indices->transfer;

    free(queue_family_properties);

    VkPhysicalDeviceSynchronization2FeaturesKHR sync_2_features = {0};
    sync_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    sync_2_features.synchronization2 = VK_TRUE;

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = {0};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;
    dynamic_rendering_features.pNext = &sync_2_features;

    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = queue_count;
    create_info.pQueueCreateInfos = queue_create_info;
    create_info.enabledExtensionCount = ssp_vulkan_device_extensions_name_size;
    create_info.ppEnabledExtensionNames = ssp_vulkan_device_extensions_name;
    create_info.pNext = &dynamic_rendering_features;

    if (ext_func->vkCreateDevice(physical_device, &create_info, NULL, logical_device) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_LOGICAL_DEVICE_CREATION;
    
    if ((error_code = ssp_vulkan_ext_func_device(ext_func, *logical_device)))
        return error_code;

    free(queue_create_info);

    ext_func->vkGetDeviceQueue(*logical_device, queue_family_indices->graphic, 0, graphic_queue);
    if (queue_family_indices->graphic != queue_family_indices->present)
        ext_func->vkGetDeviceQueue(*logical_device, queue_family_indices->present, 0, present_queue);
    else
        *present_queue = *graphic_queue;

    if (queue_family_indices->transfer != queue_family_indices->graphic)
        ext_func->vkGetDeviceQueue(*logical_device, queue_family_indices->transfer, 0, transfer_queue);
    else
        *transfer_queue = *graphic_queue;

    return SSP_ERROR_CODE_SUCCESS;
}

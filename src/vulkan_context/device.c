#include "vulkan_context/device.h"

static const char *ssp_vulkan_device_extensions_name[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
    VK_KHR_SPIRV_1_4_EXTENSION_NAME,
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME
};

static const size_t ssp_vulkan_device_extensions_name_size = ARRAY_SIZE(ssp_vulkan_device_extensions_name);

enum SSP_ERROR_CODE ssp_vulkan_create_logical_device(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, struct SSPVulkanDevice *device, VkSurfaceKHR surface)
{
    enum SSP_ERROR_CODE error_code;
    
    uint32_t queue_family_properties_count;
    ext_func->vkGetPhysicalDeviceQueueFamilyProperties(device->physical_device, &queue_family_properties_count, NULL);
    VkQueueFamilyProperties *queue_family_properties = malloc(sizeof(VkQueueFamilyProperties) * queue_family_properties_count);
    ext_func->vkGetPhysicalDeviceQueueFamilyProperties(device->physical_device, &queue_family_properties_count, queue_family_properties);

    if (error_code = ssp_vulkan_get_queue_families_indices(ext_func, device, surface, queue_family_properties, queue_family_properties_count)) {
        free(queue_family_properties);
        return error_code;
    }

    VkDeviceQueueCreateInfo *queue_create_info = calloc(device->queue_family_indices.queue_count, sizeof(VkDeviceQueueCreateInfo));
    int queue_count = device->queue_family_indices.queue_count;
    float queue_priorities = 1.0f;

    for (int i = 0; i < device->queue_family_indices.queue_count; ++i) {
        queue_create_info[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info[i].queueCount = 1;
        queue_create_info[i].pQueuePriorities = &queue_priorities;
    }

    int index = 0;
    queue_create_info[index++].queueFamilyIndex = device->queue_family_indices.graphic;
    if (device->queue_family_indices.graphic != device->queue_family_indices.present)
        queue_create_info[index++].queueFamilyIndex = device->queue_family_indices.present;
    if (device->queue_family_indices.transfer != device->queue_family_indices.graphic)
        queue_create_info[index++].queueFamilyIndex = device->queue_family_indices.transfer;

    free(queue_family_properties);

    VkPhysicalDeviceFeatures physical_device_features = {0};
    physical_device_features.samplerAnisotropy = VK_TRUE;

    VkPhysicalDeviceSynchronization2FeaturesKHR sync_2_features = {0};
    sync_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    sync_2_features.synchronization2 = VK_TRUE;

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = {0};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;
    dynamic_rendering_features.pNext = &sync_2_features;

    VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timeline_semaphores_features = {0};
    timeline_semaphores_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;
    timeline_semaphores_features.timelineSemaphore = VK_TRUE;
    timeline_semaphores_features.pNext = &dynamic_rendering_features;

    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = queue_count;
    create_info.pQueueCreateInfos = queue_create_info;
    create_info.enabledExtensionCount = ssp_vulkan_device_extensions_name_size;
    create_info.ppEnabledExtensionNames = ssp_vulkan_device_extensions_name;
    create_info.pEnabledFeatures = &physical_device_features;
    create_info.pNext = &timeline_semaphores_features;

    if (ext_func->vkCreateDevice(device->physical_device, &create_info, NULL, &device->logical_device) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_LOGICAL_DEVICE_CREATION;
    if ((error_code = ssp_vulkan_ext_func_device(ext_func, device->logical_device)))
        return error_code;

    free(queue_create_info);
    ext_func->vkGetDeviceQueue(device->logical_device, device->queue_family_indices.graphic, 0, &device->graphic_queue);
    if (device->queue_family_indices.graphic != device->queue_family_indices.present)
        ext_func->vkGetDeviceQueue(device->logical_device, device->queue_family_indices.present, 0, &device->present_queue);
    else
        device->present_queue = device->graphic_queue;

    if (device->queue_family_indices.transfer != device->queue_family_indices.graphic)
        ext_func->vkGetDeviceQueue(device->logical_device, device->queue_family_indices.transfer, 0, &device->transfer_queue);
    else
        device->transfer_queue = device->graphic_queue;

    return SSP_ERROR_CODE_SUCCESS;
}

static int ssp_vulkan_physical_device_score(VkPhysicalDevice physical_device, struct SSPVulkanContextExtFunc *pExt_func)
{
    VkPhysicalDeviceProperties properties;
    pExt_func->vkGetPhysicalDeviceProperties(physical_device, &properties);

    int score = 0;

    if (properties.apiVersion < SSP_VULKAN_API_VERSION)
        return -1;

    int device_type_score;
    switch (properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            device_type_score = 30;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            device_type_score = 20;
            break;
        default:
            device_type_score = 10;
            break;
    }
    score += device_type_score;

    VkPhysicalDeviceFeatures features;
    pExt_func->vkGetPhysicalDeviceFeatures(physical_device, &features);
    if (features.samplerAnisotropy == VK_TRUE)
        score += 1;

    return score;
}

static uint32_t ssp_vulkan_choose_physical_device(VkPhysicalDevice *physical_devices, uint32_t physical_devices_count, struct SSPVulkanContextExtFunc *pExt_func)
{
    uint32_t selected_physical_device_index = 0;
    int selected_physical_device_score = 0;

    int physical_device_score = 0;

    for (uint32_t i = 0; i < physical_devices_count; ++i) {
        physical_device_score = ssp_vulkan_physical_device_score(physical_devices[i], pExt_func);
        if (physical_device_score > selected_physical_device_score && physical_device_score >= 0) {
            selected_physical_device_index = i;
            selected_physical_device_score = physical_device_score;
        }
    }

    return selected_physical_device_index;
}

enum SSP_ERROR_CODE ssp_vulkan_init_physical_device(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, struct SSPVulkanDevice *device)
{
    uint32_t physical_devices_count;
    ext_func->vkEnumeratePhysicalDevices(instance, &physical_devices_count, NULL);
    VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) * physical_devices_count);
    ext_func->vkEnumeratePhysicalDevices(instance, &physical_devices_count, physical_devices);

    device->physical_device = physical_devices[ssp_vulkan_choose_physical_device(physical_devices, physical_devices_count, ext_func)];

    free(physical_devices);
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_get_queue_families_indices(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, VkSurfaceKHR surface, VkQueueFamilyProperties *queue_family_properties, uint32_t queue_family_properties_count)
{
    struct SSPVulkanQueueFamiliesIndices indices = {0};
    bool transfer_queue_found = false;
    bool graphic_queue_found = false;
    bool present_queue_found = false;

    for (uint32_t i = 0; i < queue_family_properties_count; ++i) {
        VkQueueFlags flag = queue_family_properties[i].queueFlags;

        if (!transfer_queue_found && (flag & VK_QUEUE_TRANSFER_BIT) && !(flag & VK_QUEUE_GRAPHICS_BIT) && !(flag & VK_QUEUE_COMPUTE_BIT)) {
            transfer_queue_found = true;
            indices.transfer = i;
        }
        VkBool32 present_mode_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device->physical_device, i, surface, &present_mode_supported);
        if (!graphic_queue_found && flag & VK_QUEUE_GRAPHICS_BIT && !present_queue_found && present_mode_supported) {
            graphic_queue_found = true;
            indices.graphic = i;
            present_queue_found = true;
            indices.present = i;
        }

        if (present_queue_found && graphic_queue_found && transfer_queue_found)
            break;
    }

    if (!graphic_queue_found || !present_queue_found) {
        for (uint32_t i = 0; i < queue_family_properties_count; ++i) {
            VkQueueFlags flag = queue_family_properties[i].queueFlags;
            VkBool32 present_mode_supported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device->physical_device, i, surface, &present_mode_supported);

            if (!graphic_queue_found && flag & VK_QUEUE_GRAPHICS_BIT) {
                graphic_queue_found = true;
                indices.graphic = i;
            }
            if (!present_queue_found && present_mode_supported) {
                present_queue_found = true;
                indices.present = i;
            }
            if (present_queue_found && graphic_queue_found)
                break;
        }
        if(!present_queue_found || !graphic_queue_found)
            return SSP_ERROR_CODE_VULKAN_QUEUE_FAMILIES_NOT_VALID;
    }

    if (!transfer_queue_found)
        indices.transfer = indices.graphic;

    device->queue_family_indices = indices;
    device->queue_family_indices.queue_count = 1
        + (indices.present != indices.graphic)
        + (indices.transfer != indices.graphic && indices.transfer != indices.present);
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE sspv_vulkan_device_destroy()
{

}

#include "vulkan_context/physical_device.h"

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
            device_type_score = 3;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            device_type_score = 2;
            break;
        default:
            device_type_score = 1;
            break;
    }
    score += device_type_score;
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

enum SSP_ERROR_CODE ssp_vulkan_init_physical_device(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, VkPhysicalDevice *physical_device)
{
    uint32_t physical_devices_count;
    ext_func->vkEnumeratePhysicalDevices(instance, &physical_devices_count, NULL);
    VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) * physical_devices_count);
    ext_func->vkEnumeratePhysicalDevices(instance, &physical_devices_count, physical_devices);

    *physical_device = physical_devices[ssp_vulkan_choose_physical_device(physical_devices, physical_devices_count, ext_func)];

    free(physical_devices);
    return SSP_ERROR_CODE_SUCCESS;
}

#include "vulkan_context/queue.h"

enum SSP_ERROR_CODE ssp_vulkan_get_queue_families_indices(struct SSPVulkanContextExtFunc *ext_func,
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    VkQueueFamilyProperties *queue_family_properties,
    uint32_t queue_family_properties_count,
    struct SSPVulkanQueueFamiliesIndices *queue_families_indices)
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
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_mode_supported);
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
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_mode_supported);

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

    *queue_families_indices = indices;
    queue_families_indices->queue_count = 1
        + (indices.present != indices.graphic)
        + (indices.transfer != indices.graphic && indices.transfer != indices.present);
    return SSP_ERROR_CODE_SUCCESS;
}

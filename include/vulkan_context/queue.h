#ifndef _SISYPHUS_VULKAN_CONTEXT_QUEUE_H
#define _SISYPHUS_VULKAN_CONTEXT_QUEUE_H

#include <vulkan/vulkan.h>
#include <stdbool.h>

#include "errors.h"
#include "ext_func.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPVulkanQueueFamiliesIndices {
    uint32_t transfer;
    uint32_t graphic;
    uint32_t present;

    uint32_t queue_count;
};

/**
 * @brief gather families indices needed from the one availabled.
 * 1 dedicated transfer queue if it can, if it can't fall back to the graphic queue.
 * 1 queue for both graphic operations and present if it can, otherwise take any present queue and any graphic queue.
 * 
 * @return enum SSP_ERROR_CODE 
 */
enum SSP_ERROR_CODE ssp_vulkan_get_queue_families_indices(
    struct SSPVulkanContextExtFunc *ext_func,
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    VkQueueFamilyProperties *queue_family_properties,
    uint32_t queue_family_properties_count,
    struct SSPVulkanQueueFamiliesIndices *queue_families_indices);

#ifdef __cplusplus
}
#endif

#endif

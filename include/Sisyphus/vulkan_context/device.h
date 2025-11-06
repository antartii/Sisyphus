#pragma once

#include <vulkan/vulkan.h>
#include <stdbool.h>

#include <Sisyphus/errors.h>
#include <Sisyphus/window.h>
#include "ext_func.h"
#include "instance.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPVulkanQueueFamiliesIndices {
    uint32_t transfer;
    uint32_t graphic;
    uint32_t present;

    uint32_t queue_count;
};

struct SSPVulkanDevice {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;

    struct SSPVulkanQueueFamiliesIndices queue_family_indices;
    VkQueue transfer_queue;
    VkQueue present_queue;
    VkQueue graphic_queue;
};

enum SSP_ERROR_CODE ssp_vulkan_init_physical_device(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_create_logical_device(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, struct SSPVulkanDevice *device, VkSurfaceKHR surface);

/**
 * @brief gather families indices needed from the one availabled.
 * 1 dedicated transfer queue if it can, if it can't fall back to the graphic queue.
 * 1 queue for both graphic operations and present if it can, otherwise take any present queue and any graphic queue.
 * 
 * @return enum SSP_ERROR_CODE 
 */
enum SSP_ERROR_CODE ssp_vulkan_get_queue_families_indices(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, VkSurfaceKHR surface, VkQueueFamilyProperties *queue_family_properties, uint32_t queue_family_properties_count);

#ifdef __cplusplus
}
#endif

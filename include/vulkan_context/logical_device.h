#ifndef _SISYPHUS_VULKAN_CONTEXT_LOGICAL_DEVICE_H
#define _SISYPHUS_VULKAN_CONTEXT_LOGICAL_DEVICE_H

#include <vulkan/vulkan.h>
#include "errors.h"
#include "window.h"
#include "ext_func.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_ERROR_CODE ssp_vulkan_create_logical_device(
    struct SSPVulkanContextExtFunc *ext_func,
    struct SSPWindow *window,
    VkPhysicalDevice physical_device,
    VkDevice *logical_device,
    VkSurfaceKHR surface,
    struct SSPVulkanQueueFamiliesIndices *queue_family_indices,
    VkQueue *transfer_queue,
    VkQueue *present_queue,
    VkQueue *graphic_queue); // maybe do a struct to stores the vkqueues

#ifdef __cplusplus
}
#endif

#endif

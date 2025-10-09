#ifndef _SISYPHUS_VULKAN_CONTEXT_UTILS_H
#define _SISYPHUS_VULKAN_CONTEXT_UTILS_H

#include <vulkan/vulkan.h>
#include "ext_func.h"

#ifdef __cplusplus
extern "C" {
#endif

int ssp_vulkan_find_memory_type(struct SSPVulkanContextExtFunc *ext_func, VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties);

#ifdef __cplusplus
    }
#endif

#endif

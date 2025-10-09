#ifndef _SISYPHUS_VULKAN_CONTEXT_PHYSICAL_DEVICE_H
#define _SISYPHUS_VULKAN_CONTEXT_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include "errors.h"
#include "instance.h"
#include "ext_func.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_ERROR_CODE ssp_vulkan_init_physical_device(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, VkPhysicalDevice *physical_device);

#ifdef __cplusplus
    }
#endif

#endif

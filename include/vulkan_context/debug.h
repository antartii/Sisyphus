#ifndef _SISYPHUS_VULKAN_CONTEXT_DEBUG_H
#define _SISYPHUS_VULKAN_CONTEXT_DEBUG_H

#include "errors.h"
#include <vulkan/vulkan.h>
#include "ext_func.h"
#include <inttypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_ERROR_CODE ssp_vulkan_setup_debug_messenger(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, VkDebugUtilsMessengerEXT *debug_messenger);

#ifdef __cplusplus
    }
#endif

#endif

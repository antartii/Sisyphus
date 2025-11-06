#pragma once

#include <Sisyphus/errors.h>
#include <vulkan/vulkan.h>
#include "ext_func.h"
#include <inttypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
enum SSP_ERROR_CODE ssp_vulkan_setup_debug_messenger(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, VkDebugUtilsMessengerEXT *debug_messenger);
#endif

#ifdef __cplusplus
    }
#endif

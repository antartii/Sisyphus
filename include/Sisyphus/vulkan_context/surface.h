#pragma once

#include <vulkan/vulkan.h>

#include <Sisyphus/errors.h>
#include "ext_func.h"
#include <Sisyphus/window.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_ERROR_CODE ssp_vulkan_create_surface(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance, VkSurfaceKHR *surface, struct SSPWindow *window);

#ifdef __cplusplus
}
#endif

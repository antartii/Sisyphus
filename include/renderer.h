#ifndef _SISYPHUS_RENDERER_H
#define _SISYPHUS_RENDERER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "sisyphus_config.h"
#include "utils.h"
#include "errors.h"
#include "string.h"

#define SSP_VULKAN_API_VERSION VK_API_VERSION_1_0

struct SSPVulkanContext {
    VkInstance instance;
};

struct SSPRenderer {
    struct SSPVulkanContext vulkan_context;
};

enum SSP_ERROR_CODE ssp_renderer_create(struct SSPRenderer *pRenderer, const char *app_name, const struct Version app_version);
void ssp_renderer_destroy(struct SSPRenderer *pRenderer);

#endif

#ifndef _SISYPHUS_VULKAN_CONTEXT_INSTANCE_H
#define _SISYPHUS_VULKAN_CONTEXT_INSTANCE_H

#include "errors.h"
#include <vulkan/vulkan.h>
#include "../utils.h"
#include "ext_func.h"
#include "sisyphus_config.h"

#define SSP_VULKAN_API_VERSION VK_API_VERSION_1_1
#define SSP_VULKAN_APP_INFO_NAME_DEFAULT "Sisyphus application"

enum SSP_ERROR_CODE ssp_vulkan_create_instance(struct SSPVulkanContextExtFunc *ext_func, VkInstance *instance, const char *app_name, const struct SSPVersion app_version);

#endif

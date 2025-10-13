#ifndef _SISYPHUS_VULKAN_CONTEXT_SYNC_OBJECT_H
#define _SISYPHUS_VULKAN_CONTEXT_SYNC_OBJECT_H

#include <vulkan/vulkan.h>

#include "errors.h"
#include "graphic.h"
#include "ext_func.h"
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_ERROR_CODE ssp_vulkan_create_sync_objects(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_sync_objects_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);

#ifdef __cplusplus
}
#endif

#endif

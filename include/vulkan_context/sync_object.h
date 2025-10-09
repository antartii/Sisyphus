#ifndef _SISYPHUS_VULKAN_CONTEXT_SYNC_OBJECT_H
#define _SISYPHUS_VULKAN_CONTEXT_SYNC_OBJECT_H

#include <vulkan/vulkan.h>

#include "errors.h"
#include "graphic.h"
#include "ext_func.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_ERROR_CODE ssp_vulkan_create_sync_objects(struct SSPVulkanContextExtFunc *ext_func,
    VkSemaphore **present_complete_semaphores,
    VkSemaphore **render_finished_semaphores,
    VkFence **frames_in_flight_fences,
    VkDevice logical_device,
    VkFence *transfer_copy_buffer_fence,
    VkFence *transfer_copy_buffer_to_image_fence);

#ifdef __cplusplus
}
#endif

#endif

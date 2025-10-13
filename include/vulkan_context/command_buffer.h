#ifndef _SISYPHUS_VULKAN_CONTEXT_COMMAND_BUFFER
#define _SISYPHUS_VULKAN_CONTEXT_COMMAND_BUFFER

#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdio.h>

#include "errors.h"
#include "ext_func.h"
#include "device.h"

#include "../utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSP_MAX_FRAMES_IN_FLIGHT 2 // todo : replace by known available max frames in flight by physical device capabilities

#define SSP_TRANSFER_COMMAND_BUFFERS_COUNT 2
#define SSP_DEFAULT_COPY_BUFFER_QUEUE_POOL_SIZE 5
#define SSP_DEFAULT_COPY_BUFFER_TO_IMAGE_QUEUE_POOL_SIZE 5

struct SSPVulkanCopyBufferData {
    VkBuffer *dst_buffer;
    VkDeviceSize size;
    VkBuffer src_buffer;
    VkDeviceMemory src_memory;

    int post_process_bitmask;
};

struct SSPVulkanCopyBufferToImageData {

};

struct SSPVulkanCommandContext {
    VkCommandPool *command_pools;
    int command_pools_size;

    int graphic_command_pool_index;
    int transfer_command_pool_index;

    VkCommandBuffer *graphic_command_buffers;
    VkCommandBuffer *transfer_command_buffers;
    int transfer_command_buffer_copy_index;
    int transfer_command_buffer_image_copy_index;

    struct SSPDynamicArray *transfer_copy_buffer_queue;
    struct SSPDynamicArray *transfer_copy_buffer_to_image_queue;

    VkFence transfer_copy_buffer_fence;
    VkFence transfer_copy_buffer_to_image_fence;
    VkSemaphore *present_complete_semaphores;
    VkSemaphore *render_finished_semaphores;
    VkFence *frames_in_flight_fences;
};

enum SSP_ERROR_CODE ssp_vulkan_create_command_pools(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_allocate_command_buffers(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_command_buffer_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);

#ifdef __cplusplus
}
#endif

#endif

#pragma once

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
#define SSP_DEFAULT_TRANSITION_IMAGE_LAYOUT_QUEUE_POOL_SIZE 5
#define SSP_DEFAULT_TRANSFER_OTHER_QUEUE_POOL_SIZE 5

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
    struct SSPDynamicArray *transfer_transition_image_layout_queue;

    VkSemaphore *present_complete_semaphores;
    VkSemaphore *render_finished_semaphores;

    VkSemaphore transfer_semaphore_timeline;
    uint64_t transfer_semaphore_timeline_value;
    uint64_t curr_transfer_semaphore_value;

    VkSemaphore transfer_image_semaphore_timeline;
    uint64_t transfer_image_semaphore_value;
    uint64_t curr_transfer_image_semaphore_value;

    VkFence *frames_in_flight_fences;
};

enum SSP_ERROR_CODE ssp_vulkan_create_command_pools(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_allocate_command_buffers(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_command_buffer_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);

#ifdef __cplusplus
}
#endif

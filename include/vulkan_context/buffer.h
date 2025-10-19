#ifndef _SISYPHUS_VULKAN_CONTEXT_BUFFER_H
#define _SISYPHUS_VULKAN_CONTEXT_BUFFER_H

#include <vulkan/vulkan.h>
#include "ext_func.h"
#include "utils.h"
#include "shaders.h"
#include "../utils.h"
#include <unistd.h>

#include "command_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_VULKAN_COPY_BUFFER_POST_PROCESS
{
    SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER = 1,
    SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY = 1 << 1
};

enum SSP_VULKAN_BUFFER_STATE {
    SSP_VULKAN_BUFFER_STATE_UNDEFINED = -1,
    SSP_VULKAN_BUFFER_STATE_NOT_INITIALIZED,
    SSP_VULKAN_BUFFER_STATE_LOADING,
    SSP_VULKAN_BUFFER_STATE_READY,

    SSP_VULKAN_BUFFER_STATE_COUNT
};

struct SSPVulkanBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;

    enum SSP_VULKAN_BUFFER_STATE state;
    uint64_t queue_batch;
};

struct SSPVulkanCopyBufferData {
    struct SSPVulkanBuffer *dst_buffer; // do somthing for errors in case the pointer is bad / changed before usage
    VkDeviceSize size;
    VkBuffer src_buffer;
    VkDeviceMemory src_memory;

    int post_process_bitmask;
};

struct SSPVulkanImage {
    VkImage image;

    enum SSP_VULKAN_BUFFER_STATE state;
    uint64_t queue_batch;
};

struct SSPVulkanCopyBufferToImageData {
    int offsetX;
    int offsetY;
    uint32_t height;
    uint32_t width;
    VkBuffer src_buffer;
    VkDeviceMemory src_memory;

    struct SSPVulkanImage image;
    VkImageLayout image_layout;

    int post_process_bitmask;
};

enum SSP_ERROR_CODE ssp_vulkan_create_buffer(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, VkBuffer *buffer, VkDeviceMemory *memory);
void ssp_vulkan_stage_buffer(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, VkDeviceSize size, void *data, VkBuffer *staging_buffer, VkDeviceMemory *staging_buffer_memory);
enum SSP_ERROR_CODE ssp_vulkan_create_vertex_buffer(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, struct SSPVulkanCommandContext *command_context, struct SSPVulkanBuffer *buffer, struct SSPShaderVertex *vertices, uint32_t vertices_count);
enum SSP_ERROR_CODE ssp_vulkan_create_index_buffer(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, struct SSPVulkanCommandContext *command_context, struct SSPVulkanBuffer *buffer, uint16_t *indices, uint32_t indices_count);
enum SSP_ERROR_CODE ssp_vulkan_copy_buffer_queue_push(struct SSPDynamicArray *transfer_copy_buffer_queue, struct SSPVulkanBuffer *dst_buffer, VkBuffer src_buffer, VkDeviceMemory src_memory, VkDeviceSize size, enum SSP_VULKAN_COPY_BUFFER_POST_PROCESS prost_process_flags);
enum SSP_ERROR_CODE ssp_vulkan_copy_buffer_round(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_copy_image_buffer_queue_round(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device);

enum SSP_ERROR_CODE ssp_vulkan_transition_image_layout(
    struct SSPVulkanContextExtFunc *ext_func,
    VkImage image,
    VkImageLayout old_layout,
    VkImageLayout new_layout,
    VkAccessFlags2 src_access_mask,
    VkAccessFlags2 dst_access_mask,
    VkPipelineStageFlags2 src_stage_mask,
    VkPipelineStageFlags2 dst_stage_mask,
    VkCommandBuffer command_buffer,
    uint32_t src_queue_family_index,
    uint32_t dst_queue_family_index);

enum SSP_ERROR_CODE ssp_vulkan_copy_image_buffer_queue_push(struct SSPDynamicArray *transfer_transition_queue,
    struct SSPVulkanCommandContext *command_context,
    VkImage image,
    uint32_t height,
    uint32_t width,
    int offsetX,
    int offsetY,
    VkBuffer src_buffer,
    VkDeviceMemory src_memory,
    VkImageLayout image_layout,
    int post_process_bitmask);

#ifdef __cplusplus
    }
#endif

#endif

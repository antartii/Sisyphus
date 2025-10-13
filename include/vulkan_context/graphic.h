#ifndef _SISYPHUS_VULKAN_CONTEXT_GRAPHIC_H
#define _SISYPHUS_VULKAN_CONTEXT_GRAPHIC_H

#include <vulkan/vulkan.h>

#include "errors.h"
#include "ext_func.h"
#include "sisyphus_config.h"
#include "shaders.h"
#include "../utils.h"
#include "buffer.h"
#include "../camera.h"
#include "swapchain.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPVulkanPipelineContext {
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet *descriptor_sets;

    VkViewport viewport;

    VkPipelineLayout pipeline_layout;
    VkPipeline graphic_pipeline;

    VkBuffer *uniform_buffers;
    VkDeviceMemory *uniform_buffers_memory;
    void **uniform_buffers_mapped;

    int image_index;
};

enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_set_layout(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, struct SSPVulkanPipelineContext *pipeline_context);
enum SSP_ERROR_CODE ssp_vulkan_create_graphic_pipeline(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, struct SSPVulkanDevice *device, struct SSPVulkanSwapchain *swapchain);
enum SSP_ERROR_CODE ssp_vulkan_create_uniform_buffers(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_create_image_view(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanSwapchain *swapchain, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_pool(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, struct SSPVulkanPipelineContext *pipeline_context);
enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_sets(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_update_descriptor_sets(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, struct SSPVulkanDevice *device);
enum SSP_ERROR_CODE ssp_vulkan_create_image(
    struct SSPVulkanContextExtFunc *ext_func,
    VkDevice logical_device,
    VkPhysicalDevice physical_device,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage *image,
    VkDeviceMemory *image_memory);

void ssp_vulkan_update_proj(struct SSPCamera *camera, struct SSPVulkanPipelineContext *pipeline_context, VkExtent2D swapchain_extent);
void ssp_vulkan_update_view(struct SSPCamera *camera, struct SSPVulkanPipelineContext *pipeline_context);

enum SSP_ERROR_CODE ssp_vulkan_record_graphic_command_buffer(struct SSPVulkanContextExtFunc *ext_func,
    struct SSPVulkanSwapchain *swapchain,
    struct SSPVulkanCommandContext *command_context,
    struct SSPVulkanPipelineContext *pipeline_context,
    int current_frame,
    struct SSPDynamicArray *objects_to_draw);
enum SSP_ERROR_CODE ssp_vulkan_transition_image_layout(
    struct SSPVulkanContextExtFunc *ext_func,
    VkImage *image,
    VkImageLayout old_layout,
    VkImageLayout new_layout,
    VkAccessFlags2 src_access_mask,
    VkAccessFlags2 dst_access_mask,
    VkPipelineStageFlags2 src_stage_mask,
    VkPipelineStageFlags2 dst_stage_mask,
    VkCommandBuffer command_buffer,
    uint32_t src_queue_family_index,
    uint32_t dst_queue_family_index);

enum SSP_ERROR_CODE ssp_vulkan_pipeline_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, VkDevice logical_device);

#ifdef __cplusplus
}
#endif

#endif
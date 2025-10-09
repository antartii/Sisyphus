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

#ifdef __cplusplus
extern "C" {
#endif

#define SSP_MAX_FRAMES_IN_FLIGHT 2 // todo : replace by known available max frames in flight by physical device capabilities

enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_set_layout(struct SSPVulkanContextExtFunc *ext_func, VkDevice logical_device, VkDescriptorSetLayout *descriptor_set_layout);
enum SSP_ERROR_CODE ssp_vulkan_create_graphic_pipeline(
    struct SSPVulkanContextExtFunc *ext_func,
    VkDevice logical_device,
    VkViewport *viewport,
    VkExtent2D swapchain_extent,
    VkDescriptorSetLayout descriptor_set_layout,
    VkPipelineLayout *pipeline_layout,
    VkFormat swapchain_image_format,
    VkPipeline *graphic_pipeline);
enum SSP_ERROR_CODE ssp_vulkan_create_uniform_buffers(struct SSPVulkanContextExtFunc *ext_func,
    VkBuffer **uniform_buffers,
    VkDeviceMemory **uniform_buffers_memory,
    void *** uniform_buffers_mapped,
    VkDevice logical_device,
    VkPhysicalDevice physical_device);
enum SSP_ERROR_CODE ssp_vulkan_create_image_view(struct SSPVulkanContextExtFunc *ext_func,
    VkFormat swapchain_image_format,
    VkImageView **swapchain_image_views,
    int swapchain_images_count,
    VkImage *swapchain_images,
    VkDevice logical_device);
enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_pool(struct SSPVulkanContextExtFunc *ext_func,
    VkDevice logical_device,
    VkDescriptorPool *descriptor_pool);
enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_sets(struct SSPVulkanContextExtFunc *ext_func,
    VkDescriptorSet **descriptor_sets,
    VkDescriptorSetLayout descriptor_set_layout,
    VkDescriptorPool descriptor_pool,
    VkDevice logical_device,
    VkBuffer *uniform_buffers);
enum SSP_ERROR_CODE ssp_vulkan_update_descriptor_sets(struct SSPVulkanContextExtFunc *ext_func,
    VkBuffer *uniform_buffers,
    VkDescriptorSet *descriptor_sets,
    VkDevice logical_device);
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

void ssp_vulkan_update_proj(struct SSPCamera *camera, void **uniform_buffers_mapped, VkExtent2D swapchain_extent);
void ssp_vulkan_update_view(struct SSPCamera *camera, void **uniform_buffers_mapped);

enum SSP_ERROR_CODE ssp_vulkan_record_graphic_command_buffer(struct SSPVulkanContextExtFunc *ext_func,
    VkCommandBuffer *graphic_command_buffers,
    int current_frame,
    VkImage **swapchain_images,
    int image_index,
    VkImageView *swapchain_image_views,
    VkExtent2D swapchain_extent,
    VkPipeline graphic_pipeline,
    VkPipelineLayout pipeline_layout,
    VkViewport viewport,
    VkDescriptorSet **descriptor_sets,
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

#ifdef __cplusplus
}
#endif

#endif
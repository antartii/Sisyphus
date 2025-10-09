#ifndef _SISYPHUS_VULKAN_CONTEXT_H
#define _SISYPHUS_VULKAN_CONTEXT_H

#include <vulkan/vulkan.h>
#include <cglm/cglm.h>
#include <inttypes.h>

#include "utils.h"
#include "errors.h"
#include "shaders.h"
#include "sisyphus_config.h"
#include "window.h"
#include "ext_func.h"
#include "buffer.h"
#include "instance.h"
#include "debug.h"
#include "surface.h"
#include "physical_device.h"
#include "logical_device.h"
#include "queue.h"
#include "swapchain.h"
#include "graphic.h"
#include "command_buffer.h"
#include "sync_object.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPVulkanContext {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    VkSurfaceKHR surface;
    VkCommandPool *command_pools;
    int command_pools_size;
    int transfer_command_pool_index;
    int graphic_command_pool_index;
    VkQueue graphic_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkSwapchainKHR swapchain;

    struct SSPVulkanQueueFamiliesIndices queue_family_indices;

    int swapchain_images_count;
    VkImage *swapchain_images;    
    VkImageView *swapchain_image_views;
    VkFormat swapchain_image_format;
    VkExtent2D swapchain_extent;

    VkCommandBuffer *transfer_command_buffers;
    VkCommandBuffer *graphic_command_buffers;
    int command_buffers_size;
    int transfer_command_buffer_copy_index;
    int transfer_command_buffer_image_copy_index;

    VkFence transfer_copy_buffer_fence;
    struct SSPDynamicArray *transfer_copy_buffer_queue;
    VkFence transfer_copy_buffer_to_image_fence;
    struct SSPDynamicArray *transfer_copy_buffer_to_image_queue;

    VkSemaphore *present_complete_semaphores;
    VkSemaphore *render_finished_semaphores;
    VkFence *frames_in_flight_fences;

    VkDescriptorSetLayout descriptor_set_layout;
    VkViewport viewport;

    VkPipeline graphic_pipeline;
    VkPipelineLayout pipeline_layout;

    VkBuffer *uniform_buffers;
    VkDeviceMemory *uniform_buffers_memory;
    void **uniform_buffers_mapped;

    VkDescriptorPool descriptor_pool;
    VkDescriptorSet *descriptor_sets;

    struct SSPCamera *cameraData; // need to see for removing

    #ifdef DEBUG
    VkDebugUtilsMessengerEXT debug_messenger;
    #endif

    int image_index;
    int current_frame;

    struct SSPVulkanContextExtFunc ext_func;
};

struct SSPCamera;

enum SSP_ERROR_CODE ssp_vulkan_context_destroy(struct SSPVulkanContext *pContext);
enum SSP_ERROR_CODE ssp_vulkan_draw_frame(struct SSPVulkanContext *pContext, struct SSPWindow *window, struct SSPDynamicArray *objects_to_draw);
enum SSP_ERROR_CODE ssp_vulkan_context_init(struct SSPVulkanContext *pContext, struct SSPWindow *window, const char *app_name, struct SSPVersion app_version);

#ifdef __cplusplus
    }
#endif

#endif

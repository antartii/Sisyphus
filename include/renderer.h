#ifndef _SISYPHUS_RENDERER_H
#define _SISYPHUS_RENDERER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <vulkan/vulkan.h>
#include <unistd.h>

#include "shaders.h"
#include "sisyphus_config.h"
#include "utils.h"
#include "errors.h"
#include <string.h>
#include "config.h"
#include "window.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSP_VULKAN_API_VERSION VK_API_VERSION_1_1
#define SSP_RENDERER_APP_INFO_NAME_DEFAULT "Sisyphus application"
#define SSP_MAX_FRAMES_IN_FLIGHT 2 // todo : replace by known available max frames in flight by physical device capabilities
#define SSP_TRANSFER_COMMAND_BUFFERS_COUNT 1
#define SSP_DEFAULT_OBJECTS_POOLS_SIZE 50

struct SSPVulkanContextExtFunc {
    PFN_vkCreateInstance vkCreateInstance;
    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties; // deprecated: Vulkan 1.1
    PFN_vkCreateDevice vkCreateDevice;
    PFN_vkDestroyDevice vkDestroyDevice;
    PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkCreateCommandPool vkCreateCommandPool;
    PFN_vkDestroyCommandPool vkDestroyCommandPool;
    PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
    PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
    PFN_vkResetCommandBuffer vkResetCommandBuffer;
    PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
    PFN_vkEndCommandBuffer vkEndCommandBuffer;
    PFN_vkQueueSubmit vkQueueSubmit;
    PFN_vkGetDeviceQueue vkGetDeviceQueue;

    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;

    PFN_vkCreateSemaphore vkCreateSemaphore;
    PFN_vkDestroySemaphore vkDestroySemaphore;
    PFN_vkCreateFence vkCreateFence;
    PFN_vkDestroyFence vkDestroyFence;
    PFN_vkWaitForFences vkWaitForFences;
    PFN_vkResetFences vkResetFences;
    PFN_vkQueuePresentKHR vkQueuePresentKHR;
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
    PFN_vkCreateShaderModule vkCreateShaderModule;    

    PFN_vkCreateImageView vkCreateImageView;
    PFN_vkDestroyImageView vkDestroyImageView;

    PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;
    PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
    PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;

    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
    PFN_vkDestroyImage vkDestroyImage;

    PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
    PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;

    PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
    PFN_vkDestroyShaderModule vkDestroyShaderModule;
    PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
    PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
    PFN_vkDestroyPipeline vkDestroyPipeline;

    PFN_vkMapMemory vkMapMemory;
    PFN_vkUnmapMemory vkUnmapMemory;
    PFN_vkCreateBuffer vkCreateBuffer;
    PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
    PFN_vkAllocateMemory vkAllocateMemory;
    PFN_vkBindBufferMemory vkBindBufferMemory;
    PFN_vkFreeMemory vkFreeMemory;
    PFN_vkDestroyBuffer vkDestroyBuffer;
    PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;

    PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
    PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;

    PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
    PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
    PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;

    PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
    PFN_vkQueueWaitIdle vkQueueWaitIdle;

    PFN_vkCmdBindPipeline vkCmdBindPipeline;
    PFN_vkCmdSetViewport vkCmdSetViewport;
    PFN_vkCmdSetScissor vkCmdSetScissor;
    PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;

    PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
    PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
    PFN_vkCmdPushConstants vkCmdPushConstants;
    PFN_vkCmdDrawIndexed vkCmdDrawIndexed;

    #ifdef DEBUG
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    #endif
    PFN_vkDestroyInstance vkDestroyInstance;

    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
    #endif
    PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
};

struct SSPVulkanQueueFamiliesIndices {
    uint32_t transfer;
    uint32_t graphic;
    uint32_t present;

    uint32_t queue_count;
};

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

    struct SSPCamera *cameraData;

    #ifdef DEBUG
    VkDebugUtilsMessengerEXT debug_messenger;
    #endif

    int image_index;
    int current_frame;

    struct SSPVulkanContextExtFunc ext_func;
};

/*enum SSP_RENDERER_INIT_STEPS {
    SSP_RENDERER_INIT_STEPS_NOT_INITIALISED = 0,
    SSP_RENDERER_INIT_STEPS_EXT_FUNC_INSTANCE_GLOBAL,
    SSP_RENDERER_INIT_STEPS_CREATE_INSTANCE,
    SSP_RENDERER_INIT_STEPS_EXT_FUNC_INSTANCE_NGLOBAL,
    #ifdef DEBUG
    SSP_RENDERER_INIT_STEPS_SETUP_DEBUG_MESSENGER,
    #endif
    SSP_RENDERER_INIT_STEPS_INIT_PHYSICAL_DEVICE,
    SSP_RENDERER_INIT_STEPS_CREATE_LOGICAL_DEVICE
};*/

struct SSPRenderer {
    struct SSPDynamicArray *objects_to_draw;

    struct SSPVulkanContext vulkan_context;
};

enum SSP_ERROR_CODE ssp_renderer_create(struct SSPRenderer *pRenderer, struct SSPWindow *window, struct SSPConfig *config);
void ssp_renderer_destroy(struct SSPRenderer *pRenderer);

enum SSP_ERROR_CODE ssp_renderer_draw_frame(struct SSPRenderer *pRenderer, struct SSPWindow *window);
enum SSP_ERROR_CODE ssp_renderer_stop(struct SSPRenderer *pRenderer);
enum SSP_ERROR_CODE ssp_vulkan_create_buffer(struct SSPVulkanContext *pContext, VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, VkBuffer *buffer, VkDeviceMemory *memory);
enum SSP_ERROR_CODE ssp_vulkan_copy_buffer(struct SSPVulkanContext *pContext, VkBuffer *src_buffer, VkBuffer *dst_buffer, VkDeviceSize size);

enum SSP_ERROR_CODE ssp_vulkan_create_vertex_buffer(struct SSPVulkanContext *pContext, VkBuffer *buffer, VkDeviceMemory *memory, struct SSPShaderVertex *vertices, uint32_t vertices_count);
enum SSP_ERROR_CODE ssp_vulkan_create_index_buffer(struct SSPVulkanContext *pContext, VkBuffer *buffer, VkDeviceMemory *memory, uint16_t *indices, uint32_t indices_count);

struct SSPCamera;
void ssp_vulkan_update_proj(struct SSPVulkanContext *context);
void ssp_vulkan_update_view(struct SSPVulkanContext *context);

#ifdef __cplusplus
    }
#endif

#endif

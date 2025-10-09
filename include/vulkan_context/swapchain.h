#ifndef _SISYPHUS_VULKAN_CONTEXT_SWAPCHAIN_H
#define _SISYPHUS_VULKAN_CONTEXT_SWAPCHAIN_H

#include <vulkan/vulkan.h>

#include "errors.h"
#include "ext_func.h"
#include "window.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_ERROR_CODE ssp_vulkan_create_swapchain(
    struct SSPVulkanContextExtFunc *ext_func,
    VkPhysicalDevice physical_device,
    VkDevice logical_device,
    VkSurfaceKHR surface,
    struct SSPWindow *window,
    VkFormat *swapchain_image_format, 
    VkExtent2D *swapchain_extent,
    struct SSPVulkanQueueFamiliesIndices queue_family_indices,
    VkSwapchainKHR *swapchain,
    int *swapchain_images_count,
    VkImage **swapchain_images);

enum SSP_ERROR_CODE ssp_vulkan_swapchain_destroy(struct SSPVulkanContextExtFunc *ext_func,
    VkImageView *swapchain_image_views,
    int swapchain_images_count,
    VkImage *swapchain_images,
    VkDevice logical_device,
    VkSwapchainKHR swapchain);

#ifdef __cplusplus
    }
#endif

#endif

#pragma once

#include <vulkan/vulkan.h>

#include <Sisyphus/errors.h>
#include "ext_func.h"
#include <Sisyphus/window.h>
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPVulkanSwapchain {
    VkFormat format;
    VkExtent2D extent;
    VkSwapchainKHR vk_swapchain;
    int images_count;
    VkImage *images;
    VkImageView *image_views;
};

enum SSP_ERROR_CODE ssp_vulkan_create_swapchain(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanSwapchain *swapchain, struct SSPVulkanDevice *device, VkSurfaceKHR surface, struct SSPWindow *window);
enum SSP_ERROR_CODE ssp_vulkan_swapchain_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanSwapchain *swapchain, struct SSPVulkanDevice *device);

#ifdef __cplusplus
    }
#endif

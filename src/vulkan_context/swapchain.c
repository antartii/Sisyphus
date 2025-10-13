#include "vulkan_context/swapchain.h"

static VkSurfaceFormatKHR ssp_vulkan_swapchain_choose_format(struct SSPVulkanContextExtFunc *ext_func, VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    uint32_t surface_formats_count;
    ext_func->vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_formats_count, NULL);
    VkSurfaceFormatKHR *surface_formats = malloc(sizeof(VkSurfaceFormatKHR) * surface_formats_count);
    ext_func->vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_formats_count, surface_formats);
    VkSurfaceFormatKHR result = surface_formats[0];

    for (uint32_t i = 0; i < surface_formats_count; ++i) {
        if (surface_formats[i].format == VK_FORMAT_R8G8B8A8_SRGB && surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            result = surface_formats[i];
            break;
        }
    }

    free(surface_formats);
    return result;
}

static VkPresentModeKHR ssp_vulkan_swapchain_choose_present_mode(struct SSPVulkanContextExtFunc *ext_func, VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    uint32_t present_modes_count;
    ext_func->vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_modes_count, NULL);
    VkPresentModeKHR *present_modes = malloc(sizeof(VkPresentModeKHR) * present_modes_count);
    ext_func->vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_modes_count, present_modes);
    VkPresentModeKHR result = VK_PRESENT_MODE_FIFO_KHR;

    for (uint32_t i = 0; i < present_modes_count; ++i) {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            result = present_modes[i];
            break;
        }
    }

    free(present_modes);
    return result;
}

static VkExtent2D ssp_vulkan_swapchain_choose_extent(VkExtent2D current_extent, VkExtent2D min_extent, VkExtent2D max_extent, struct SSPWindow *window)
{
    if (current_extent.width == UINT32_MAX) {
        current_extent.width = clamp_int(window->width, min_extent.width, max_extent.width);
        current_extent.height = clamp_int(window->height, min_extent.height, max_extent.height);
    }

    return current_extent;
}

enum SSP_ERROR_CODE ssp_vulkan_create_swapchain(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanSwapchain *swapchain, struct SSPVulkanDevice *device, VkSurfaceKHR surface, struct SSPWindow *window)
{
    VkSurfaceCapabilitiesKHR surface_capabilities;
    ext_func->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physical_device, surface, &surface_capabilities);

    VkSurfaceFormatKHR surface_format = ssp_vulkan_swapchain_choose_format(ext_func, device->physical_device, surface);
    swapchain->format = surface_format.format;
    VkPresentModeKHR present_mode = ssp_vulkan_swapchain_choose_present_mode(ext_func, device->physical_device, surface);
    swapchain->extent = ssp_vulkan_swapchain_choose_extent(surface_capabilities.currentExtent, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent, window);
    if (swapchain->extent.width == 0 || swapchain->extent.height == 0)
        return SSP_ERROR_CODE_VULKAN_SWAPCHAIN_EXTENT;
    
    VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    
    if (!(surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR))
        composite_alpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

    VkSwapchainCreateInfoKHR create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.clipped = VK_TRUE;
    create_info.compositeAlpha = composite_alpha;
    create_info.imageArrayLayers = 1;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = swapchain->extent;
    create_info.imageFormat = swapchain->format;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.minImageCount = surface_capabilities.minImageCount;
    create_info.presentMode = present_mode;
    create_info.preTransform = surface_capabilities.currentTransform;

    if (device->queue_family_indices.graphic == device->queue_family_indices.present)
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    else {
        uint32_t queue_family_indices_arr[] = {device->queue_family_indices.graphic, device->queue_family_indices.present};
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.pQueueFamilyIndices = queue_family_indices_arr;
        create_info.queueFamilyIndexCount = 2;
    }

    if (ext_func->vkCreateSwapchainKHR(device->logical_device, &create_info, NULL, &swapchain->vk_swapchain) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_SWAPCHAIN_CREATION;

    swapchain->images_count = surface_capabilities.minImageCount;
    swapchain->images = calloc(swapchain->images_count, sizeof(VkImage));
    ext_func->vkGetSwapchainImagesKHR(device->logical_device, swapchain->vk_swapchain, &swapchain->images_count, swapchain->images);

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_swapchain_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanSwapchain *swapchain, struct SSPVulkanDevice *device)
{
    if (swapchain->image_views) {
        for (int i = 0; i < swapchain->images_count; ++i)
            ext_func->vkDestroyImageView(device->logical_device, swapchain->image_views[i], NULL);
        free(swapchain->image_views);
    }

    free(swapchain->images);

    ext_func->vkDestroySwapchainKHR(device->logical_device, swapchain->vk_swapchain, NULL);
}

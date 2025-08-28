#ifndef _SISYPHUS_RENDERER_H
#define _SISYPHUS_RENDERER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <vulkan/vulkan.h>
#include <unistd.h>

#include "sisyphus_config.h"
#include "utils.h"
#include "errors.h"
#include "string.h"

#define SSP_VULKAN_API_VERSION VK_API_VERSION_1_0

struct SSPVulkanContextExtFunc {
    PFN_vkCreateInstance vkCreateInstance;
    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties; // deprecated: Vulkan 1.1

    #ifdef DEBUG
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    #endif
    PFN_vkDestroyInstance vkDestroyInstance;
};

struct SSPVulkanContext {
    VkInstance instance;
    VkPhysicalDevice physical_device;

    #ifdef DEBUG
    VkDebugUtilsMessengerEXT debug_messenger;
    #endif

    struct SSPVulkanContextExtFunc ext_func;
};

struct SSPRenderer {
    struct SSPVulkanContext vulkan_context;
};

enum SSP_ERROR_CODE ssp_renderer_create(struct SSPRenderer *pRenderer, const char *app_name, const struct Version app_version);
void ssp_renderer_destroy(struct SSPRenderer *pRenderer);

#endif

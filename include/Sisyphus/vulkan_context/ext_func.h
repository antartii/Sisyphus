#pragma once

#include <vulkan/vulkan.h>

#include <Sisyphus/errors.h>

#ifdef __cplusplus
extern "C" {
#endif

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

    PFN_vkCreateImage vkCreateImage;
    PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
    PFN_vkBindImageMemory vkBindImageMemory;
    PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;

    PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;

    #ifdef DEBUG
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    #endif
    PFN_vkDestroyInstance vkDestroyInstance;

    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
    #endif
    PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;

    PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR;
    PFN_vkCreateSampler vkCreateSampler;
    PFN_vkDestroySampler vkDestroySampler;

    PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
};

enum SSP_ERROR_CODE ssp_vulkan_ext_func_instance_global(struct SSPVulkanContextExtFunc *ext_func);
enum SSP_ERROR_CODE ssp_vulkan_ext_func_instance_nglobal(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance);
enum SSP_ERROR_CODE ssp_vulkan_ext_func_device(struct SSPVulkanContextExtFunc *ext_func, VkDevice logical_device);

#ifdef __cplusplus
    }
#endif

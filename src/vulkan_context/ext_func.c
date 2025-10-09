#include "vulkan_context/ext_func.h"

enum SSP_ERROR_CODE ssp_vulkan_ext_func_instance_global(struct SSPVulkanContextExtFunc *ext_func)
{
    ext_func->vkCreateInstance = (PFN_vkCreateInstance) vkGetInstanceProcAddr(NULL, "vkCreateInstance");
    ext_func->vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
    ext_func->vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
    ext_func->vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");    

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_ext_func_instance_nglobal(struct SSPVulkanContextExtFunc *ext_func, VkInstance instance)
{
    ext_func->vkDestroyInstance = (PFN_vkDestroyInstance) vkGetInstanceProcAddr(instance, "vkDestroyInstance");
    #ifdef DEBUG
    ext_func->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    ext_func->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    #endif
    ext_func->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices");
    ext_func->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties");
    ext_func->vkCreateDevice = (PFN_vkCreateDevice) vkGetInstanceProcAddr(instance, "vkCreateDevice");
    ext_func->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    ext_func->vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateWaylandSurfaceKHR");
    #endif
    ext_func->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR");
    ext_func->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    ext_func->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    ext_func->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    ext_func->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties");

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_ext_func_device(struct SSPVulkanContextExtFunc *ext_func, VkDevice logical_device)
{
    ext_func->vkDestroyDevice = (PFN_vkDestroyDevice) vkGetDeviceProcAddr(logical_device, "vkDestroyDevice");
    ext_func->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle) vkGetDeviceProcAddr(logical_device, "vkDeviceWaitIdle");
    ext_func->vkCreateCommandPool = (PFN_vkCreateCommandPool) vkGetDeviceProcAddr(logical_device, "vkCreateCommandPool");
    ext_func->vkDestroyCommandPool = (PFN_vkDestroyCommandPool) vkGetDeviceProcAddr(logical_device, "vkDestroyCommandPool");
    ext_func->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers) vkGetDeviceProcAddr(logical_device, "vkAllocateCommandBuffers");
    ext_func->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers) vkGetDeviceProcAddr(logical_device, "vkFreeCommandBuffers");
    ext_func->vkResetCommandBuffer = (PFN_vkResetCommandBuffer) vkGetDeviceProcAddr(logical_device, "vkResetCommandBuffer");
    ext_func->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer) vkGetDeviceProcAddr(logical_device, "vkBeginCommandBuffer");
    ext_func->vkEndCommandBuffer = (PFN_vkEndCommandBuffer) vkGetDeviceProcAddr(logical_device, "vkEndCommandBuffer");
    ext_func->vkGetDeviceQueue = (PFN_vkGetDeviceQueue) vkGetDeviceProcAddr(logical_device, "vkGetDeviceQueue");
    ext_func->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR) vkGetDeviceProcAddr(logical_device, "vkCreateSwapchainKHR");
    ext_func->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR) vkGetDeviceProcAddr(logical_device, "vkDestroySwapchainKHR");
    ext_func->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR) vkGetDeviceProcAddr(logical_device, "vkGetSwapchainImagesKHR");
    ext_func->vkDestroyImage = (PFN_vkDestroyImage) vkGetDeviceProcAddr(logical_device, "vkDestroyImage");
    ext_func->vkCreateImageView = (PFN_vkCreateImageView) vkGetDeviceProcAddr(logical_device, "vkCreateImageView");
    ext_func->vkDestroyImageView = (PFN_vkDestroyImageView) vkGetDeviceProcAddr(logical_device, "vkDestroyImageView");
    ext_func->vkCreateSemaphore = (PFN_vkCreateSemaphore) vkGetDeviceProcAddr(logical_device, "vkCreateSemaphore");
    ext_func->vkDestroySemaphore = (PFN_vkDestroySemaphore) vkGetDeviceProcAddr(logical_device, "vkDestroySemaphore");
    ext_func->vkCreateFence = (PFN_vkCreateFence) vkGetDeviceProcAddr(logical_device, "vkCreateFence");
    ext_func->vkDestroyFence = (PFN_vkDestroyFence) vkGetDeviceProcAddr(logical_device, "vkDestroyFence");
    ext_func->vkWaitForFences = (PFN_vkWaitForFences) vkGetDeviceProcAddr(logical_device, "vkWaitForFences");
    ext_func->vkResetFences = (PFN_vkResetFences) vkGetDeviceProcAddr(logical_device, "vkResetFences");
    ext_func->vkQueuePresentKHR = (PFN_vkQueuePresentKHR) vkGetDeviceProcAddr(logical_device, "vkQueuePresentKHR");
    ext_func->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) vkGetDeviceProcAddr(logical_device, "vkAcquireNextImageKHR");
    ext_func->vkCmdPipelineBarrier2KHR = (PFN_vkCmdPipelineBarrier2KHR) vkGetDeviceProcAddr(logical_device, "vkCmdPipelineBarrier2KHR");
    ext_func->vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) vkGetDeviceProcAddr(logical_device, "vkCmdEndRenderingKHR");
    ext_func->vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) vkGetDeviceProcAddr(logical_device, "vkCmdBeginRenderingKHR");
    ext_func->vkQueueSubmit = (PFN_vkQueueSubmit) vkGetDeviceProcAddr(logical_device, "vkQueueSubmit");
    ext_func->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout) vkGetDeviceProcAddr(logical_device, "vkCreateDescriptorSetLayout");
    ext_func->vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout) vkGetDeviceProcAddr(logical_device, "vkDestroyDescriptorSetLayout");

    ext_func->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines) vkGetDeviceProcAddr(logical_device, "vkCreateGraphicsPipelines");
    ext_func->vkDestroyShaderModule = (PFN_vkDestroyShaderModule) vkGetDeviceProcAddr(logical_device, "vkDestroyShaderModule");
    ext_func->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout) vkGetDeviceProcAddr(logical_device, "vkCreatePipelineLayout");
    ext_func->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout) vkGetDeviceProcAddr(logical_device, "vkDestroyPipelineLayout");
    ext_func->vkDestroyPipeline = (PFN_vkDestroyPipeline) vkGetDeviceProcAddr(logical_device, "vkDestroyPipeline");
    ext_func->vkCreateShaderModule = (PFN_vkCreateShaderModule) vkGetDeviceProcAddr(logical_device, "vkCreateShaderModule");

    ext_func->vkMapMemory = (PFN_vkMapMemory) vkGetDeviceProcAddr(logical_device, "vkMapMemory");
    ext_func->vkUnmapMemory = (PFN_vkUnmapMemory) vkGetDeviceProcAddr(logical_device, "vkUnmapMemory");
    ext_func->vkCreateBuffer = (PFN_vkCreateBuffer) vkGetDeviceProcAddr(logical_device, "vkCreateBuffer");
    ext_func->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements) vkGetDeviceProcAddr(logical_device, "vkGetBufferMemoryRequirements");
    ext_func->vkAllocateMemory = (PFN_vkAllocateMemory) vkGetDeviceProcAddr(logical_device, "vkAllocateMemory");
    ext_func->vkBindBufferMemory = (PFN_vkBindBufferMemory) vkGetDeviceProcAddr(logical_device, "vkBindBufferMemory");
    ext_func->vkFreeMemory = (PFN_vkFreeMemory) vkGetDeviceProcAddr(logical_device, "vkFreeMemory");
    ext_func->vkDestroyBuffer = (PFN_vkDestroyBuffer) vkGetDeviceProcAddr(logical_device, "vkDestroyBuffer");
    ext_func->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool) vkGetDeviceProcAddr(logical_device, "vkCreateDescriptorPool");
    ext_func->vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool) vkGetDeviceProcAddr(logical_device, "vkDestroyDescriptorPool");

    ext_func->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets) vkGetDeviceProcAddr(logical_device, "vkAllocateDescriptorSets");
    ext_func->vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets) vkGetDeviceProcAddr(logical_device, "vkFreeDescriptorSets");
    ext_func->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets) vkGetDeviceProcAddr(logical_device, "vkUpdateDescriptorSets");

    ext_func->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer) vkGetDeviceProcAddr(logical_device, "vkCmdCopyBuffer");
    ext_func->vkQueueWaitIdle = (PFN_vkQueueWaitIdle) vkGetDeviceProcAddr(logical_device, "vkQueueWaitIdle");

    ext_func->vkCmdBindPipeline = (PFN_vkCmdBindPipeline) vkGetDeviceProcAddr(logical_device, "vkCmdBindPipeline");
    ext_func->vkCmdSetViewport = (PFN_vkCmdSetViewport) vkGetDeviceProcAddr(logical_device, "vkCmdSetViewport");
    ext_func->vkCmdSetScissor = (PFN_vkCmdSetScissor) vkGetDeviceProcAddr(logical_device, "vkCmdSetScissor");
    ext_func->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets) vkGetDeviceProcAddr(logical_device, "vkCmdBindDescriptorSets");

    ext_func->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers) vkGetDeviceProcAddr(logical_device, "vkCmdBindVertexBuffers");
    ext_func->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer) vkGetDeviceProcAddr(logical_device, "vkCmdBindIndexBuffer");
    ext_func->vkCmdPushConstants = (PFN_vkCmdPushConstants) vkGetDeviceProcAddr(logical_device, "vkCmdPushConstants");
    ext_func->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed) vkGetDeviceProcAddr(logical_device, "vkCmdDrawIndexed");

    ext_func->vkCreateImage = (PFN_vkCreateImage) vkGetDeviceProcAddr(logical_device, "vkCreateImage");
    ext_func->vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements) vkGetDeviceProcAddr(logical_device, "vkGetImageMemoryRequirements");
    ext_func->vkBindImageMemory = (PFN_vkBindImageMemory) vkGetDeviceProcAddr(logical_device, "vkBindImageMemory");
    ext_func->vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage) vkGetDeviceProcAddr(logical_device, "vkCmdCopyBufferToImage");

    ext_func->vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier) vkGetDeviceProcAddr(logical_device, "vkCmdPipelineBarrier");

    return SSP_ERROR_CODE_SUCCESS;
}

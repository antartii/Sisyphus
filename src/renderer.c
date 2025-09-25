#include "renderer.h"
#include "object.h"

static const char *ssp_vulkan_layer_names[] = {
    #ifdef DEBUG
    "VK_LAYER_KHRONOS_validation",
    #endif
};
static const size_t ssp_vulkan_instance_layer_names_size = ARRAY_SIZE(ssp_vulkan_layer_names);
static const char *ssp_vulkan_instance_extensions_names[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    #ifdef DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    #endif
    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
    #endif
};
static const size_t ssp_vulkan_instance_extensions_names_size = ARRAY_SIZE(ssp_vulkan_instance_extensions_names);
static const char *ssp_vulkan_device_extensions_name[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
    VK_KHR_SPIRV_1_4_EXTENSION_NAME,
};
static const size_t ssp_vulkan_device_extensions_name_size = ARRAY_SIZE(ssp_vulkan_device_extensions_name);

static bool ssp_vulkan_check_layer_names(struct SSPVulkanContextExtFunc *ext_func)
{
    uint32_t layers_properties_count;
    ext_func->vkEnumerateInstanceLayerProperties(&layers_properties_count, NULL);
    VkLayerProperties *layers_properties = malloc(sizeof(VkLayerProperties) * layers_properties_count);
    ext_func->vkEnumerateInstanceLayerProperties(&layers_properties_count, layers_properties);

    for (size_t i = 0; i < ssp_vulkan_instance_layer_names_size; ++i) {
        bool is_included = false;

        for (uint32_t j = 0; j < layers_properties_count; ++j) {
            if (!strcmp(layers_properties[j].layerName, ssp_vulkan_layer_names[i])) {
                is_included = true;
                break;
            }
        }
        if (!is_included)
            return false;
    }

    free(layers_properties);
    return true;
}

static bool ssp_vulkan_check_extensions_names(struct SSPVulkanContextExtFunc *ext_func)
{
    uint32_t extensions_properties_count;
    ext_func->vkEnumerateInstanceExtensionProperties(NULL, &extensions_properties_count, NULL);
    VkExtensionProperties *extensions_properties = malloc(sizeof(VkExtensionProperties) * extensions_properties_count);
    ext_func->vkEnumerateInstanceExtensionProperties(NULL, &extensions_properties_count, extensions_properties);
    
    for (size_t i = 0; i < ssp_vulkan_instance_extensions_names_size; ++i) {
        bool is_included = false;

        for (uint32_t j = 0; j < extensions_properties_count; ++j) {
            if (!strcmp(extensions_properties[j].extensionName, ssp_vulkan_instance_extensions_names[i])) {
                is_included = true;
                break;
            }
        }
        if (!is_included)
            return false;
    }

    free(extensions_properties);
    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL ssp_vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        fprintf(stderr, "Validation layer: type %" PRIu32 " msg: %s\n", type, pCallbackData->pMessage);

    return VK_FALSE;
}

#ifdef DEBUG
static enum SSP_ERROR_CODE ssp_vulkan_setup_debug_messenger(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkDebugUtilsMessageSeverityFlagsEXT severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    VkDebugUtilsMessageTypeFlagsEXT message_type_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .messageSeverity = severity_flags,
        .messageType = message_type_flags,
        .pfnUserCallback = ssp_vulkan_debug_callback
    };

    ext_func->vkCreateDebugUtilsMessengerEXT(pContext->instance, &debug_messenger_info, NULL, &pContext->debug_messenger) == VK_SUCCESS;

    return SSP_ERROR_CODE_SUCCESS;
}
#endif

static enum SSP_ERROR_CODE ssp_vulkan_create_instance(struct SSPVulkanContext *pContext, const char *app_name, const struct SSPVersion app_version)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    uint32_t instance_version;
    ext_func->vkEnumerateInstanceVersion(&instance_version);
    if (instance_version < SSP_VULKAN_API_VERSION)
        return SSP_ERROR_CODE_VULKAN_LOADER_SUPPORT_INSTANCE_VERSION;
    if (!ssp_vulkan_check_layer_names(ext_func))
        return SSP_ERROR_CODE_VULKAN_INSTANCE_LAYERS;
    if (!ssp_vulkan_check_extensions_names(ext_func))
        return SSP_ERROR_CODE_VULKAN_INSTANCE_EXTENSIONS;

    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pEngineName = SSP_ENGINE_NAME;
    app_info.engineVersion = VK_MAKE_VERSION(SSP_VERSION_MAJOR, SSP_VERSION_MINOR, SSP_VERSION_PATCH);
    app_info.pApplicationName = (app_name) ? app_name : SSP_RENDERER_APP_INFO_NAME_DEFAULT;
    app_info.applicationVersion = VK_MAKE_VERSION(app_version.major, app_version.minor, app_version.patch);
    app_info.apiVersion = SSP_VULKAN_API_VERSION;

    VkInstanceCreateInfo instance_create_info = {0};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledLayerCount = ssp_vulkan_instance_layer_names_size;
    instance_create_info.ppEnabledLayerNames = ssp_vulkan_layer_names;
    instance_create_info.enabledExtensionCount = ssp_vulkan_instance_extensions_names_size;
    instance_create_info.ppEnabledExtensionNames = ssp_vulkan_instance_extensions_names;

    if (ext_func->vkCreateInstance(&instance_create_info, NULL, &(pContext->instance)) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_INSTANCE_CREATE;

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_ext_func_instance_global(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    ext_func->vkCreateInstance = (PFN_vkCreateInstance) vkGetInstanceProcAddr(NULL, "vkCreateInstance");
    ext_func->vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
    ext_func->vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceLayerProperties");
    ext_func->vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties) vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");    

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_ext_func_instance_nglobal(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    ext_func->vkDestroyInstance = (PFN_vkDestroyInstance) vkGetInstanceProcAddr(pContext->instance, "vkDestroyInstance");
    #ifdef DEBUG
    ext_func->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(pContext->instance, "vkCreateDebugUtilsMessengerEXT");
    ext_func->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(pContext->instance, "vkDestroyDebugUtilsMessengerEXT");
    #endif
    ext_func->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) vkGetInstanceProcAddr(pContext->instance, "vkEnumeratePhysicalDevices");
    ext_func->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties) vkGetInstanceProcAddr(pContext->instance, "vkGetPhysicalDeviceProperties");
    ext_func->vkCreateDevice = (PFN_vkCreateDevice) vkGetInstanceProcAddr(pContext->instance, "vkCreateDevice");
    ext_func->vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties) vkGetInstanceProcAddr(pContext->instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    ext_func->vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR) vkGetInstanceProcAddr(pContext->instance, "vkCreateWaylandSurfaceKHR");
    #endif
    ext_func->vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) vkGetInstanceProcAddr(pContext->instance, "vkDestroySurfaceKHR");
    ext_func->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR) vkGetInstanceProcAddr(pContext->instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    ext_func->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR) vkGetInstanceProcAddr(pContext->instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    ext_func->vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR) vkGetInstanceProcAddr(pContext->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    ext_func->vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties) vkGetInstanceProcAddr(pContext->instance, "vkGetPhysicalDeviceMemoryProperties");

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_ext_func_device(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    ext_func->vkDestroyDevice = (PFN_vkDestroyDevice) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyDevice");
    ext_func->vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle) vkGetDeviceProcAddr(pContext->logical_device, "vkDeviceWaitIdle");
    ext_func->vkCreateCommandPool = (PFN_vkCreateCommandPool) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateCommandPool");
    ext_func->vkDestroyCommandPool = (PFN_vkDestroyCommandPool) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyCommandPool");
    ext_func->vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers) vkGetDeviceProcAddr(pContext->logical_device, "vkAllocateCommandBuffers");
    ext_func->vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers) vkGetDeviceProcAddr(pContext->logical_device, "vkFreeCommandBuffers");
    ext_func->vkResetCommandBuffer = (PFN_vkResetCommandBuffer) vkGetDeviceProcAddr(pContext->logical_device, "vkResetCommandBuffer");
    ext_func->vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer) vkGetDeviceProcAddr(pContext->logical_device, "vkBeginCommandBuffer");
    ext_func->vkEndCommandBuffer = (PFN_vkEndCommandBuffer) vkGetDeviceProcAddr(pContext->logical_device, "vkEndCommandBuffer");
    ext_func->vkGetDeviceQueue = (PFN_vkGetDeviceQueue) vkGetDeviceProcAddr(pContext->logical_device, "vkGetDeviceQueue");
    ext_func->vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateSwapchainKHR");
    ext_func->vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroySwapchainKHR");
    ext_func->vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR) vkGetDeviceProcAddr(pContext->logical_device, "vkGetSwapchainImagesKHR");
    ext_func->vkDestroyImage = (PFN_vkDestroyImage) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyImage");
    ext_func->vkCreateImageView = (PFN_vkCreateImageView) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateImageView");
    ext_func->vkDestroyImageView = (PFN_vkDestroyImageView) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyImageView");
    ext_func->vkCreateSemaphore = (PFN_vkCreateSemaphore) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateSemaphore");
    ext_func->vkDestroySemaphore = (PFN_vkDestroySemaphore) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroySemaphore");
    ext_func->vkCreateFence = (PFN_vkCreateFence) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateFence");
    ext_func->vkDestroyFence = (PFN_vkDestroyFence) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyFence");
    ext_func->vkWaitForFences = (PFN_vkWaitForFences) vkGetDeviceProcAddr(pContext->logical_device, "vkWaitForFences");
    ext_func->vkResetFences = (PFN_vkResetFences) vkGetDeviceProcAddr(pContext->logical_device, "vkResetFences");
    ext_func->vkQueuePresentKHR = (PFN_vkQueuePresentKHR) vkGetDeviceProcAddr(pContext->logical_device, "vkQueuePresentKHR");
    ext_func->vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) vkGetDeviceProcAddr(pContext->logical_device, "vkAcquireNextImageKHR");
    ext_func->vkCmdPipelineBarrier2KHR = (PFN_vkCmdPipelineBarrier2KHR) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdPipelineBarrier2KHR");
    ext_func->vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdEndRenderingKHR");
    ext_func->vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdBeginRenderingKHR");
    ext_func->vkQueueSubmit = (PFN_vkQueueSubmit) vkGetDeviceProcAddr(pContext->logical_device, "vkQueueSubmit");
    ext_func->vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateDescriptorSetLayout");
    ext_func->vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyDescriptorSetLayout");

    ext_func->vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateGraphicsPipelines");
    ext_func->vkDestroyShaderModule = (PFN_vkDestroyShaderModule) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyShaderModule");
    ext_func->vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout) vkGetDeviceProcAddr(pContext->logical_device, "vkCreatePipelineLayout");
    ext_func->vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyPipelineLayout");
    ext_func->vkDestroyPipeline = (PFN_vkDestroyPipeline) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyPipeline");
    ext_func->vkCreateShaderModule = (PFN_vkCreateShaderModule) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateShaderModule");

    ext_func->vkMapMemory = (PFN_vkMapMemory) vkGetDeviceProcAddr(pContext->logical_device, "vkMapMemory");
    ext_func->vkUnmapMemory = (PFN_vkUnmapMemory) vkGetDeviceProcAddr(pContext->logical_device, "vkUnmapMemory");
    ext_func->vkCreateBuffer = (PFN_vkCreateBuffer) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateBuffer");
    ext_func->vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements) vkGetDeviceProcAddr(pContext->logical_device, "vkGetBufferMemoryRequirements");
    ext_func->vkAllocateMemory = (PFN_vkAllocateMemory) vkGetDeviceProcAddr(pContext->logical_device, "vkAllocateMemory");
    ext_func->vkBindBufferMemory = (PFN_vkBindBufferMemory) vkGetDeviceProcAddr(pContext->logical_device, "vkBindBufferMemory");
    ext_func->vkFreeMemory = (PFN_vkFreeMemory) vkGetDeviceProcAddr(pContext->logical_device, "vkFreeMemory");
    ext_func->vkDestroyBuffer = (PFN_vkDestroyBuffer) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyBuffer");
    ext_func->vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool) vkGetDeviceProcAddr(pContext->logical_device, "vkCreateDescriptorPool");
    ext_func->vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool) vkGetDeviceProcAddr(pContext->logical_device, "vkDestroyDescriptorPool");

    ext_func->vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets) vkGetDeviceProcAddr(pContext->logical_device, "vkAllocateDescriptorSets");
    ext_func->vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets) vkGetDeviceProcAddr(pContext->logical_device, "vkFreeDescriptorSets");
    ext_func->vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets) vkGetDeviceProcAddr(pContext->logical_device, "vkUpdateDescriptorSets");

    ext_func->vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdCopyBuffer");
    ext_func->vkQueueWaitIdle = (PFN_vkQueueWaitIdle) vkGetDeviceProcAddr(pContext->logical_device, "vkQueueWaitIdle");

    ext_func->vkCmdBindPipeline = (PFN_vkCmdBindPipeline) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdBindPipeline");
    ext_func->vkCmdSetViewport = (PFN_vkCmdSetViewport) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdSetViewport");
    ext_func->vkCmdSetScissor = (PFN_vkCmdSetScissor) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdSetScissor");
    ext_func->vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdBindDescriptorSets");

    ext_func->vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdBindVertexBuffers");
    ext_func->vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdBindIndexBuffer");
    ext_func->vkCmdPushConstants = (PFN_vkCmdPushConstants) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdPushConstants");
    ext_func->vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed) vkGetDeviceProcAddr(pContext->logical_device, "vkCmdDrawIndexed");

    return SSP_ERROR_CODE_SUCCESS;
}

static int ssp_vulkan_physical_device_score(VkPhysicalDevice physical_device, struct SSPVulkanContextExtFunc *pExt_func)
{
    VkPhysicalDeviceProperties properties;
    pExt_func->vkGetPhysicalDeviceProperties(physical_device, &properties);

    int score = 0;

    if (properties.apiVersion < SSP_VULKAN_API_VERSION)
        return -1;

    int device_type_score;
    switch (properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            device_type_score = 3;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            device_type_score = 2;
            break;
        default:
            device_type_score = 1;
            break;
    }
    score += device_type_score;
    return score;
}

static uint32_t ssp_vulkan_choose_physical_device(VkPhysicalDevice *physical_devices, uint32_t physical_devices_count, struct SSPVulkanContextExtFunc *pExt_func)
{
    uint32_t selected_physical_device_index = 0;
    int selected_physical_device_score = 0;

    int physical_device_score = 0;

    for (uint32_t i = 0; i < physical_devices_count; ++i) {
        physical_device_score = ssp_vulkan_physical_device_score(physical_devices[i], pExt_func);
        if (physical_device_score > selected_physical_device_score && physical_device_score >= 0) {
            selected_physical_device_index = i;
            selected_physical_device_score = physical_device_score;
        }
    }

    return selected_physical_device_index;
}

static enum SSP_ERROR_CODE ssp_vulkan_init_physical_device(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    uint32_t physical_devices_count;
    ext_func->vkEnumeratePhysicalDevices(pContext->instance, &physical_devices_count, NULL);
    VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) * physical_devices_count);
    ext_func->vkEnumeratePhysicalDevices(pContext->instance, &physical_devices_count, physical_devices);

    pContext->physical_device = physical_devices[ssp_vulkan_choose_physical_device(physical_devices, physical_devices_count, ext_func)];

    free(physical_devices);
    return SSP_ERROR_CODE_SUCCESS;
}

/**
 * @brief gather families indices needed from the one availabled.
 * 1 dedicated transfer queue if it can, if it can't fall back to the graphic queue.
 * 1 queue for both graphic operations and present if it can, otherwise take any present queue and any graphic queue.
 * 
 * @param pContext 
 * @param queue_family_properties 
 * @param queue_family_properties_count 
 * @param queue_families_indices 
 * @return enum SSP_ERROR_CODE 
 */
static enum SSP_ERROR_CODE ssp_vulkan_get_queue_families_indices(struct SSPVulkanContext *pContext, VkQueueFamilyProperties *queue_family_properties, uint32_t queue_family_properties_count, struct SSPVulkanQueueFamiliesIndices *queue_families_indices) {
    struct SSPVulkanQueueFamiliesIndices indices = {0};
    bool transfer_queue_found = false;
    bool graphic_queue_found = false;
    bool present_queue_found = false;

    for (uint32_t i = 0; i < queue_family_properties_count; ++i) {
        VkQueueFlags flag = queue_family_properties[i].queueFlags;

        if (!transfer_queue_found && (flag & VK_QUEUE_TRANSFER_BIT) && !(flag & VK_QUEUE_GRAPHICS_BIT) && !(flag & VK_QUEUE_COMPUTE_BIT)) {
            transfer_queue_found = true;
            indices.transfer = i;
        }
        VkBool32 present_mode_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(pContext->physical_device, i, pContext->surface, &present_mode_supported);
        if (!graphic_queue_found && flag & VK_QUEUE_GRAPHICS_BIT && !present_queue_found && present_mode_supported) {
            graphic_queue_found = true;
            indices.graphic = i;
            present_queue_found = true;
            indices.present = i;
        }

        if (present_queue_found && graphic_queue_found && transfer_queue_found)
            break;
    }

    if (!graphic_queue_found || !present_queue_found) {
        for (uint32_t i = 0; i < queue_family_properties_count; ++i) {
            VkQueueFlags flag = queue_family_properties[i].queueFlags;
            VkBool32 present_mode_supported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(pContext->physical_device, i, pContext->surface, &present_mode_supported);

            if (!graphic_queue_found && flag & VK_QUEUE_GRAPHICS_BIT) {
                graphic_queue_found = true;
                indices.graphic = i;
            }
            if (!present_queue_found && present_mode_supported) {
                present_queue_found = true;
                indices.present = i;
            }
            if (present_queue_found && graphic_queue_found)
                break;
        }
        if(!present_queue_found || !graphic_queue_found)
            return SSP_ERROR_CODE_VULKAN_QUEUE_FAMILIES_NOT_VALID;
    }

    if (!transfer_queue_found)
        indices.transfer = indices.graphic;

    *queue_families_indices = indices;
    queue_families_indices->queue_count = 1
        + (indices.present != indices.graphic)
        + (indices.transfer != indices.graphic && indices.transfer != indices.present);
    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_surface(struct SSPVulkanContext *pContext, struct SSPWindow *window)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    enum SSP_ERROR_CODE result = SSP_ERROR_CODE_VULKAN_INVALID_SURFACE;

    #ifdef VK_USE_PLATFORM_WAYLAND_KHR
    VkWaylandSurfaceCreateInfoKHR surface_create_info = {0};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surface_create_info.display = window->surface_context.display;
    surface_create_info.surface = window->surface_context.surface;
    if (ext_func->vkCreateWaylandSurfaceKHR(pContext->instance, &surface_create_info, NULL, &pContext->surface) == VK_SUCCESS)
        result = SSP_ERROR_CODE_SUCCESS;
    #endif

    return result;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_command_pools(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    
    VkCommandPoolCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    create_info.queueFamilyIndex = pContext->queue_family_indices.graphic;

    int index = 0;
    pContext->command_pools = calloc(pContext->queue_family_indices.queue_count, sizeof(VkCommandPool));

    ext_func->vkCreateCommandPool(pContext->logical_device, &create_info, NULL, &(pContext->command_pools[index]));
    pContext->graphic_command_pool_index = index++;
    
    if (pContext->queue_family_indices.transfer == pContext->queue_family_indices.graphic)
        pContext->transfer_command_pool_index = pContext->graphic_command_pool_index;
    else {
        ext_func->vkCreateCommandPool(pContext->logical_device, &create_info, NULL, &pContext->command_pools[index]);
        pContext->transfer_command_pool_index = index++;
    }

    pContext->command_pools_size = index;
    #ifdef DEBUG
    printf("Using %d command buffers\n", index);
    #endif

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_allocate_command_buffers(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    alloc_info.commandPool = pContext->command_pools[pContext->graphic_command_pool_index];
    alloc_info.commandBufferCount = SSP_MAX_FRAMES_IN_FLIGHT;
    pContext->graphic_command_buffers = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkCommandBuffer));
    if (ext_func->vkAllocateCommandBuffers(pContext->logical_device, &alloc_info, pContext->graphic_command_buffers) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_ALLOCATION_GRAPHIC;

    alloc_info.commandPool = pContext->command_pools[pContext->transfer_command_pool_index];
    alloc_info.commandBufferCount = SSP_TRANSFER_COMMAND_BUFFERS_COUNT;
    pContext->transfer_command_buffers = calloc(SSP_TRANSFER_COMMAND_BUFFERS_COUNT, sizeof(VkCommandBuffer));
    if (ext_func->vkAllocateCommandBuffers(pContext->logical_device, &alloc_info, pContext->transfer_command_buffers) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_ALLOCATION_TRANSFER;

    return SSP_ERROR_CODE_SUCCESS;
}

static VkSurfaceFormatKHR ssp_vulkan_swapchain_choose_format(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    uint32_t surface_formats_count;
    ext_func->vkGetPhysicalDeviceSurfaceFormatsKHR(pContext->physical_device, pContext->surface, &surface_formats_count, NULL);
    VkSurfaceFormatKHR *surface_formats = malloc(sizeof(VkSurfaceFormatKHR) * surface_formats_count);
    ext_func->vkGetPhysicalDeviceSurfaceFormatsKHR(pContext->physical_device, pContext->surface, &surface_formats_count, surface_formats);
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

static VkPresentModeKHR ssp_vulkan_swapchain_choose_present_mode(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    uint32_t present_modes_count;
    ext_func->vkGetPhysicalDeviceSurfacePresentModesKHR(pContext->physical_device, pContext->surface, &present_modes_count, NULL);
    VkPresentModeKHR *present_modes = malloc(sizeof(VkPresentModeKHR) * present_modes_count);
    ext_func->vkGetPhysicalDeviceSurfacePresentModesKHR(pContext->physical_device, pContext->surface, &present_modes_count, present_modes);
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


static enum SSP_ERROR_CODE ssp_vulkan_create_swapchain(struct SSPVulkanContext *pContext, struct SSPWindow *window)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkSurfaceCapabilitiesKHR surface_capabilities;
    ext_func->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pContext->physical_device, pContext->surface, &surface_capabilities);

    VkSurfaceFormatKHR surface_format = ssp_vulkan_swapchain_choose_format(pContext);
    pContext->swapchain_image_format = surface_format.format;
    VkPresentModeKHR present_mode = ssp_vulkan_swapchain_choose_present_mode(pContext);
    pContext->swapchain_extent = ssp_vulkan_swapchain_choose_extent(surface_capabilities.currentExtent, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent, window);
    if (pContext->swapchain_extent.width == 0 || pContext->swapchain_extent.height == 0)
        return SSP_ERROR_CODE_VULKAN_SWAPCHAIN_EXTENT;

    VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    
    if (!(surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR))
        composite_alpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

    VkSwapchainCreateInfoKHR create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = pContext->surface;
    create_info.clipped = VK_TRUE;
    create_info.compositeAlpha = composite_alpha;
    create_info.imageArrayLayers = 1;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = pContext->swapchain_extent;
    create_info.imageFormat = pContext->swapchain_image_format;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.minImageCount = surface_capabilities.minImageCount;
    create_info.presentMode = present_mode;
    create_info.preTransform = surface_capabilities.currentTransform;

    if (pContext->queue_family_indices.graphic == pContext->queue_family_indices.present)
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    else {
        uint32_t queue_family_indices_arr[] = {pContext->queue_family_indices.graphic, pContext->queue_family_indices.present};
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.pQueueFamilyIndices = queue_family_indices_arr;
        create_info.queueFamilyIndexCount = 2;
    }

    if (ext_func->vkCreateSwapchainKHR(pContext->logical_device, &create_info, NULL, &pContext->swapchain) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_SWAPCHAIN_CREATION;

    pContext->swapchain_images_count = surface_capabilities.minImageCount;
    pContext->swapchain_images = calloc(pContext->swapchain_images_count, sizeof(VkImage));
    ext_func->vkGetSwapchainImagesKHR(pContext->logical_device, pContext->swapchain, &pContext->swapchain_images_count, pContext->swapchain_images);

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_logical_device(struct SSPVulkanContext *pContext, struct SSPWindow *window)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    enum SSP_ERROR_CODE error_code;
    
    uint32_t queue_family_properties_count;
    ext_func->vkGetPhysicalDeviceQueueFamilyProperties(pContext->physical_device, &queue_family_properties_count, NULL);
    VkQueueFamilyProperties *queue_family_properties = malloc(sizeof(VkQueueFamilyProperties) * queue_family_properties_count);
    ext_func->vkGetPhysicalDeviceQueueFamilyProperties(pContext->physical_device, &queue_family_properties_count, queue_family_properties);

    if (error_code = ssp_vulkan_get_queue_families_indices(pContext, queue_family_properties, queue_family_properties_count, &pContext->queue_family_indices)) {
        free(queue_family_properties);
        return error_code;
    }

    VkDeviceQueueCreateInfo *queue_create_info = calloc(pContext->queue_family_indices.queue_count, sizeof(VkDeviceQueueCreateInfo));
    int queue_count = pContext->queue_family_indices.queue_count;
    float queue_priorities = 1.0f;

    for (int i = 0; i < pContext->queue_family_indices.queue_count; ++i) {
        queue_create_info[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info[i].queueCount = 1;
        queue_create_info[i].pQueuePriorities = &queue_priorities;
    }

    int index = 0;
    queue_create_info[index++].queueFamilyIndex = pContext->queue_family_indices.graphic;
    if (pContext->queue_family_indices.graphic != pContext->queue_family_indices.present)
        queue_create_info[index++].queueFamilyIndex = pContext->queue_family_indices.present;
    if (pContext->queue_family_indices.transfer != pContext->queue_family_indices.graphic)
        queue_create_info[index++].queueFamilyIndex = pContext->queue_family_indices.transfer;

    free(queue_family_properties);

    VkPhysicalDeviceSynchronization2FeaturesKHR sync_2_features = {0};
    sync_2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    sync_2_features.synchronization2 = VK_TRUE;

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = {0};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;
    dynamic_rendering_features.pNext = &sync_2_features;

    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = queue_count;
    create_info.pQueueCreateInfos = queue_create_info;
    create_info.enabledExtensionCount = ssp_vulkan_device_extensions_name_size;
    create_info.ppEnabledExtensionNames = ssp_vulkan_device_extensions_name;
    create_info.pNext = &dynamic_rendering_features;

    if (ext_func->vkCreateDevice(pContext->physical_device, &create_info, NULL, &pContext->logical_device) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_LOGICAL_DEVICE_CREATION;
    
    if ((error_code = ssp_vulkan_ext_func_device(pContext)))
        return error_code;

    free(queue_create_info);

    ext_func->vkGetDeviceQueue(pContext->logical_device, pContext->queue_family_indices.graphic, 0, &pContext->graphic_queue);
    if (pContext->queue_family_indices.graphic != pContext->queue_family_indices.present)
        ext_func->vkGetDeviceQueue(pContext->logical_device, pContext->queue_family_indices.present, 0, &pContext->present_queue);
    else
        pContext->present_queue = pContext->graphic_queue;
    
    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_image_view(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkImageViewCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.components.a = VK_COMPONENT_SWIZZLE_A;
    create_info.components.r = VK_COMPONENT_SWIZZLE_R;
    create_info.components.g = VK_COMPONENT_SWIZZLE_G;
    create_info.components.b = VK_COMPONENT_SWIZZLE_B;
    create_info.format = pContext->swapchain_image_format;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

    pContext->swapchain_image_views = calloc(pContext->swapchain_images_count, sizeof(VkImageView));
    for (int i = 0; i < pContext->swapchain_images_count; ++i) {
        create_info.image = pContext->swapchain_images[i];
        if (ext_func->vkCreateImageView(pContext->logical_device, &create_info, NULL, &(pContext->swapchain_image_views[i])) != VK_SUCCESS)
            return SSP_ERROR_CODE_VULKAN_CREATE_IMAGE_VIEW;
    }

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_transition_image_layout(
    uint32_t image_index,
    VkImageLayout old_layout,
    VkImageLayout new_layout,
    VkAccessFlags2 src_access_mask,
    VkAccessFlags2 dst_access_mask,
    VkPipelineStageFlags2 src_stage_mask,
    VkPipelineStageFlags2 dst_stage_mask,
    VkImage *swapchain_images,
    VkCommandBuffer command_buffer,
    struct SSPVulkanContextExtFunc *ext_func)
{
    VkImageMemoryBarrier2 barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.srcAccessMask = src_access_mask;
    barrier.dstAccessMask = dst_access_mask;
    barrier.srcStageMask = src_stage_mask;
    barrier.dstStageMask = dst_stage_mask;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = swapchain_images[image_index];
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkDependencyInfo dependency_info = {0};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &barrier;

    ext_func->vkCmdPipelineBarrier2KHR(command_buffer, &dependency_info);
    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_record_graphic_command_buffer(struct SSPVulkanContext *pContext, struct SSPDynamicArray *objects_to_draw)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    ext_func->vkResetCommandBuffer(pContext->graphic_command_buffers[pContext->current_frame], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    ext_func->vkBeginCommandBuffer(pContext->graphic_command_buffers[pContext->current_frame], &begin_info);
    ssp_vulkan_transition_image_layout(pContext->image_index, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, pContext->swapchain_images, pContext->graphic_command_buffers[pContext->current_frame], &pContext->ext_func);

    VkClearValue clear_color = {.color.float32 = {0.0, 0.0, 0.0, 1.0}}; // todo : define it at init

    VkRenderingAttachmentInfo rendering_attachment_info = {0};
    rendering_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    rendering_attachment_info.clearValue = clear_color;
    rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rendering_attachment_info.imageView = pContext->swapchain_image_views[pContext->image_index];
    rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo rendering_info = {0};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.renderArea.offset.x = 0;
    rendering_info.renderArea.offset.y = 0;
    rendering_info.renderArea.extent = pContext->swapchain_extent;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments = &rendering_attachment_info;
    rendering_info.layerCount = 1;

    ext_func->vkCmdBeginRenderingKHR(pContext->graphic_command_buffers[pContext->current_frame], &rendering_info);
    
    ext_func->vkCmdBindPipeline(pContext->graphic_command_buffers[pContext->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, pContext->graphic_pipeline);
    ext_func->vkCmdSetViewport(pContext->graphic_command_buffers[pContext->current_frame], 0, 1, &pContext->viewport);
    
    VkRect2D scissor = {0};
    scissor.extent = pContext->swapchain_extent;

    ext_func->vkCmdSetScissor(pContext->graphic_command_buffers[pContext->current_frame], 0, 1, &scissor);
    ext_func->vkCmdBindDescriptorSets(pContext->graphic_command_buffers[pContext->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, pContext->pipeline_layout, 0, 1, &(pContext->descriptor_sets[pContext->current_frame]), 0, NULL);

    VkDeviceSize offset = 0;

    for (size_t i = 0; i < objects_to_draw->size; ++i) {
        struct SSPObject *object = objects_to_draw->data[i];

        if (!object)
            continue;

        ext_func->vkCmdBindVertexBuffers(pContext->graphic_command_buffers[pContext->current_frame], 0, 1, &object->vertex_buffer, &offset);
        ext_func->vkCmdBindIndexBuffer(pContext->graphic_command_buffers[pContext->current_frame], object->index_buffer, offset, VK_INDEX_TYPE_UINT16);
        ext_func->vkCmdPushConstants(pContext->graphic_command_buffers[pContext->current_frame], pContext->pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(struct SSPShaderPushConstant), &object->vertex_push_constant);
        ext_func->vkCmdDrawIndexed(pContext->graphic_command_buffers[pContext->current_frame], object->indices_count, 1, 0, 0, 0);
    }
    objects_to_draw->size = 0;

    ext_func->vkCmdEndRenderingKHR(pContext->graphic_command_buffers[pContext->current_frame]);
    ssp_vulkan_transition_image_layout(pContext->image_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, 0, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, pContext->swapchain_images, pContext->graphic_command_buffers[pContext->current_frame], &pContext->ext_func);
    ext_func->vkEndCommandBuffer(pContext->graphic_command_buffers[pContext->current_frame]);

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_renderer_swapchain_destroy(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    if (pContext->swapchain_image_views) {
        for (int i = 0; i < pContext->swapchain_images_count; ++i)
            ext_func->vkDestroyImageView(pContext->logical_device, pContext->swapchain_image_views[i], NULL);
        free(pContext->swapchain_image_views);
    }

    free(pContext->swapchain_images);

    ext_func->vkDestroySwapchainKHR(pContext->logical_device, pContext->swapchain, NULL);
}

static enum SSP_ERROR_CODE ssp_renderer_resize(struct SSPVulkanContext *pContext, struct SSPWindow *window)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    if (window->height != 0 && window->width != 0) {
        ext_func->vkDeviceWaitIdle(pContext->logical_device);
        ssp_renderer_swapchain_destroy(pContext);

        ssp_vulkan_create_swapchain(pContext, window);
        ssp_vulkan_create_image_view(pContext);
    }

    window->resize_needed = false;

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_renderer_draw_frame(struct SSPRenderer *pRenderer, struct SSPWindow *window)
{
    struct SSPVulkanContext *pContext = &pRenderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    bool resize_after_present = false;

    ext_func->vkWaitForFences(pContext->logical_device, 1, &(pContext->frames_in_flight_fences[pContext->current_frame]), VK_TRUE, UINT64_MAX);

    if (window->resize_needed)
        return ssp_renderer_resize(pContext, window);

    VkResult result = ext_func->vkAcquireNextImageKHR(pContext->logical_device, pContext->swapchain, UINT64_MAX, pContext->present_complete_semaphores[pContext->current_frame], NULL, &pContext->image_index);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        return ssp_renderer_resize(pContext, window);
    else if (result == VK_SUBOPTIMAL_KHR)
        resize_after_present = true;

    ssp_vulkan_record_graphic_command_buffer(pContext, pRenderer->objects_to_draw);

    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pCommandBuffers = &(pContext->graphic_command_buffers[pContext->current_frame]);
    submit_info.commandBufferCount = 1;
    submit_info.pSignalSemaphores = &(pContext->render_finished_semaphores[pContext->current_frame]);
    submit_info.signalSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &(pContext->present_complete_semaphores[pContext->current_frame]);
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitDstStageMask = wait_stages;

    ext_func->vkResetFences(pContext->logical_device, 1, &(pContext->frames_in_flight_fences[pContext->current_frame]));
    ext_func->vkQueueSubmit(pContext->graphic_queue, 1, &submit_info, pContext->frames_in_flight_fences[pContext->current_frame]);

    VkPresentInfoKHR present_info = {0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &(pContext->render_finished_semaphores[pContext->current_frame]);
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &pContext->swapchain;
    present_info.pImageIndices = &pContext->image_index;

    result = ext_func->vkQueuePresentKHR(pContext->present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR
        || result == VK_SUBOPTIMAL_KHR
        || window->resize_needed
        || resize_after_present)
        ssp_renderer_resize(pContext, window);

    pContext->current_frame = (pContext->current_frame + 1) % SSP_MAX_FRAMES_IN_FLIGHT;

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_create_sync_objects(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    pContext->present_complete_semaphores = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    pContext->render_finished_semaphores = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore));
    pContext->frames_in_flight_fences = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkFence));

    VkSemaphoreCreateInfo semaphore_create_info = {0};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fence_create_info = {0};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i) {
        if (ext_func->vkCreateSemaphore(pContext->logical_device, &semaphore_create_info, NULL, &(pContext->present_complete_semaphores[i])))
            return SSP_ERROR_CODE_VULKAN_CREATE_PRESENT_COMPLETE_SEMAPHORES;
        if (ext_func->vkCreateSemaphore(pContext->logical_device, &semaphore_create_info, NULL, &(pContext->render_finished_semaphores[i])))
            return SSP_ERROR_CODE_VULKAN_CREATE_RENDER_FINISHED_SEMAPHORES;
        if (ext_func->vkCreateFence(pContext->logical_device, &fence_create_info, NULL, &(pContext->frames_in_flight_fences[i])) != VK_SUCCESS)
            return SSP_ERROR_CODE_VULKAN_CREATE_FRAMES_IN_FLIGHT_FENCES;
    }

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_set_layout(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkDescriptorSetLayoutBinding descriptor_binding = {0};

    descriptor_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptor_binding.descriptorCount = 1;
    descriptor_binding.pImmutableSamplers = NULL;
    descriptor_binding.binding = 0;

    VkDescriptorSetLayoutCreateInfo descriptor_info = {0};
    descriptor_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_info.bindingCount = 1;
    descriptor_info.pBindings = &descriptor_binding;

    if (ext_func->vkCreateDescriptorSetLayout(pContext->logical_device, &descriptor_info, NULL, &pContext->descriptor_set_layout) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_CREATE_DESCRIPTOR_SET_LAYOUT;
    return SSP_ERROR_CODE_SUCCESS;
}

static VkShaderModule ssp_vulkan_create_shader_module(struct SSPVulkanContextExtFunc *ext_func, VkDevice logical_device, const char *code, uint32_t code_size)
{
    VkShaderModuleCreateInfo shader_module_info = {0};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_info.codeSize = code_size;
    shader_module_info.pCode = (const uint32_t *) code;

    VkShaderModule shader_module;

    ext_func->vkCreateShaderModule(logical_device, &shader_module_info, NULL, &shader_module);

    return shader_module;
}

static int ssp_vulkan_find_memory_type(struct SSPVulkanContext *pContext, uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkPhysicalDeviceMemoryProperties memory_properties;
    ext_func->vkGetPhysicalDeviceMemoryProperties(pContext->physical_device, &memory_properties);

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    return -1;
}

enum SSP_ERROR_CODE ssp_vulkan_create_buffer(struct SSPVulkanContext *pContext, VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, VkBuffer *buffer, VkDeviceMemory *memory)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkBufferCreateInfo buffer_info = {0};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = buffer_usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (ext_func->vkCreateBuffer(pContext->logical_device, &buffer_info, NULL, buffer) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_CREATE_BUFFER;
    VkMemoryRequirements memory_requirements;
    ext_func->vkGetBufferMemoryRequirements(pContext->logical_device, *buffer, &memory_requirements);

    uint32_t memory_type_index = ssp_vulkan_find_memory_type(pContext, memory_requirements.memoryTypeBits, memory_properties);
    if (memory_type_index == -1)
        return SSP_ERROR_CODE_VULKAN_CREATE_BUFFER;

    VkMemoryAllocateInfo memory_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = memory_type_index
    };

    ext_func->vkAllocateMemory(pContext->logical_device, &memory_allocate_info, NULL, memory);
    ext_func->vkBindBufferMemory(pContext->logical_device, *buffer, *memory, 0);

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_copy_buffer(struct SSPVulkanContext *pContext, VkBuffer *src_buffer, VkBuffer *dst_buffer, VkDeviceSize size)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandBufferCount = 1;
    alloc_info.commandPool = pContext->command_pools[pContext->transfer_command_pool_index];
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer command_copy_buffer;
    ext_func->vkAllocateCommandBuffers(pContext->logical_device, &alloc_info, &command_copy_buffer);

    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    ext_func->vkBeginCommandBuffer(command_copy_buffer, &begin_info);

    VkBufferCopy copy_region = {0};
    copy_region.size = size;

    ext_func->vkCmdCopyBuffer(command_copy_buffer, *src_buffer, *dst_buffer, 1, &copy_region);
    ext_func->vkEndCommandBuffer(command_copy_buffer);

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_copy_buffer;

    ext_func->vkQueueSubmit(pContext->graphic_queue, 1, &submit_info, NULL);
    ext_func->vkQueueWaitIdle(pContext->graphic_queue);

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_uniform_buffers(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    pContext->uniform_buffers = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkBuffer));
    pContext->uniform_buffers_memory = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkDeviceMemory));
    pContext->uniform_buffers_mapped = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(void *));

    for (size_t i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i) {
        VkDeviceSize size = sizeof(struct SSPShaderUniformBuffer);

        if (ssp_vulkan_create_buffer(pContext, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &(pContext->uniform_buffers[i]), &(pContext->uniform_buffers_memory[i])) != SSP_ERROR_CODE_SUCCESS
            || ext_func->vkMapMemory(pContext->logical_device, pContext->uniform_buffers_memory[i], 0, size, 0, &(pContext->uniform_buffers_mapped[i])) != VK_SUCCESS)
            return SSP_ERROR_CODE_VULKAN_CREATE_UNIFORM_BUFFERS;
    }
    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_graphic_pipeline(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    uint32_t code_size;
    const char *shader_file = SSP_SHADERS_PATH;
    char *shader_code = read_file(shader_file, &code_size);

    VkShaderModule shader_module = ssp_vulkan_create_shader_module(&pContext->ext_func, pContext->logical_device, shader_code, code_size);
    
    VkPipelineShaderStageCreateInfo vert_stage_info = {0};

    vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_info.module = shader_module;
    vert_stage_info.pName = SSP_SHADER_VERTEX_ENTRY_POINT;
    

    VkPipelineShaderStageCreateInfo frag_stage_info = {0};
    frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_info.module = shader_module;
    frag_stage_info.pName = SSP_SHADER_FRAGMENT_ENTRY_POINT;

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_stage_info, frag_stage_info};

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info = {0};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = 2;
    dynamic_state_info.pDynamicStates = dynamic_states;

    VkVertexInputBindingDescription vertex_binding_descriptions = {0};
    vertex_binding_descriptions.binding = 0;
    vertex_binding_descriptions.stride = sizeof(struct SSPShaderVertex);
    vertex_binding_descriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertex_attribute_descriptions[2];
    vertex_attribute_descriptions[0].location = 0;
    vertex_attribute_descriptions[0].binding = 0;
    vertex_attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attribute_descriptions[0].offset = offsetof(struct SSPShaderVertex, pos);

    vertex_attribute_descriptions[1].location = 1;
    vertex_attribute_descriptions[1].binding = 0;
    vertex_attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attribute_descriptions[1].offset = offsetof(struct SSPShaderVertex, color);

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding_descriptions;
    vertex_input_info.vertexAttributeDescriptionCount = 2;
    vertex_input_info.pVertexAttributeDescriptions = vertex_attribute_descriptions;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {0};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    pContext->viewport = (VkViewport) {
        .x = 0,
        .y = 0,
        .height = (float) pContext->swapchain_extent.height,
        .width = (float) pContext->swapchain_extent.width,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkPipelineViewportStateCreateInfo viewport_state_info = {0};
    viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization_info = {0};
    rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_info.depthClampEnable = VK_FALSE;
    rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_info.depthBiasClamp = VK_FALSE;
    rasterization_info.depthBiasSlopeFactor = 1.0f;
    rasterization_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample_info = {0};
    multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_info.sampleShadingEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT
        | VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blend_info = {0};
    color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.logicOpEnable = VK_FALSE;
    color_blend_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;

    VkPushConstantRange push_constant_range = {0};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.size = sizeof(struct SSPShaderPushConstant);

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &pContext->descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (ext_func->vkCreatePipelineLayout(pContext->logical_device, &pipeline_layout_info, NULL, &pContext->pipeline_layout) != VK_SUCCESS) {
        ext_func->vkDestroyShaderModule(pContext->logical_device, shader_module, NULL);
        return SSP_ERROR_CODE_VULKAN_CREATE_GRAPHIC_PIPELINE;
    }

    VkPipelineRenderingCreateInfo pipeline_rendering_info = {0};
    pipeline_rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipeline_rendering_info.colorAttachmentCount = 1;
    pipeline_rendering_info.pColorAttachmentFormats = &pContext->swapchain_image_format;

    VkGraphicsPipelineCreateInfo graphic_pipeline_info = {0};
    graphic_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphic_pipeline_info.pNext = &pipeline_rendering_info;
    graphic_pipeline_info.stageCount = 2;
    graphic_pipeline_info.pStages = shader_stages;
    graphic_pipeline_info.pVertexInputState = &vertex_input_info;
    graphic_pipeline_info.pInputAssemblyState = &input_assembly_info;
    graphic_pipeline_info.pViewportState = &viewport_state_info;
    graphic_pipeline_info.pRasterizationState = &rasterization_info;
    graphic_pipeline_info.pMultisampleState = &multisample_info;
    graphic_pipeline_info.pColorBlendState = &color_blend_info;
    graphic_pipeline_info.pDynamicState = &dynamic_state_info;
    graphic_pipeline_info.layout = pContext->pipeline_layout;
    graphic_pipeline_info.renderPass = NULL;
    graphic_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    graphic_pipeline_info.basePipelineIndex = -1;

    VkResult result = ext_func->vkCreateGraphicsPipelines(pContext->logical_device, NULL, 1, &graphic_pipeline_info, NULL, &pContext->graphic_pipeline);

    free(shader_code);
    ext_func->vkDestroyShaderModule(pContext->logical_device, shader_module, NULL);

    if (result != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_CREATE_GRAPHIC_PIPELINE;
    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_pool(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkDescriptorPoolSize size = {0};
    size.descriptorCount = SSP_MAX_FRAMES_IN_FLIGHT;
    size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorPoolCreateInfo descriptor_info = {0};
    descriptor_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptor_info.maxSets = SSP_MAX_FRAMES_IN_FLIGHT;
    descriptor_info.poolSizeCount = 1;
    descriptor_info.pPoolSizes = &size;
    
    if (ext_func->vkCreateDescriptorPool(pContext->logical_device, &descriptor_info, NULL, &pContext->descriptor_pool) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_CREATE_DESCRIPTOR_POOL;
    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_sets(struct SSPVulkanContext *pContext)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    VkDescriptorSetLayout *layouts = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSetLayout));
    pContext->descriptor_sets = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSet));

    for (size_t i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
        layouts[i] = pContext->descriptor_set_layout;

    VkDescriptorSetAllocateInfo descriptor_set_info = {0};
    descriptor_set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_info.descriptorPool = pContext->descriptor_pool;
    descriptor_set_info.descriptorSetCount = SSP_MAX_FRAMES_IN_FLIGHT;
    descriptor_set_info.pSetLayouts = layouts;

    if (ext_func->vkAllocateDescriptorSets(pContext->logical_device, &descriptor_set_info, pContext->descriptor_sets) != VK_SUCCESS) {
        free(layouts);
        return false;
    }

    VkDescriptorBufferInfo buffer_info = {0};
    buffer_info.range = sizeof(struct SSPShaderUniformBuffer);

    VkWriteDescriptorSet write_descriptor = {0};
    write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor.dstBinding = 0;
    write_descriptor.dstArrayElement = 0;
    write_descriptor.descriptorCount = 1;
    write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor.pBufferInfo = &buffer_info;
    
    for (size_t i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i) {
        buffer_info.buffer = pContext->uniform_buffers[i];
        write_descriptor.dstSet = pContext->descriptor_sets[i];

        ext_func->vkUpdateDescriptorSets(pContext->logical_device, 1, &write_descriptor, 0, NULL);
    }

    free(layouts);
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_renderer_stop(struct SSPRenderer *pRenderer)
{
    vkDeviceWaitIdle(pRenderer->vulkan_context.logical_device);
}

enum SSP_ERROR_CODE ssp_renderer_create(struct SSPRenderer *pRenderer, struct SSPWindow *window, struct SSPConfig *config)
{
    struct SSPVulkanContext *vulkan_context = &pRenderer->vulkan_context;
    const char *app_name = (config && config->app_name) ? config->app_name : NULL;
    const struct SSPVersion app_version = (config) ? config->app_version : SSP_VERSION_DEFAULT;

    enum SSP_ERROR_CODE err_code = SSP_ERROR_CODE_SUCCESS;

    (err_code = ssp_vulkan_ext_func_instance_global(vulkan_context))
        || (err_code = ssp_vulkan_create_instance(vulkan_context, app_name, app_version))
        || (err_code = ssp_vulkan_ext_func_instance_nglobal(vulkan_context))
        #ifdef DEBUG
        || (err_code = ssp_vulkan_setup_debug_messenger(vulkan_context))
        #endif
        || (err_code = ssp_vulkan_create_surface(vulkan_context, window))
        || (err_code = ssp_vulkan_init_physical_device(vulkan_context))
        || (err_code = ssp_vulkan_create_logical_device(vulkan_context, window))
        || (err_code = ssp_vulkan_create_swapchain(vulkan_context, window))
        || (err_code = ssp_vulkan_create_descriptor_set_layout(vulkan_context))
        || (err_code = ssp_vulkan_create_graphic_pipeline(vulkan_context))
        || (err_code = ssp_vulkan_create_command_pools(vulkan_context))
        || (err_code = ssp_vulkan_create_uniform_buffers(vulkan_context))
        || (err_code = ssp_vulkan_create_image_view(vulkan_context))
        || (err_code = ssp_vulkan_create_descriptor_pool(vulkan_context))
        || (err_code = ssp_vulkan_create_descriptor_sets(vulkan_context))
        || (err_code = ssp_vulkan_create_sync_objects(vulkan_context))
        || (err_code = ssp_vulkan_allocate_command_buffers(vulkan_context));

    pRenderer->objects_to_draw = ssp_dynamic_array_init(SSP_DEFAULT_OBJECTS_POOLS_SIZE);

    return err_code;
}

void ssp_renderer_destroy(struct SSPRenderer *pRenderer)
{
    struct SSPVulkanContext *context = &pRenderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;

    ssp_dynamic_array_free(pRenderer->objects_to_draw);

    if (context->logical_device) {
        ext_func->vkDeviceWaitIdle(context->logical_device);

        ext_func->vkDestroyDescriptorPool(context->logical_device, context->descriptor_pool, NULL);
        ext_func->vkDestroyDescriptorSetLayout(context->logical_device, context->descriptor_set_layout, NULL);
        ext_func->vkDestroyPipeline(context->logical_device, context->graphic_pipeline, NULL);
        ext_func->vkDestroyPipelineLayout(context->logical_device, context->pipeline_layout, NULL);

        if (context->uniform_buffers_mapped) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkUnmapMemory(context->logical_device, context->uniform_buffers_memory[i]);
            free(context->uniform_buffers_mapped);
        }

        if (context->uniform_buffers_memory) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkFreeMemory(context->logical_device, context->uniform_buffers_memory[i], NULL);
            free(context->uniform_buffers_memory);
        }

        if (context->uniform_buffers) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkDestroyBuffer(context->logical_device, context->uniform_buffers[i], NULL);
            free(context->uniform_buffers);
        }

        if(context->frames_in_flight_fences) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkDestroyFence(context->logical_device, context->frames_in_flight_fences[i], NULL);
            free(context->frames_in_flight_fences);
        }

        if (context->present_complete_semaphores) {
            for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
                ext_func->vkDestroySemaphore(context->logical_device, context->present_complete_semaphores[i], NULL);
            free(context->present_complete_semaphores);
        }

        if (context->render_finished_semaphores) {
            for (int i = 0; i < context->swapchain_images_count; ++i)
                ext_func->vkDestroySemaphore(context->logical_device, context->render_finished_semaphores[i], NULL);
            free(context->render_finished_semaphores);
        }

        ssp_renderer_swapchain_destroy(context);

        if (context->command_pools) {
            ext_func->vkFreeCommandBuffers(context->logical_device, context->command_pools[context->transfer_command_pool_index], SSP_TRANSFER_COMMAND_BUFFERS_COUNT, context->transfer_command_buffers);
            free(context->transfer_command_buffers);
            ext_func->vkFreeCommandBuffers(context->logical_device, context->command_pools[context->graphic_command_pool_index], SSP_MAX_FRAMES_IN_FLIGHT, context->graphic_command_buffers);
            free(context->graphic_command_buffers);
            for (int i = 0; i < context->command_pools_size; ++i)
                ext_func->vkDestroyCommandPool(context->logical_device, context->command_pools[i], NULL);
            free(context->command_pools);
        }
        ext_func->vkDestroyDevice(context->logical_device, NULL);
    }
    if (context->instance) {
        if (context->surface)
            ext_func->vkDestroySurfaceKHR(context->instance, context->surface, NULL);
        #ifdef DEBUG
        ext_func->vkDestroyDebugUtilsMessengerEXT(context->instance, context->debug_messenger, NULL);
        #endif
    }
    ext_func->vkDestroyInstance(context->instance, NULL);
    free(pRenderer);
}

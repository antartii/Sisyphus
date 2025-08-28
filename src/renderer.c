#include "renderer.h"

static const char *ssp_vulkan_layer_names[] = {
    #ifdef DEBUG
    "VK_LAYER_KHRONOS_validation",
    #endif
};
static const size_t ssp_vulkan_layer_names_size = ARRAY_SIZE(ssp_vulkan_layer_names);
static const char *ssp_vulkan_extensions_names[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    #ifdef DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    #endif
};
static const size_t ssp_vulkan_extensions_names_size = ARRAY_SIZE(ssp_vulkan_extensions_names);

static bool ssp_vulkan_check_layer_names(struct SSPVulkanContextExtFunc *ext_func)
{
    uint32_t layers_properties_count;
    ext_func->vkEnumerateInstanceLayerProperties(&layers_properties_count, NULL);
    VkLayerProperties *layers_properties = malloc(sizeof(VkLayerProperties) * layers_properties_count);
    ext_func->vkEnumerateInstanceLayerProperties(&layers_properties_count, layers_properties);

    for (size_t i = 0; i < ssp_vulkan_layer_names_size; ++i) {
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
    
    for (size_t i = 0; i < ssp_vulkan_extensions_names_size; ++i) {
        bool is_included = false;

        for (uint32_t j = 0; j < extensions_properties_count; ++j) {
            if (strcmp(extensions_properties[j].extensionName, ssp_vulkan_extensions_names[i])) {
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

static enum SSP_ERROR_CODE ssp_vulkan_create_instance(struct SSPVulkanContext *pContext, const char *app_name, const struct Version app_version)
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
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = VK_MAKE_VERSION(app_version.major, app_version.minor, app_version.patch);
    app_info.apiVersion = SSP_VULKAN_API_VERSION;

    VkInstanceCreateInfo instance_create_info = {0};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledLayerCount = ssp_vulkan_layer_names_size;
    instance_create_info.ppEnabledLayerNames = ssp_vulkan_layer_names;
    instance_create_info.enabledExtensionCount = ssp_vulkan_extensions_names_size;
    instance_create_info.ppEnabledExtensionNames = ssp_vulkan_extensions_names;

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
    VkInstance instance = pContext->instance;

    ext_func->vkDestroyInstance = (PFN_vkDestroyInstance) vkGetInstanceProcAddr(pContext->instance, "vkDestroyInstance");
    #ifdef DEBUG
    ext_func->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(pContext->instance, "vkCreateDebugUtilsMessengerEXT");
    ext_func->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(pContext->instance, "vkDestroyDebugUtilsMessengerEXT");
    #endif
    ext_func->vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) vkGetInstanceProcAddr(pContext->instance, "vkEnumeratePhysicalDevices");
    ext_func->vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties) vkGetInstanceProcAddr(pContext->instance, "vkGetPhysicalDeviceProperties");

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

enum SSP_ERROR_CODE ssp_renderer_create(struct SSPRenderer *pRenderer, const char *app_name, const struct Version app_version)
{
    struct SSPVulkanContext *vulkan_context = &pRenderer->vulkan_context;

    enum SSP_ERROR_CODE err_code = SSP_ERROR_CODE_SUCCESS;

    (err_code = ssp_vulkan_ext_func_instance_global(vulkan_context))
        || (err_code = ssp_vulkan_create_instance(vulkan_context, app_name, app_version))
        || (err_code = ssp_vulkan_ext_func_instance_nglobal(vulkan_context))
        #ifdef DEBUG
        || (err_code = ssp_vulkan_setup_debug_messenger(vulkan_context))
        #endif
        || (err_code = ssp_vulkan_init_physical_device(vulkan_context));

    return err_code;
}

void ssp_renderer_destroy(struct SSPRenderer *pRenderer)
{
    struct SSPVulkanContext *context = &pRenderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;

    #ifdef DEBUG
    ext_func->vkDestroyDebugUtilsMessengerEXT(context->instance, context->debug_messenger, NULL);
    #endif
    ext_func->vkDestroyInstance(context->instance, NULL);
    free(pRenderer);
}

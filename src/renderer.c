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

static bool ssp_vulkan_check_layer_names()
{
    uint32_t layers_properties_count;
    vkEnumerateInstanceLayerProperties(&layers_properties_count, NULL);
    VkLayerProperties *layers_properties = malloc(sizeof(VkLayerProperties) * layers_properties_count);
    vkEnumerateInstanceLayerProperties(&layers_properties_count, layers_properties);

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

static bool ssp_vulkan_check_extensions_names()
{
    uint32_t extensions_properties_count;
    vkEnumerateInstanceExtensionProperties(NULL, &extensions_properties_count, NULL);
    VkExtensionProperties *extensions_properties = malloc(sizeof(VkExtensionProperties) * extensions_properties_count);
    vkEnumerateInstanceExtensionProperties(NULL, &extensions_properties_count, extensions_properties);
    
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

static enum SSP_ERROR_CODE ssp_vulkan_create_instance(struct SSPRenderer *pRenderer, const char *app_name, const struct Version app_version)
{
    struct SSPVulkanContext *context = &pRenderer->vulkan_context;

    uint32_t instance_version;
    vkEnumerateInstanceVersion(&instance_version);
    if (instance_version < SSP_VULKAN_API_VERSION)
        return SSP_ERROR_CODE_VULKAN_LOADER_SUPPORT_INSTANCE_VERSION;
    if (!ssp_vulkan_check_layer_names())
        return SSP_ERROR_CODE_VULKAN_INSTANCE_LAYERS;
    if (!ssp_vulkan_check_extensions_names())
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

    if (vkCreateInstance(&instance_create_info, NULL, &(context->instance)) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_INSTANCE_CREATE;

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_renderer_create(struct SSPRenderer *pRenderer, const char *app_name, const struct Version app_version)
{
    return ssp_vulkan_create_instance(pRenderer, app_name, app_version);
}

void ssp_renderer_destroy(struct SSPRenderer *pRenderer)
{
    struct SSPVulkanContext *context = &pRenderer->vulkan_context;

    vkDestroyInstance(pRenderer->vulkan_context.instance, NULL);
    free(pRenderer);
}

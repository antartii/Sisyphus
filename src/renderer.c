#include "renderer.h"
#include "object.h"
#include "camera.h"

enum SSP_ERROR_CODE ssp_renderer_destroy(struct SSPRenderer *pRenderer)
{
    struct SSPVulkanContext *context = &pRenderer->vulkan_context;
    
    ssp_dynamic_array_free(pRenderer->objects_to_draw);
    return ssp_vulkan_context_destroy(&pRenderer->vulkan_context);
}

enum SSP_ERROR_CODE ssp_renderer_stop(struct SSPRenderer *pRenderer)
{
    vkDeviceWaitIdle(pRenderer->vulkan_context.device.logical_device);
}

enum SSP_ERROR_CODE ssp_renderer_create(struct SSPRenderer *pRenderer, struct SSPWindow *window, struct SSPConfig *config)
{
    const char *app_name = (config && config->app_name) ? config->app_name : NULL;
    const struct SSPVersion app_version = (config) ? config->app_version : SSP_VERSION_DEFAULT;

    ssp_vulkan_context_init(&pRenderer->vulkan_context, window, app_name, app_version);

    pRenderer->objects_to_draw = ssp_dynamic_array_init(SSP_DEFAULT_OBJECTS_POOLS_SIZE, sizeof(struct SSPObject *), true);
}

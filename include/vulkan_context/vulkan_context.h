#pragma once

#include <vulkan/vulkan.h>
#include <cglm/cglm.h>
#include <inttypes.h>

#include "utils.h"
#include "errors.h"
#include "shaders.h"
#include "sisyphus_config.h"
#include "window.h"
#include "ext_func.h"
#include "buffer.h"
#include "instance.h"
#include "debug.h"
#include "surface.h"
#include "device.h"
#include "device.h"

#include "swapchain.h"
#include "graphic.h"
#include "command_buffer.h"
#include "sync_object.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPVulkanContext {
    VkInstance instance;
    VkSurfaceKHR surface;

    struct SSPVulkanDevice device;
    struct SSPVulkanPipelineContext pipeline_context;
    struct SSPVulkanSwapchain swapchain;
    struct SSPVulkanCommandContext command_context;

    struct SSPCamera *cameraData; // need to see for removing

    #ifdef DEBUG
    VkDebugUtilsMessengerEXT debug_messenger;
    #endif

    struct SSPVulkanContextExtFunc ext_func;
};

struct SSPCamera;

enum SSP_ERROR_CODE ssp_vulkan_context_destroy(struct SSPVulkanContext *pContext);
enum SSP_ERROR_CODE ssp_vulkan_draw_frame(struct SSPVulkanContext *pContext, struct SSPWindow *window, struct SSPDynamicArray *objects_to_draw);
enum SSP_ERROR_CODE ssp_vulkan_context_init(struct SSPVulkanContext *pContext, struct SSPWindow *window, const char *app_name, struct SSPVersion app_version);

#ifdef __cplusplus
    }
#endif

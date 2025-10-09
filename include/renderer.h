#ifndef _SISYPHUS_RENDERER_H
#define _SISYPHUS_RENDERER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <vulkan/vulkan.h>
#include <unistd.h>

#include "shaders.h"
#include "sisyphus_config.h"
#include "utils.h"
#include "errors.h"
#include <string.h>
#include "config.h"
#include "window.h"
#include "vulkan_context/vulkan_context.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSP_DEFAULT_OBJECTS_POOLS_SIZE 50

struct SSPRenderer {
    struct SSPDynamicArray *objects_to_draw;

    struct SSPVulkanContext vulkan_context;
};

enum SSP_ERROR_CODE ssp_renderer_create(struct SSPRenderer *pRenderer, struct SSPWindow *window, struct SSPConfig *config);
enum SSP_ERROR_CODE ssp_renderer_destroy(struct SSPRenderer *pRenderer);
enum SSP_ERROR_CODE ssp_renderer_stop(struct SSPRenderer *pRenderer);

#ifdef __cplusplus
    }
#endif

#endif

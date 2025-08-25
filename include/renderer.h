#ifndef _SISYPHUS_RENDERER_H
#define _SISYPHUS_RENDERER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "sisyphus_config.h"
#include "utils.h"
#include "errors.h"

#define SSP_VULKAN_API_VERSION VK_API_VERSION_1_0

typedef void (*SSPRendererErrorHandlerCallback)(void *, int, enum SSP_ERROR_SEVERITY);

struct SSPRendererErrorHandler {
    bool destroy;
    SSPRendererErrorHandlerCallback callback;
    enum SSP_ERROR_SEVERITY min_severity;
    void *data;
};

struct SSPRenderer {
    struct SSPRendererErrorHandler error_handler;
};

struct SSPRenderer *SSPRenderer_create(void);
void SSPRenderer_destroy(struct SSPRenderer *renderer);

#endif

#pragma once

#include "renderer.h"
#include "window.h"
#include "errors.h"
#include "surfaces/surfaces.h"
#include "camera.h"
#include "vulkan_context/vulkan_context.h"
#include "utils.h"

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SSPEngineErrorHandlerCallback)(void *, int, enum SSP_ERROR_SEVERITY);

struct SSPEngineErrorHandler {
    SSPEngineErrorHandlerCallback callback;
    enum SSP_ERROR_SEVERITY min_severity;
    void *data;
};

struct SSPEngine {
    struct SSPWindow *window;
    struct SSPRenderer *renderer;

    struct SSPEngineErrorHandler error_handler;
    struct SSPCamera *camera;
    bool external_camera;
};

SSP_API enum SSP_ERROR_CODE ssp_engine_create(struct SSPConfig *config, struct SSPEngine *pEngine);
SSP_API void ssp_engine_destroy(struct SSPEngine *pEngine);
SSP_API bool ssp_engine_run(struct SSPEngine *pEngine);
SSP_API void ssp_engine_error(struct SSPEngine *pEngine, enum SSP_ERROR_CODE err_code);

struct SSPObject;
SSP_API void ssp_engine_draw(struct SSPEngine *pEngine, struct SSPObject *object);

#ifdef __cplusplus
    }
#endif

#ifndef _SISYPHUS_ENGINE_H
#define _SISYPHUS_ENGINE_H

#include "renderer.h"
#include "window.h"
#include "errors.h"

#include <stdlib.h>
#include <stdbool.h>

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
};

struct SSPEngine *ssp_engine_create(const char *app_name, const struct Version app_version);
void ssp_engine_destroy(struct SSPEngine *pEngine);

#endif

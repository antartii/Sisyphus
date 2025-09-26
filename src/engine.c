#include "engine.h"
#include "object.h"

static void ssp_engine_error(struct SSPEngine *pEngine, enum SSP_ERROR_CODE err_code)
{
    struct SSPEngineErrorHandler *error_handler = &pEngine->error_handler;
    enum SSP_ERROR_SEVERITY err_severity = ssp_get_error_severity(err_code);

    if (error_handler->callback)
        error_handler->callback(error_handler->data, err_code, err_severity);    

    #ifdef DEBUG
        printf("Sisyphus error %d: %s\n", err_code, SSP_ERROR_STR[err_code]);
    #endif

    if (err_severity >= SSP_ERROR_SEVERITY_CRITICAL)
        ssp_engine_destroy(pEngine);
}

bool ssp_engine_run(struct SSPEngine *pEngine)
{
    if (!pEngine->window->should_close
        && ssp_window_run(pEngine->window)
        && (ssp_renderer_draw_frame(pEngine->renderer, pEngine->window) == SSP_ERROR_CODE_SUCCESS)) {
            return true;
    }

    ssp_renderer_stop(pEngine->renderer);

    return false;
}

struct SSPEngine *ssp_engine_create(struct SSPConfig *config, enum SSP_ERROR_CODE *error_code)
{
    struct SSPEngine *pEngine = calloc(1, sizeof(struct SSPEngine));

    pEngine->renderer = calloc(1, sizeof(struct SSPRenderer));
    pEngine->window = calloc(1, sizeof(struct SSPWindow));

    pEngine->window->width = SSP_WINDOW_DEFAULT_WIDTH;
    pEngine->window->height = SSP_WINDOW_DEFAULT_HEIGHT;

    enum SSP_ERROR_CODE err_code = SSP_ERROR_CODE_SUCCESS;

    (err_code = ssp_window_create(pEngine->window, config))
    || (err_code = ssp_renderer_create(pEngine->renderer, pEngine->window, config));

    pEngine->camera = ssp_camera_create(pEngine);
    ssp_camera_update(pEngine, pEngine->camera);

    if (err_code != SSP_ERROR_CODE_SUCCESS) {
        ssp_engine_error(pEngine, err_code);
        return NULL;
    }

    return pEngine;
}

void ssp_engine_draw(struct SSPEngine *pEngine, struct SSPObject *object)
{
    ssp_dynamic_array_push(pEngine->renderer->objects_to_draw, object);
}

void ssp_engine_destroy(struct SSPEngine *pEngine)
{
    if (pEngine == NULL)
        return;

    ssp_renderer_destroy(pEngine->renderer);
    ssp_window_destroy(pEngine->window);
    ssp_camera_destroy(pEngine, pEngine->camera);

    free(pEngine->window);
    free(pEngine->renderer);
    free(pEngine);
}

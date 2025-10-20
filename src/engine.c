#include "engine.h"
#include "object.h"

void ssp_engine_error(struct SSPEngine *pEngine, enum SSP_ERROR_CODE err_code)
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
        && (ssp_vulkan_draw_frame(&pEngine->renderer->vulkan_context, pEngine->window, pEngine->renderer->objects_to_draw) == SSP_ERROR_CODE_SUCCESS)) {
            return true;
    }
    ssp_renderer_stop(pEngine->renderer);

    return false;
}

enum SSP_ERROR_CODE ssp_engine_create(struct SSPConfig *config, struct SSPEngine *pEngine)
{
    pEngine->renderer = calloc(1, sizeof(struct SSPRenderer));
    pEngine->window = calloc(1, sizeof(struct SSPWindow));

    pEngine->window->width = SSP_WINDOW_DEFAULT_WIDTH;
    pEngine->window->height = SSP_WINDOW_DEFAULT_HEIGHT;

    enum SSP_ERROR_CODE err_code = SSP_ERROR_CODE_SUCCESS;

    (err_code = ssp_window_create(pEngine->window, config))
    || (err_code = ssp_renderer_create(pEngine->renderer, pEngine->window, config));

    struct SSPCamera camera = {0};
    ssp_camera_create(&camera);
    ssp_camera_update(pEngine, &camera);

    if (err_code != SSP_ERROR_CODE_SUCCESS)
        ssp_engine_error(pEngine, err_code);

    return err_code;
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

    free(pEngine->window);
    free(pEngine->renderer);
}

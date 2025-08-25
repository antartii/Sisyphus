#include "engine.h"

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

static enum SSP_ERROR_CODE ssp_engine_init(struct SSPEngine *pEngine, const char *app_name, const struct Version app_version)
{
    pEngine->renderer = calloc(1, sizeof(struct SSPRenderer));
    pEngine->window = calloc(1, sizeof(struct SSPWindow));

    enum SSP_ERROR_CODE err_code = SSP_ERROR_CODE_SUCCESS;

    (err_code = ssp_window_create(pEngine->window))
    || (err_code = ssp_renderer_create(pEngine->renderer, app_name, app_version));

    if (err_code != SSP_ERROR_CODE_SUCCESS)
        ssp_engine_error(pEngine, err_code);

    return err_code;
}

struct SSPEngine *ssp_engine_create(const char *app_name, const struct Version app_version)
{
    struct SSPEngine *pEngine = calloc(1, sizeof(struct SSPEngine));

    if (ssp_engine_init(pEngine, app_name, app_version) != SSP_ERROR_CODE_SUCCESS)
        return NULL;
    return pEngine;
}

void ssp_engine_destroy(struct SSPEngine *pEngine)
{
    if (pEngine == NULL)
        return;

    ssp_window_destroy(pEngine->window);
    ssp_renderer_destroy(pEngine->renderer);

    free(pEngine);
}

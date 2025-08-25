#include "renderer.h"

static void SSPRenderer_error(struct SSPRenderer *renderer, enum SSP_ERROR_CODE err_code)
{
    struct SSPRendererErrorHandler *error_handler = &renderer->error_handler;
    enum SSP_ERROR_SEVERITY err_severity = SSPget_error_severity(err_code);

    if (error_handler->callback)
        error_handler->callback(error_handler->data, err_code, err_severity);    

    #ifdef DEBUG
        printf("Renderer error %d: %s\n", err_code, SSP_ERROR_STR[err_code]);
    #endif

    if (error_handler->destroy)
        SSPRenderer_destroy(renderer);
}

static void SSPRenderer_default(struct SSPRenderer *renderer)
{
    struct SSPRendererErrorHandler *error_handler = &renderer->error_handler;

    error_handler->destroy = true;
}

struct SSPRenderer *SSPRenderer_create(void)
{
    struct SSPRenderer *renderer = calloc(1, sizeof(struct SSPRenderer));
    SSPRenderer_default(renderer);

    return renderer;
}

void SSPRenderer_destroy(struct SSPRenderer *renderer)
{
    free(renderer);
}

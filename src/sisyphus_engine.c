#include "sisyphus_engine.h"

struct SSPEngine *SSPEngine_create(void)
{
    struct SSPEngine *engine = calloc(1, sizeof(struct SSPEngine));
    engine->window = SSPWindow_create();
    engine->renderer = SSPRenderer_create();

    return engine;
}

void SSPEngine_destroy(struct SSPEngine *engine)
{
    SSPWindow_destroy(engine->window);
    SSPRenderer_destroy(engine->renderer);
    free(engine);
}

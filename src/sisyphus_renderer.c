#include "sisyphus_renderer.h"

struct SSPRenderer *SSPRenderer_create(void)
{
    struct SSPRenderer *renderer = calloc(1, sizeof(struct SSPRenderer));    

    return renderer;
}

void SSPRenderer_destroy(struct SSPRenderer *renderer)
{
    free(renderer);
}

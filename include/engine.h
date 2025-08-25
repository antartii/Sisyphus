#ifndef _SISYPHUS_ENGINE_H
#define _SISYPHUS_ENGINE_H

#include "renderer.h"
#include "window.h"

#include <stdlib.h>

struct SSPEngine {
    struct SSPWindow *window;
    struct SSPRenderer *renderer;
};

struct SSPEngine *SSPEngine_create(void);
void SSPEngine_destroy(struct SSPEngine *engine);

#endif

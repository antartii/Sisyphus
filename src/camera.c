#include "camera.h"
#include "engine.h"

struct SSPCamera *ssp_camera_create(struct SSPEngine *pEngine)
{
    struct SSPCamera *camera = calloc(1, sizeof(struct SSPCamera));

    return camera;
}

void ssp_camera_destroy(struct SSPEngine *pEngine, struct SSPCamera *camera)
{
    free(camera);
}

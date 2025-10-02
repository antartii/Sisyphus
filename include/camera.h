#ifndef _SISYPHUS_CAMERA_H
#define _SISYPHUS_CAMERA_H

#include <vulkan/vulkan.h>
#include <cglm/cglm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SSP_CAMERA_DEPTH_MIN_RENDER_DEFAULT 0.0f
#define SSP_CAMERA_DEPTH_MAX_RENDER_DEFAULT 10.0f
#define SSP_CAMERA_FOV_DEFAULT_DEGREES glm_rad(45.0f)

struct SSPEngine;

struct SSPCamera {
    vec3 pos;
    vec3 target;
    vec3 up;

    vec2 render_depth_range;
    float fov_in_radians;
};

struct SSPCamera *ssp_camera_create();
void ssp_camera_destroy(struct SSPCamera *camera);
void ssp_camera_update(struct SSPEngine *pEngine, struct SSPCamera *camera);
void ssp_camera_init(struct SSPCamera *camera);

#ifdef __cplusplus
    }
#endif

#endif
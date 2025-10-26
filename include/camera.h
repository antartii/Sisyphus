#pragma once

#include <vulkan/vulkan.h>
#include <cglm/cglm.h>

#include "export.h"

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

SSP_API enum SSP_ERROR_CODE ssp_camera_create(struct SSPCamera *camera);
SSP_API void ssp_camera_destroy(struct SSPCamera *camera);
SSP_API void ssp_camera_update(struct SSPEngine *pEngine, struct SSPCamera *camera);

#ifdef __cplusplus
    }
#endif
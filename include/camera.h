#ifndef _SISYPHUS_CAMERA_H
#define _SISYPHUS_CAMERA_H

#include <vulkan/vulkan.h>
#include <cglm/cglm.h>

struct SSPEngine;

struct SSPCamera {
    vec3 pos;
    vec3 target;
    vec3 up;

    vec2 render_depth_range;
    float fov_in_radians;

    VkBuffer *uniform_buffers;
    VkDeviceMemory *uniform_buffers_memory;
    void **uniform_buffers_mapped;
};

struct SSPCamera *ssp_camera_create(struct SSPEngine *pEngine);
void ssp_camera_destroy(struct SSPEngine *pEngine, struct SSPCamera *camera);

#endif
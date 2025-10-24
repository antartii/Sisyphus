#ifndef _SISYPHUS_SHADERS_H
#define _SISYPHUS_SHADERS_H

#include <stdalign.h>
#include <cglm/cglm.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SSPShaderUniformBuffer {
    alignas(16) mat4 view;
    alignas(16) mat4 proj;
};

struct SSPShaderPushConstant {
    alignas(16) mat4 model;
    bool isTextured;
    int textureIndex;
};

struct SSPShaderVertex {
    vec2 pos;
    vec3 color;
};

#ifdef __cplusplus
    }
#endif

#endif

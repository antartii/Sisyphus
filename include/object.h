#ifndef _SISYPHUS_OBJECT_H
#define _SISYPHUS_OBJECT_H

#include <vulkan/vulkan.h>
#include "shaders.h"
#include "engine.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPObject {
    uint32_t indices_count;

    struct SSPShaderPushConstant vertex_push_constant;

    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_memory;
};

struct SSPObject *spp_object_create(struct SSPEngine *engine, vec2 *vertices_pos, vec3 color, uint16_t *indices, uint32_t vertices_count);
void ssp_object_destroy(struct SSPEngine *engine, struct SSPObject *object);

struct SSPObject *ssp_object_create_rectangle(struct SSPEngine *engine, vec2 pos, vec2 size, vec3 color);

#ifdef __cplusplus
    }
#endif

#endif

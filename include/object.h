#ifndef _SISYPHUS_OBJECT_H
#define _SISYPHUS_OBJECT_H

#include <vulkan/vulkan.h>

#include "shaders.h"
#include "engine.h"
#include "vulkan_context/vulkan_context.h"

#define STB_IMAGE_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_OBJECT_STATUS {
    SSP_OBJECT_NOT_INITIALISED = 0,
    SSP_OBJECT_UPLOADING,
    SSP_OBJECT_READY
};

struct SSPObject {
    uint32_t indices_count;

    struct SSPShaderPushConstant vertex_push_constant;

    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_memory;

    enum SSP_OBJECT_STATUS status;
};

struct SSPObject *spp_object_create(struct SSPEngine *engine, vec2 *vertices_pos, vec3 color, uint16_t *indices, uint32_t vertices_count);
void ssp_object_destroy(struct SSPEngine *engine, struct SSPObject *object);

struct SSPObject *ssp_object_create_rectangle(struct SSPEngine *engine, vec2 pos, vec2 size, vec3 color);

#ifdef __cplusplus
    }
#endif

#endif

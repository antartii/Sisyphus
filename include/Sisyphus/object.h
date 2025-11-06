#pragma once

#include <vulkan/vulkan.h>

#include "shaders.h"
#include "engine.h"
#include "vulkan_context/vulkan_context.h"
#include "texture.h"
#include "export.h"

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
    uint32_t vertices_count;
    struct SSPShaderVertex *vertices;

    struct SSPShaderPushConstant push_constant;

    struct SSPVulkanBuffer vertex_buffer;
    struct SSPVulkanBuffer index_buffer;

    enum SSP_OBJECT_STATUS status;

    bool is_textured;
    struct SSPTexture *texture;
};

SSP_API enum SSP_ERROR_CODE ssp_object_update_texture_coordinates(struct SSPObject *object, struct SSPEngine *engine, vec2 *texture_coordinates);
SSP_API enum SSP_ERROR_CODE spp_object_create(struct SSPObject *object, struct SSPEngine *engine, vec2 *vertices_pos, vec3 color, uint16_t *indices, uint32_t vertices_count);
SSP_API void ssp_object_destroy(struct SSPEngine *engine, struct SSPObject *object);
SSP_API enum SSP_ERROR_CODE ssp_object_apply_texture(struct SSPObject *object, struct SSPTexture *texture);
SSP_API enum SSP_ERROR_CODE ssp_object_create_rectangle(struct SSPObject *object, struct SSPEngine *engine, vec2 pos, vec2 size, vec3 color);

#ifdef __cplusplus
    }
#endif

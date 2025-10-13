#include "object.h"

struct SSPObject *spp_object_create(struct SSPEngine *engine, vec2 *vertices_pos, vec3 color, uint16_t *indices, uint32_t vertices_count)
{
    struct SSPObject *object = calloc(1, sizeof(struct SSPObject));
    struct SSPShaderVertex *vertices = malloc(sizeof(struct SSPShaderVertex) * vertices_count);

    object->indices_count = (vertices_count - 2) * 3;
    glm_mat4_identity(object->vertex_push_constant.model);

    for (uint32_t i = 0; i < vertices_count; ++i) {
        glm_vec3(color, vertices[i].color);
        glm_vec2(vertices_pos[i], vertices[i].pos);
    }

    object->status = SSP_OBJECT_NOT_INITIALISED;

    struct SSPVulkanContext *context = &engine->renderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;

    if (ssp_vulkan_create_vertex_buffer(ext_func, &context->device, &context->command_context, &object->vertex_buffer, &object->vertex_memory, vertices, vertices_count) != SSP_ERROR_CODE_SUCCESS
        || !ssp_vulkan_create_index_buffer(ext_func, &context->device, &context->command_context, &object->index_buffer, &object->index_memory, indices, object->indices_count) != SSP_ERROR_CODE_SUCCESS) {
        free(vertices);
        free(object);
        return NULL;
    }
    free(vertices);

    return object;
}

void ssp_object_destroy(struct SSPEngine *engine, struct SSPObject *object)
{
    struct SSPVulkanContext *context = &engine->renderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;
    VkDevice logical_device = context->device.logical_device;

    ext_func->vkDestroyBuffer(logical_device, object->vertex_buffer, NULL);
    ext_func->vkFreeMemory(logical_device, object->vertex_memory, NULL);

    ext_func->vkDestroyBuffer(logical_device, object->index_buffer, NULL);
    ext_func->vkFreeMemory(logical_device, object->index_memory, NULL);

    free(object);
}

struct SSPObject *ssp_object_create_rectangle(struct SSPEngine *engine, vec2 pos, vec2 size, vec3 color)
{
    uint16_t indices[] = {
        0, 1, 2, 2, 3, 0
    };

    vec2 opposite_vertice;
    glm_vec2_add(pos, size, opposite_vertice);

    mat4x2 vertices = {
        {pos[0], pos[1]},
        {opposite_vertice[0], pos[1]},
        {opposite_vertice[0], opposite_vertice[1]},
        {pos[0], opposite_vertice[1]}
    };

    return spp_object_create(engine, vertices, color, indices, 4);
}

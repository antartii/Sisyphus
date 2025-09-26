#include "object.h"

static enum SSP_ERROR_CODE ssp_vulkan_create_vertex_buffer(struct SSPVulkanContext *pContext, struct SSPObject *object, struct SSPShaderVertex *vertices, uint32_t vertices_count)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;
    VkDeviceSize size = sizeof(struct SSPShaderVertex) * vertices_count;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    ssp_vulkan_create_buffer(pContext, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_memory);

    void *data_staging;
    ext_func->vkMapMemory(pContext->logical_device, staging_memory, 0, size, 0, &data_staging);
    memcpy(data_staging, vertices, size);
    ext_func->vkUnmapMemory(pContext->logical_device, staging_memory);

    ssp_vulkan_create_buffer(pContext, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &object->vertex_buffer, &object->vertex_memory);
    ssp_vulkan_copy_buffer(pContext, &staging_buffer, &object->vertex_buffer, size);
    ext_func->vkDestroyBuffer(pContext->logical_device, staging_buffer, NULL);
    ext_func->vkFreeMemory(pContext->logical_device, staging_memory, NULL);

    return SSP_ERROR_CODE_SUCCESS;
}

static enum SSP_ERROR_CODE ssp_vulkan_create_index_buffer(struct SSPVulkanContext *pContext, struct SSPObject *object, uint16_t *indices, uint32_t indices_count)
{
    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkDeviceSize size = sizeof(uint16_t) * indices_count;
    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    ssp_vulkan_create_buffer(pContext, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_memory);
    
    void *data_staging;
    ext_func->vkMapMemory(pContext->logical_device, staging_memory, 0, size, 0, &data_staging);
    memcpy(data_staging, indices, size);
    ext_func->vkUnmapMemory(pContext->logical_device, staging_memory);

    ssp_vulkan_create_buffer(pContext, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &object->index_buffer, &object->index_memory);
    ssp_vulkan_copy_buffer(pContext, &staging_buffer, &object->index_buffer, size);

    ext_func->vkDestroyBuffer(pContext->logical_device, staging_buffer, NULL);
    ext_func->vkFreeMemory(pContext->logical_device, staging_memory, NULL);

    return true;
}

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

    if (ssp_vulkan_create_vertex_buffer(&engine->renderer->vulkan_context, object, vertices, vertices_count) != SSP_ERROR_CODE_SUCCESS
        || !ssp_vulkan_create_index_buffer(&engine->renderer->vulkan_context, object, indices, object->indices_count) != SSP_ERROR_CODE_SUCCESS) {
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

    ext_func->vkDestroyBuffer(context->logical_device, object->vertex_buffer, NULL);
    ext_func->vkFreeMemory(context->logical_device, object->vertex_memory, NULL);

    ext_func->vkDestroyBuffer(context->logical_device, object->index_buffer, NULL);
    ext_func->vkFreeMemory(context->logical_device, object->index_memory, NULL);

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

#include "object.h"

static enum SSP_ERROR_CODE ssp_object_create_buffer(struct SSPVulkanContextExtFunc *ext_func,
    struct SSPVulkanDevice *device,
    struct SSPVulkanCommandContext *command_context,
    struct SSPVulkanBuffer *buffer,
    void *data,
    VkDeviceSize size,
    uint32_t data_count,
    VkBufferUsageFlags usage_flags,
    enum SSP_VULKAN_COPY_BUFFER_POST_PROCESS post_processes_flags)
{
    enum SSP_ERROR_CODE err_code;
    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    buffer->state = SSP_VULKAN_BUFFER_STATE_NOT_INITIALIZED;

    ssp_vulkan_stage_buffer(ext_func, device, size, data, &staging_buffer, &staging_memory);
    (err_code = ssp_vulkan_create_buffer(ext_func, device, size, usage_flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer->buffer, &buffer->memory)) ||
    (err_code = ssp_vulkan_copy_buffer_queue_push(command_context->transfer_copy_buffer_queue, buffer, staging_buffer, staging_memory, size, post_processes_flags));

    return err_code;
}

enum SSP_ERROR_CODE spp_object_create(struct SSPObject *object, struct SSPEngine *engine, vec2 *vertices_pos, vec3 color, uint16_t *indices, uint32_t vertices_count)
{
    object->vertices = calloc(vertices_count, sizeof(struct SSPShaderVertex));

    object->vertices_count = vertices_count;
    object->indices_count = (vertices_count - 2) * 3;
    glm_mat4_identity(object->push_constant.model);

    for (uint32_t i = 0; i < vertices_count; ++i) {
        glm_vec3(color, object->vertices[i].color);
        glm_vec2(vertices_pos[i], object->vertices[i].pos);
    }

    object->status = SSP_OBJECT_NOT_INITIALISED;

    struct SSPVulkanContext *context = &engine->renderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;
    enum SSP_VULKAN_COPY_BUFFER_POST_PROCESS post_process_flags = SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER | SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY;

    if (ssp_object_create_buffer(ext_func, &context->device, &context->command_context, &object->vertex_buffer, object->vertices, sizeof(struct SSPShaderVertex) * object->vertices_count, vertices_count, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, post_process_flags) != SSP_ERROR_CODE_SUCCESS
        || ssp_object_create_buffer(ext_func, &context->device, &context->command_context, &object->index_buffer, indices, sizeof(uint16_t) * object->indices_count, object->indices_count, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, post_process_flags) != SSP_ERROR_CODE_SUCCESS) {
        ssp_object_destroy(engine, object);
        return SSP_ERROR_CODE_CREATE_OBJECT;
    }

    return SSP_ERROR_CODE_SUCCESS;
}

void ssp_object_destroy(struct SSPEngine *engine, struct SSPObject *object)
{
    struct SSPVulkanContext *context = &engine->renderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;
    VkDevice logical_device = context->device.logical_device;
    struct SSPVulkanBuffer *vertex_buffer = &object->vertex_buffer;
    struct SSPVulkanBuffer *index_buffer = &object->index_buffer;

    ext_func->vkDestroyBuffer(logical_device, vertex_buffer->buffer, NULL);
    ext_func->vkFreeMemory(logical_device, vertex_buffer->memory, NULL);

    ext_func->vkDestroyBuffer(logical_device, index_buffer->buffer, NULL);
    ext_func->vkFreeMemory(logical_device, index_buffer->memory, NULL);

    free(object->vertices);
}

enum SSP_ERROR_CODE ssp_object_create_rectangle(struct SSPObject *object, struct SSPEngine *engine, vec2 pos, vec2 size, vec3 color)
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

    vec2 texture_coordinates[] = {
        {1.f, 1.f},
        {0.f, 1.f},
        {0.f, 0.f},
        {1.f, 0.f}
    };

    enum SSP_ERROR_CODE err_code;

    (err_code = spp_object_create(object, engine, vertices, color, indices, 4))
    || (err_code = ssp_object_update_texture_coordinates(object, engine, texture_coordinates));

    return err_code;
}

enum SSP_ERROR_CODE ssp_object_update_texture_coordinates(struct SSPObject *object, struct SSPEngine *engine, vec2 *texture_coordinates)
{
    struct SSPVulkanContext *context = &engine->renderer->vulkan_context;
    struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;
    struct SSPVulkanDevice *device = &context->device;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;
    VkDeviceSize size = sizeof(struct SSPShaderVertex) * object->vertices_count;

    object->status = SSP_OBJECT_UPLOADING;

    for (uint32_t i = 0; i < object->vertices_count; ++i)
        glm_vec2(texture_coordinates[i], object->vertices[i].texture_coordinate);

    ssp_vulkan_stage_buffer(ext_func, device, size, object->vertices, &staging_buffer, &staging_memory);
    ssp_vulkan_copy_buffer_queue_push(context->command_context.transfer_copy_buffer_queue, &object->vertex_buffer, staging_buffer, staging_memory, size, SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER | SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY);

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_object_apply_texture(struct SSPObject *object, struct SSPTexture *texture)
{
    object->is_textured = true;
    object->texture = texture;

    return SSP_ERROR_CODE_SUCCESS;
}

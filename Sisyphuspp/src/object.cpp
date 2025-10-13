#include "object.hpp"
#include "engine.hpp"
#include "object.h"

namespace Sisyphus {
    Object::Object(Engine &engine, std::vector<glm::vec2> verticesPos, glm::vec3 color, std::vector<uint16_t> indices, uint32_t verticesCount):
        _object(std::make_unique<struct SSPObject>())
    {
        std::vector<SSPShaderVertex> vertices(verticesCount);

        _object->indices_count = (verticesCount - 2) * 3;

        glm_mat4_identity(_object->vertex_push_constant.model);

        for (uint32_t i = 0; i < verticesCount; ++i) {
            vertices[i].color[0] = color.x;
            vertices[i].color[1] = color.y;
            vertices[i].color[2] = color.z;

            vertices[i].pos[0] = verticesPos[i].x;
            vertices[i].pos[1] = verticesPos[i].y;
        }

        SSPVulkanContext *vulkan_context = engine.getRenderer().dataVulkanContext();
        SSPVulkanContextExtFunc *ext_func = &vulkan_context->ext_func;

        if (ssp_vulkan_create_vertex_buffer(ext_func, &vulkan_context->device, &vulkan_context->command_context, &_object->vertex_buffer, &_object->vertex_memory, vertices.data(), verticesCount) != SSP_ERROR_CODE_SUCCESS
            || ssp_vulkan_create_index_buffer(ext_func, &vulkan_context->device, &vulkan_context->command_context, &_object->index_buffer, &_object->index_memory, indices.data(), _object->indices_count) != SSP_ERROR_CODE_SUCCESS) {
            std::cerr << "Couldn't create object, object address : " << this << std::endl;
        }
    }

    void Object::FreeBuffers(Engine &engine)
    {
        struct SSPVulkanContext *context = &engine.getRenderer().data()->vulkan_context;
        struct SSPVulkanContextExtFunc *ext_func = &context->ext_func;
        VkDevice logical_device = context->device.logical_device;

        ext_func->vkDestroyBuffer(logical_device, _object->vertex_buffer, NULL);
        ext_func->vkFreeMemory(logical_device, _object->vertex_memory, NULL);

        ext_func->vkDestroyBuffer(logical_device, _object->index_buffer, NULL);
        ext_func->vkFreeMemory(logical_device, _object->index_memory, NULL);
    }

    Object::~Object()
    {
    }

    const std::vector<uint16_t> Rectangle::_indices = {0, 1, 2, 2, 3, 0};

    Rectangle::Rectangle(Engine &engine, glm::vec2 pos, glm::vec2 size, glm::vec3 color):
        Object(engine, generateVerticesPos(pos, size), color, _indices, 4)
    {
    }

    Rectangle::~Rectangle()
    {
    }

    std::vector<glm::vec2> Rectangle::generateVerticesPos(glm::vec2 pos, glm::vec2 size)
    {
        glm::vec2 oppositeVertices = pos + size;
        std::vector<glm::vec2> vertices = {
            pos,
            {oppositeVertices.x, pos.y},
            oppositeVertices,
            {pos.x, oppositeVertices.y}
        };

        return vertices;
    }
}
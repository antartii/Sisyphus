#include "vulkan_context/buffer.h"

enum SSP_ERROR_CODE ssp_vulkan_create_buffer(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, VkDeviceSize size, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags memory_properties, VkBuffer *buffer, VkDeviceMemory *memory)
{
    VkBufferCreateInfo buffer_info = {0};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = buffer_usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (ext_func->vkCreateBuffer(device->logical_device, &buffer_info, NULL, buffer) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_CREATE_BUFFER;
    VkMemoryRequirements memory_requirements;
    ext_func->vkGetBufferMemoryRequirements(device->logical_device, *buffer, &memory_requirements);

    uint32_t memory_type_index = ssp_vulkan_find_memory_type(ext_func, device->physical_device, memory_requirements.memoryTypeBits, memory_properties);
    if (memory_type_index == -1)
        return SSP_ERROR_CODE_VULKAN_CREATE_BUFFER;

    VkMemoryAllocateInfo memory_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = memory_type_index
    };

    ext_func->vkAllocateMemory(device->logical_device, &memory_allocate_info, NULL, memory);
    ext_func->vkBindBufferMemory(device->logical_device, *buffer, *memory, 0);

    return SSP_ERROR_CODE_SUCCESS;
}

void ssp_vulkan_stage_buffer(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, VkDeviceSize size, void *data, VkBuffer *staging_buffer, VkDeviceMemory *staging_buffer_memory)
{
    ssp_vulkan_create_buffer(ext_func, device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
    void* data_staging;
    ext_func->vkMapMemory(device->logical_device, *staging_buffer_memory, 0, size, 0, &data_staging);
    memcpy(data_staging, data, (size_t) size);
    ext_func->vkUnmapMemory(device->logical_device, *staging_buffer_memory);
}

enum SSP_ERROR_CODE ssp_vulkan_create_vertex_buffer(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, struct SSPVulkanCommandContext *command_context, VkBuffer *buffer, VkDeviceMemory *memory, struct SSPShaderVertex *vertices, uint32_t vertices_count)
{
    VkDeviceSize size = sizeof(struct SSPShaderVertex) * vertices_count;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    ssp_vulkan_stage_buffer(ext_func, device, size, vertices, &staging_buffer, &staging_memory);

    ssp_vulkan_create_buffer(ext_func, device, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);
    ssp_vulkan_copy_buffer_queue_push(command_context->transfer_copy_buffer_queue, buffer, staging_buffer, staging_memory, size, SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER | SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY);

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_create_index_buffer(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, struct SSPVulkanCommandContext *command_context, VkBuffer *buffer, VkDeviceMemory *memory, uint16_t *indices, uint32_t indices_count)
{
    VkDeviceSize size = sizeof(uint16_t) * indices_count;
    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    ssp_vulkan_stage_buffer(ext_func, device, size, indices, &staging_buffer, &staging_memory);
    ssp_vulkan_create_buffer(ext_func, device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, memory);
    ssp_vulkan_copy_buffer_queue_push(command_context->transfer_copy_buffer_queue, buffer, staging_buffer, staging_memory, size, SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER | SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY);

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_copy_buffer_queue_push(struct SSPDynamicArray *transfer_copy_buffer_queue, VkBuffer *dst_buffer, VkBuffer src_buffer, VkDeviceMemory src_memory, VkDeviceSize size, enum SSP_VULKAN_COPY_BUFFER_POST_PROCESS prost_process_flags)
{
    struct SSPVulkanCopyBufferData data = {0};
    data.dst_buffer = dst_buffer;
    data.size = size;
    data.post_process_bitmask = prost_process_flags;
    data.src_buffer = src_buffer;
    data.src_memory = src_memory;

    ssp_dynamic_array_push(transfer_copy_buffer_queue, &data);
}

enum SSP_ERROR_CODE ssp_vulkan_copy_buffer_to_image_queue_push(struct SSPVulkanContextExtFunc *ext_func, VkCommandBuffer command_buffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, vec3 offset)
{
    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    ext_func->vkBeginCommandBuffer(command_buffer, &begin_info);

    VkBufferImageCopy region = {0};
    region.imageOffset.x = offset[0];
    region.imageOffset.y = offset[1];
    region.imageOffset.z = offset[2];
    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;

    ext_func->vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    ext_func->vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
}

/*enum SSP_ERROR_CODE ssp_vulkan_copy_buffer_to_image_round(struct SSPVulkanContextExtFunc *ext_func, struct SSPDynamicArray *transfer_copy_buffer_queue)
{
    struct SSPDynamicArray *copy_buffer_queue = pContext->transfer_copy_buffer_queue;

    if (transfer_copy_buffer_queue->size == 0)
        return SSP_ERROR_CODE_SUCCESS;

    struct SSPVulkanContextExtFunc *ext_func = &pContext->ext_func;

    VkCommandBuffer command_buffer = pContext->transfer_command_buffers[pContext->transfer_command_buffer_copy_index];
    ext_func->vkResetCommandBuffer(command_buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
}*/

enum SSP_ERROR_CODE ssp_vulkan_copy_buffer_round(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device, VkFence *transfer_copy_buffer_fence)
{
    if (command_context->transfer_copy_buffer_queue->size == 0)
        return SSP_ERROR_CODE_SUCCESS;

    VkCommandBuffer command_buffer = command_context->transfer_command_buffers[command_context->transfer_command_buffer_copy_index];

    ext_func->vkResetCommandBuffer(command_buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    ext_func->vkBeginCommandBuffer(command_buffer, &begin_info);

    VkBufferCopy copy_region = {0};
    
    uint32_t src_queue_family_index;
    uint32_t dst_queue_family_index;

    if (device->queue_family_indices.graphic == device->queue_family_indices.transfer) {
        src_queue_family_index = VK_QUEUE_FAMILY_IGNORED;
        dst_queue_family_index = VK_QUEUE_FAMILY_IGNORED;
    } else {
        src_queue_family_index = device->queue_family_indices.transfer;
        dst_queue_family_index = device->queue_family_indices.graphic;
    }

    for (size_t i = 0; i < command_context->transfer_copy_buffer_queue->size; ++i) {
        struct SSPVulkanCopyBufferData *data = ssp_dynamic_array_get(command_context->transfer_copy_buffer_queue, i);

        copy_region.size = data->size;
        ext_func->vkCmdCopyBuffer(command_buffer, data->src_buffer, *(data->dst_buffer), 1, &copy_region);

        VkBufferMemoryBarrier buffer_barrier = {0};
        buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        buffer_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        buffer_barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        buffer_barrier.srcQueueFamilyIndex = src_queue_family_index;
        buffer_barrier.dstQueueFamilyIndex = dst_queue_family_index;
        buffer_barrier.buffer = *(data->dst_buffer);
        buffer_barrier.size = VK_WHOLE_SIZE;

        ext_func->vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, NULL, 1, &buffer_barrier, 0, NULL);
    }

    ext_func->vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &(command_buffer);

    ext_func->vkResetFences(device->logical_device, 1, transfer_copy_buffer_fence);
    ext_func->vkQueueSubmit(device->transfer_queue, 1, &submit_info, *transfer_copy_buffer_fence);

    ext_func->vkWaitForFences(device->logical_device, 1, transfer_copy_buffer_fence, VK_TRUE, UINT64_MAX); // need to change to not wait but check status fence
    ext_func->vkQueueWaitIdle(device->transfer_queue);

    for (size_t i = 0; i < command_context->transfer_copy_buffer_queue->size; ++i) {
        struct SSPVulkanCopyBufferData *data = ssp_dynamic_array_get(command_context->transfer_copy_buffer_queue, i);
    
        if (data->post_process_bitmask & SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER)
            ext_func->vkDestroyBuffer(device->logical_device, data->src_buffer, NULL);
        if (data->post_process_bitmask & SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY)
            ext_func->vkFreeMemory(device->logical_device, data->src_memory, NULL);
    }
    
    command_context->transfer_copy_buffer_queue->size = 0;

    return SSP_ERROR_CODE_SUCCESS;
}

#include "vulkan_context/buffer.h"
#include "vulkan_context/graphic.h"

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

static enum SSP_ERROR_CODE ssp_vulkan_copy_image_buffer_queue_post_processes(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device, struct SSPVulkanPipelineContext *pipeline_context)
{
    struct SSPDynamicArray *array = command_context->transfer_copy_buffer_to_image_queue;

    for (size_t i = 0; i < array->size; ++i) {
        struct SSPVulkanCopyBufferToImageData *data = ssp_dynamic_array_get(array, i);

        if (data->image->state < SSP_VULKAN_BUFFER_STATE_LOADING
            || command_context->curr_transfer_image_semaphore_value < data->image->queue_batch)
            continue;
        
        data->image->state = SSP_VULKAN_BUFFER_STATE_READY;

        ssp_vulkan_create_image_view(ext_func, device, VK_FORMAT_R8G8B8A8_SRGB, data->image->image, &data->image->image_view);

        VkDescriptorImageInfo image_info = {0};
        image_info.sampler = pipeline_context->texture_sampler;
        image_info.imageView = data->image->image_view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        data->image->texture_index = pipeline_context->texture_count++;

        for (size_t i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i) {
            VkWriteDescriptorSet write = {0};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            write.dstSet = pipeline_context->descriptor_sets[i];
            write.dstBinding = 1;
            write.dstArrayElement = data->image->texture_index;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.descriptorCount = 1;
            write.pImageInfo = &image_info;
            ext_func->vkUpdateDescriptorSets(device->logical_device, 1, &write, 0, NULL);
        }

        if (data->post_process_bitmask & SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER)
            ext_func->vkDestroyBuffer(device->logical_device, data->src_buffer, NULL);
        if (data->post_process_bitmask & SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY)
            ext_func->vkFreeMemory(device->logical_device, data->src_memory, NULL);

        ssp_dynamic_array_replace(array, array->size - 1, i);
    }
}

enum SSP_ERROR_CODE ssp_vulkan_copy_image_buffer_queue_round(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device, struct SSPVulkanPipelineContext *pipeline_context)
{
    struct SSPDynamicArray *array = command_context->transfer_copy_buffer_to_image_queue;

    if (array->size == 0)
        return SSP_ERROR_CODE_SUCCESS;

    ext_func->vkGetSemaphoreCounterValueKHR(device->logical_device, command_context->transfer_image_semaphore_timeline, &command_context->curr_transfer_image_semaphore_value);
    if (command_context->curr_transfer_image_semaphore_value < command_context->transfer_image_semaphore_value)
        return SSP_ERROR_CODE_SKIP_COPY;

    ssp_vulkan_copy_image_buffer_queue_post_processes(ext_func, command_context, device, pipeline_context);

    VkBufferImageCopy image_copy = {0};
    VkCommandBuffer command_buffer = command_context->transfer_command_buffers[command_context->transfer_command_buffer_image_copy_index];

    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    ext_func->vkBeginCommandBuffer(command_buffer, &begin_info);

    for (size_t i = 0; i < array->size; ++i) {
        struct SSPVulkanCopyBufferToImageData *data = ssp_dynamic_array_get(array, i);

        if (data->image->state == SSP_VULKAN_BUFFER_STATE_LOADING)
            continue;

        data->image->state = SSP_VULKAN_BUFFER_STATE_LOADING;
        data->image->queue_batch = command_context->transfer_image_semaphore_value + 1;

        ssp_vulkan_transition_image_layout(ext_func, data->image->image,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            0, VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            command_context->transfer_command_buffers[command_context->transfer_command_buffer_image_copy_index],
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED);

        image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_copy.imageSubresource.layerCount = 1;

        image_copy.imageOffset = (VkOffset3D) {data->offsetX, data->offsetY, 0};
        image_copy.imageExtent = (VkExtent3D) {data->width, data->height, 1};

        vkCmdCopyBufferToImage(command_buffer, data->src_buffer, data->image->image, data->image_layout, 1, &image_copy);

        ssp_vulkan_transition_image_layout(ext_func, data->image->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            command_buffer, device->queue_family_indices.transfer, device->queue_family_indices.graphic);
    }

    vkEndCommandBuffer(command_buffer);

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    uint64_t wait_value = command_context->transfer_image_semaphore_value++;
    uint64_t signal_value = command_context->transfer_image_semaphore_value;

    VkTimelineSemaphoreSubmitInfoKHR sem_info = {0};
    sem_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR;
    sem_info.pSignalSemaphoreValues = &signal_value;
    sem_info.pWaitSemaphoreValues = &wait_value;
    sem_info.signalSemaphoreValueCount = 1;
    sem_info.waitSemaphoreValueCount = 1;

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.pSignalSemaphores = &command_context->transfer_image_semaphore_timeline;
    submit_info.pWaitDstStageMask = &wait_stage;
    submit_info.pWaitSemaphores = &command_context->transfer_image_semaphore_timeline;
    submit_info.signalSemaphoreCount = 1;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pNext = &sem_info;

    ext_func->vkQueueSubmit(device->transfer_queue, 1, &submit_info, NULL);

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_copy_image_buffer_queue_push(struct SSPDynamicArray *transfer_transition_queue,
    struct SSPVulkanCommandContext *command_context,
    struct SSPVulkanImage *image,
    uint32_t height,
    uint32_t width,
    int offsetX,
    int offsetY,
    VkBuffer src_buffer,
    VkDeviceMemory src_memory,
    VkImageLayout image_layout,
    int post_process_bitmask)
{
    struct SSPVulkanCopyBufferToImageData data = {0};
    data.height = height;
    data.width = width;
    data.image = image;
    data.image->queue_batch = command_context->transfer_image_semaphore_value + 1;
    data.image->state = SSP_VULKAN_BUFFER_STATE_NOT_INITIALIZED;
    data.image_layout = image_layout;
    data.offsetX = offsetX;
    data.offsetY = offsetY;
    data.post_process_bitmask = post_process_bitmask;
    data.src_buffer = src_buffer;
    data.src_memory = src_memory;

    ssp_dynamic_array_push(transfer_transition_queue, &data);

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_copy_buffer_queue_push(struct SSPDynamicArray *transfer_copy_buffer_queue, struct SSPVulkanBuffer *dst_buffer, VkBuffer src_buffer, VkDeviceMemory src_memory, VkDeviceSize size, enum SSP_VULKAN_COPY_BUFFER_POST_PROCESS post_process_flags)
{
    struct SSPVulkanCopyBufferData data = {0};
    data.dst_buffer = dst_buffer;
    data.size = size;
    data.post_process_bitmask = post_process_flags;
    data.src_buffer = src_buffer;
    data.src_memory = src_memory;

    ssp_dynamic_array_push(transfer_copy_buffer_queue, &data);
    return SSP_ERROR_CODE_SUCCESS;
}

static void ssp_vulkan_copy_buffer_post_processes(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device)
{
    struct SSPDynamicArray *transfer_copy_buffer_queue = command_context->transfer_copy_buffer_queue;
    
    for (size_t i = 0; i < transfer_copy_buffer_queue->size;) {
        struct SSPVulkanCopyBufferData *data = ssp_dynamic_array_get(transfer_copy_buffer_queue, i);
        
        if (data->dst_buffer->state < SSP_VULKAN_BUFFER_STATE_LOADING
            || command_context->curr_transfer_semaphore_value < data->dst_buffer->queue_batch) {
            ++i;
            continue;
        }
        data->dst_buffer->state = SSP_VULKAN_BUFFER_STATE_READY;

        if (data->post_process_bitmask & SSP_VULKAN_COPY_BUFFER_DESTROY_SRC_BUFFER)
            ext_func->vkDestroyBuffer(device->logical_device, data->src_buffer, NULL);
        if (data->post_process_bitmask & SSP_VULKAN_COPY_BUFFER_FREE_SRC_MEMORY)
            ext_func->vkFreeMemory(device->logical_device, data->src_memory, NULL);

        ssp_dynamic_array_replace(transfer_copy_buffer_queue, transfer_copy_buffer_queue->size - 1, i);
        
    }
}

enum SSP_ERROR_CODE ssp_vulkan_transition_image_layout(
    struct SSPVulkanContextExtFunc *ext_func,
    VkImage image,
    VkImageLayout old_layout,
    VkImageLayout new_layout,
    VkAccessFlags2 src_access_mask,
    VkAccessFlags2 dst_access_mask,
    VkPipelineStageFlags2 src_stage_mask,
    VkPipelineStageFlags2 dst_stage_mask,
    VkCommandBuffer command_buffer,
    uint32_t src_queue_family_index,
    uint32_t dst_queue_family_index)
{
    VkImageMemoryBarrier2 barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.srcAccessMask = src_access_mask;
    barrier.dstAccessMask = dst_access_mask;
    barrier.srcStageMask = src_stage_mask;
    barrier.dstStageMask = dst_stage_mask;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = src_queue_family_index;
    barrier.dstQueueFamilyIndex = dst_queue_family_index;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkDependencyInfo dependency_info = {0};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &barrier;

    ext_func->vkCmdPipelineBarrier2KHR(command_buffer, &dependency_info);
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_copy_buffer_round(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanCommandContext *command_context, struct SSPVulkanDevice *device)
{
    if (command_context->transfer_copy_buffer_queue->size == 0)
        return SSP_ERROR_CODE_SUCCESS;

    ext_func->vkGetSemaphoreCounterValueKHR(device->logical_device, command_context->transfer_semaphore_timeline, &command_context->curr_transfer_semaphore_value);

    if (command_context->curr_transfer_semaphore_value < command_context->transfer_semaphore_timeline_value)
        return SSP_ERROR_CODE_SKIP_COPY;

    ssp_vulkan_copy_buffer_post_processes(ext_func, command_context, device);

    VkCommandBuffer command_buffer = command_context->transfer_command_buffers[command_context->transfer_command_buffer_copy_index];

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

        data->dst_buffer->state = SSP_VULKAN_BUFFER_STATE_LOADING;
        data->dst_buffer->queue_batch = command_context->transfer_semaphore_timeline_value + 1;

        copy_region.size = data->size;
        ext_func->vkCmdCopyBuffer(command_buffer, data->src_buffer, data->dst_buffer->buffer, 1, &copy_region);

        VkBufferMemoryBarrier buffer_barrier = {0};
        buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        buffer_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        buffer_barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        buffer_barrier.srcQueueFamilyIndex = src_queue_family_index;
        buffer_barrier.dstQueueFamilyIndex = dst_queue_family_index;
        buffer_barrier.buffer = data->dst_buffer->buffer;
        buffer_barrier.size = VK_WHOLE_SIZE;

        ext_func->vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, NULL, 1, &buffer_barrier, 0, NULL);
    }

    ext_func->vkEndCommandBuffer(command_buffer);

    uint64_t wait_sem_value = command_context->transfer_semaphore_timeline_value++;
    uint64_t signal_sem_value = command_context->transfer_semaphore_timeline_value + 1;

    VkTimelineSemaphoreSubmitInfoKHR sem_wait_info = {0};
    sem_wait_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR;
    sem_wait_info.waitSemaphoreValueCount = 1;
    sem_wait_info.pWaitSemaphoreValues = &wait_sem_value;

    sem_wait_info.signalSemaphoreValueCount = 1;
    sem_wait_info.pSignalSemaphoreValues = &signal_sem_value;

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.pWaitSemaphores = &command_context->transfer_semaphore_timeline;
    submit_info.pSignalSemaphores = &command_context->transfer_semaphore_timeline;
    submit_info.pNext = &sem_wait_info;
    submit_info.pWaitDstStageMask = &wait_stage;
    submit_info.waitSemaphoreCount = 1;
    submit_info.signalSemaphoreCount = 1;

    ext_func->vkQueueSubmit(device->transfer_queue, 1, &submit_info, NULL);

    return SSP_ERROR_CODE_SUCCESS;
}

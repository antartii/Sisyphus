#include "vulkan_context/graphic.h"
#include "object.h"

enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_set_layout(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, struct SSPVulkanPipelineContext *pipeline_context)
{
    VkDescriptorSetLayoutBinding descriptor_binding = {0};

    descriptor_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptor_binding.descriptorCount = 1;
    descriptor_binding.pImmutableSamplers = NULL;
    descriptor_binding.binding = 0;

    VkDescriptorSetLayoutCreateInfo descriptor_info = {0};
    descriptor_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_info.bindingCount = 1;
    descriptor_info.pBindings = &descriptor_binding;

    if (ext_func->vkCreateDescriptorSetLayout(device->logical_device, &descriptor_info, NULL, &pipeline_context->descriptor_set_layout) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_CREATE_DESCRIPTOR_SET_LAYOUT;
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_update_descriptor_sets(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, struct SSPVulkanDevice *device)
{
    VkDescriptorBufferInfo buffer_info = {0};
    buffer_info.range = sizeof(struct SSPShaderUniformBuffer);

    VkWriteDescriptorSet write_descriptor = {0};
    write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor.dstBinding = 0;
    write_descriptor.dstArrayElement = 0;
    write_descriptor.descriptorCount = 1;
    write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor.pBufferInfo = &buffer_info;
    
    for (size_t i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i) {
        buffer_info.buffer = pipeline_context->uniform_buffers[i];
        write_descriptor.dstSet = pipeline_context->descriptor_sets[i];

        ext_func->vkUpdateDescriptorSets(device->logical_device, 1, &write_descriptor, 0, NULL);
    }
}

enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_sets(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, struct SSPVulkanDevice *device)
{
    VkDescriptorSetLayout *layouts = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSetLayout));
    pipeline_context->descriptor_sets = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkDescriptorSet));

    for (size_t i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
        layouts[i] = pipeline_context->descriptor_set_layout;

    VkDescriptorSetAllocateInfo descriptor_set_info = {0};
    descriptor_set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_info.descriptorPool = pipeline_context->descriptor_pool;
    descriptor_set_info.descriptorSetCount = SSP_MAX_FRAMES_IN_FLIGHT;
    descriptor_set_info.pSetLayouts = layouts;

    if (ext_func->vkAllocateDescriptorSets(device->logical_device, &descriptor_set_info, pipeline_context->descriptor_sets) != VK_SUCCESS) {
        free(layouts);
        return false;
    }

    ssp_vulkan_update_descriptor_sets(ext_func, pipeline_context, device);

    free(layouts);
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_create_descriptor_pool(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanDevice *device, struct SSPVulkanPipelineContext *pipeline_context)
{
    VkDescriptorPoolSize size = {0};
    size.descriptorCount = SSP_MAX_FRAMES_IN_FLIGHT;
    size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    VkDescriptorPoolCreateInfo descriptor_info = {0};
    descriptor_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptor_info.maxSets = SSP_MAX_FRAMES_IN_FLIGHT;
    descriptor_info.poolSizeCount = 1;
    descriptor_info.pPoolSizes = &size;
    
    if (ext_func->vkCreateDescriptorPool(device->logical_device, &descriptor_info, NULL, &pipeline_context->descriptor_pool) != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_CREATE_DESCRIPTOR_POOL;
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_create_uniform_buffers(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, struct SSPVulkanDevice *device)
{
    pipeline_context->uniform_buffers = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkBuffer));
    pipeline_context->uniform_buffers_memory = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(VkDeviceMemory));
    pipeline_context->uniform_buffers_mapped = calloc(SSP_MAX_FRAMES_IN_FLIGHT, sizeof(void *));

    for (size_t i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i) {
        VkDeviceSize size = sizeof(struct SSPShaderUniformBuffer);

        if (ssp_vulkan_create_buffer(ext_func, device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &(pipeline_context->uniform_buffers[i]), &(pipeline_context->uniform_buffers_memory[i])) != SSP_ERROR_CODE_SUCCESS
            || ext_func->vkMapMemory(device->logical_device, pipeline_context->uniform_buffers_memory[i], 0, size, 0, &(pipeline_context->uniform_buffers_mapped[i])) != VK_SUCCESS)
            return SSP_ERROR_CODE_VULKAN_CREATE_UNIFORM_BUFFERS;
    }
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_create_image_view(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanSwapchain *swapchain, struct SSPVulkanDevice *device)
{
    VkImageViewCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.components.a = VK_COMPONENT_SWIZZLE_A;
    create_info.components.r = VK_COMPONENT_SWIZZLE_R;
    create_info.components.g = VK_COMPONENT_SWIZZLE_G;
    create_info.components.b = VK_COMPONENT_SWIZZLE_B;
    create_info.format = swapchain->format;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

    swapchain->image_views = calloc(swapchain->images_count, sizeof(VkImageView));
    for (int i = 0; i < swapchain->images_count; ++i) {
        create_info.image = swapchain->images[i];
        if (ext_func->vkCreateImageView(device->logical_device, &create_info, NULL, &(swapchain->image_views[i])) != VK_SUCCESS)
            return SSP_ERROR_CODE_VULKAN_CREATE_IMAGE_VIEW;
    }

    return SSP_ERROR_CODE_SUCCESS;
}

static VkShaderModule ssp_vulkan_create_shader_module(struct SSPVulkanContextExtFunc *ext_func, VkDevice logical_device, const char *code, uint32_t code_size)
{
    VkShaderModuleCreateInfo shader_module_info = {0};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_info.codeSize = code_size;
    shader_module_info.pCode = (const uint32_t *) code;

    VkShaderModule shader_module;

    ext_func->vkCreateShaderModule(logical_device, &shader_module_info, NULL, &shader_module);

    return shader_module;
}

enum SSP_ERROR_CODE ssp_vulkan_create_graphic_pipeline(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, struct SSPVulkanDevice *device, struct SSPVulkanSwapchain *swapchain)
{
    uint32_t code_size;
    const char *shader_file = SSP_SHADERS_PATH;
    char *shader_code = read_file(shader_file, &code_size);

    VkShaderModule shader_module = ssp_vulkan_create_shader_module(ext_func, device->logical_device, shader_code, code_size);
    
    VkPipelineShaderStageCreateInfo vert_stage_info = {0};

    vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_info.module = shader_module;
    vert_stage_info.pName = SSP_SHADER_VERTEX_ENTRY_POINT;

    VkPipelineShaderStageCreateInfo frag_stage_info = {0};
    frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_info.module = shader_module;
    frag_stage_info.pName = SSP_SHADER_FRAGMENT_ENTRY_POINT;

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_stage_info, frag_stage_info};

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info = {0};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = 2;
    dynamic_state_info.pDynamicStates = dynamic_states;

    VkVertexInputBindingDescription vertex_binding_descriptions = {0};
    vertex_binding_descriptions.binding = 0;
    vertex_binding_descriptions.stride = sizeof(struct SSPShaderVertex);
    vertex_binding_descriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertex_attribute_descriptions[2];
    vertex_attribute_descriptions[0].location = 0;
    vertex_attribute_descriptions[0].binding = 0;
    vertex_attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    vertex_attribute_descriptions[0].offset = offsetof(struct SSPShaderVertex, pos);

    vertex_attribute_descriptions[1].location = 1;
    vertex_attribute_descriptions[1].binding = 0;
    vertex_attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_attribute_descriptions[1].offset = offsetof(struct SSPShaderVertex, color);

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding_descriptions;
    vertex_input_info.vertexAttributeDescriptionCount = 2;
    vertex_input_info.pVertexAttributeDescriptions = vertex_attribute_descriptions;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {0};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    pipeline_context->viewport = (VkViewport) {
        .x = 0,
        .y = 0,
        .height = (float) swapchain->extent.height,
        .width = (float) swapchain->extent.width,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkPipelineViewportStateCreateInfo viewport_state_info = {0};
    viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterization_info = {0};
    rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_info.depthClampEnable = VK_FALSE;
    rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_info.depthBiasClamp = VK_FALSE;
    rasterization_info.depthBiasSlopeFactor = 1.0f;
    rasterization_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample_info = {0};
    multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_info.sampleShadingEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_A_BIT
        | VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blend_info = {0};
    color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.logicOpEnable = VK_FALSE;
    color_blend_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;

    VkPushConstantRange push_constant_range = {0};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.size = sizeof(struct SSPShaderPushConstant);

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &pipeline_context->descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (ext_func->vkCreatePipelineLayout(device->logical_device, &pipeline_layout_info, NULL, &pipeline_context->pipeline_layout) != VK_SUCCESS) {
        ext_func->vkDestroyShaderModule(device->logical_device, shader_module, NULL);
        return SSP_ERROR_CODE_VULKAN_CREATE_GRAPHIC_PIPELINE;
    }

    VkPipelineRenderingCreateInfo pipeline_rendering_info = {0};
    pipeline_rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipeline_rendering_info.colorAttachmentCount = 1;
    pipeline_rendering_info.pColorAttachmentFormats = &swapchain->format;

    VkGraphicsPipelineCreateInfo graphic_pipeline_info = {0};
    graphic_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphic_pipeline_info.pNext = &pipeline_rendering_info;
    graphic_pipeline_info.stageCount = 2;
    graphic_pipeline_info.pStages = shader_stages;
    graphic_pipeline_info.pVertexInputState = &vertex_input_info;
    graphic_pipeline_info.pInputAssemblyState = &input_assembly_info;
    graphic_pipeline_info.pViewportState = &viewport_state_info;
    graphic_pipeline_info.pRasterizationState = &rasterization_info;
    graphic_pipeline_info.pMultisampleState = &multisample_info;
    graphic_pipeline_info.pColorBlendState = &color_blend_info;
    graphic_pipeline_info.pDynamicState = &dynamic_state_info;
    graphic_pipeline_info.layout = pipeline_context->pipeline_layout;
    graphic_pipeline_info.renderPass = NULL;
    graphic_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    graphic_pipeline_info.basePipelineIndex = -1;

    VkResult result = ext_func->vkCreateGraphicsPipelines(device->logical_device, NULL, 1, &graphic_pipeline_info, NULL, &pipeline_context->graphic_pipeline);

    free(shader_code);
    ext_func->vkDestroyShaderModule(device->logical_device, shader_module, NULL);

    if (result != VK_SUCCESS)
        return SSP_ERROR_CODE_VULKAN_CREATE_GRAPHIC_PIPELINE;
    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_create_image(
    struct SSPVulkanContextExtFunc *ext_func,
    struct SSPVulkanDevice *device,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage *image,
    VkDeviceMemory *image_memory)
{
    VkImageCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType = VK_IMAGE_TYPE_2D;
    create_info.extent.width = width;
    create_info.extent.height = height;
    create_info.extent.depth = 1;
    create_info.mipLevels = 1;
    create_info.arrayLayers = 1;
    create_info.format = format;
    create_info.tiling = tiling;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;

    ext_func->vkCreateImage(device->logical_device, &create_info, NULL, image);

    VkMemoryRequirements mem_requirements;
    ext_func->vkGetImageMemoryRequirements(device->logical_device, *image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = ssp_vulkan_find_memory_type(ext_func, device->physical_device, mem_requirements.memoryTypeBits, properties);

    ext_func->vkAllocateMemory(device->logical_device, &alloc_info, NULL, image_memory);
    ext_func->vkBindImageMemory(device->logical_device, *image, *image_memory, 0);

    return SSP_ERROR_CODE_SUCCESS;
}

void ssp_vulkan_update_view(struct SSPCamera *camera, struct SSPVulkanPipelineContext *pipeline_context)
{
    if (!camera)
        return;

    mat4 view;
    glm_mat4_identity(view);

    for (size_t i = 0; i <  SSP_MAX_FRAMES_IN_FLIGHT; ++i)
        memcpy(pipeline_context->uniform_buffers_mapped[i], &view, sizeof(mat4));
}

void ssp_vulkan_update_proj(struct SSPCamera *camera, struct SSPVulkanPipelineContext *pipeline_context, VkExtent2D swapchain_extent)
{
    if (!camera)
        return;

    mat4 proj;
    glm_ortho(0.0f, (float) swapchain_extent.width, (float) swapchain_extent.height, 0.0f, -1.0f, 1.0f, proj);

    for (size_t i = 0; i <  SSP_MAX_FRAMES_IN_FLIGHT; ++i)
        memcpy(PTR_OFFSET(pipeline_context->uniform_buffers_mapped[i], sizeof(mat4)), &proj, sizeof(mat4));
}

enum SSP_ERROR_CODE ssp_vulkan_transition_image_layout(
    struct SSPVulkanContextExtFunc *ext_func,
    VkImage *image,
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
    barrier.image = *image;
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

enum SSP_ERROR_CODE ssp_vulkan_record_graphic_command_buffer(struct SSPVulkanContextExtFunc *ext_func,
    struct SSPVulkanSwapchain *swapchain,
    struct SSPVulkanCommandContext *command_context,
    struct SSPVulkanPipelineContext *pipeline_context,
    struct SSPDynamicArray *objects_to_draw)
{
    int current_frame = pipeline_context->current_frame;
    ext_func->vkResetCommandBuffer(command_context->graphic_command_buffers[current_frame], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    ext_func->vkBeginCommandBuffer(command_context->graphic_command_buffers[current_frame], &begin_info);
    ssp_vulkan_transition_image_layout(ext_func, &(swapchain->images[pipeline_context->image_index]), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, command_context->graphic_command_buffers[current_frame], VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED);

    VkClearValue clear_color = {.color.float32 = {0.0, 0.0, 0.0, 1.0}}; // todo : define it at init

    VkRenderingAttachmentInfo rendering_attachment_info = {0};
    rendering_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    rendering_attachment_info.clearValue = clear_color;
    rendering_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    rendering_attachment_info.imageView = swapchain->image_views[pipeline_context->image_index];
    rendering_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    rendering_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo rendering_info = {0};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.renderArea.offset.x = 0;
    rendering_info.renderArea.offset.y = 0;
    rendering_info.renderArea.extent = swapchain->extent;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments = &rendering_attachment_info;
    rendering_info.layerCount = 1;

    ext_func->vkCmdBeginRenderingKHR(command_context->graphic_command_buffers[current_frame], &rendering_info);
    
    ext_func->vkCmdBindPipeline(command_context->graphic_command_buffers[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_context->graphic_pipeline);
    ext_func->vkCmdSetViewport(command_context->graphic_command_buffers[current_frame], 0, 1, &pipeline_context->viewport);
    
    VkRect2D scissor = {0};
    scissor.extent = swapchain->extent;

    ext_func->vkCmdSetScissor(command_context->graphic_command_buffers[current_frame], 0, 1, &scissor);
    ext_func->vkCmdBindDescriptorSets(command_context->graphic_command_buffers[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_context->pipeline_layout, 0, 1, &(pipeline_context->descriptor_sets[current_frame]), 0, NULL);

    VkDeviceSize offset = 0;

    for (size_t i = 0; i < objects_to_draw->size; ++i) {
        struct SSPObject *object = ssp_dynamic_array_get(objects_to_draw, i);

        if (!object)
            continue;

        ext_func->vkCmdBindVertexBuffers(command_context->graphic_command_buffers[current_frame], 0, 1, &object->vertex_buffer, &offset);
        ext_func->vkCmdBindIndexBuffer(command_context->graphic_command_buffers[current_frame], object->index_buffer, offset, VK_INDEX_TYPE_UINT16);
        ext_func->vkCmdPushConstants(command_context->graphic_command_buffers[current_frame], pipeline_context->pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(struct SSPShaderPushConstant), &object->vertex_push_constant);
        ext_func->vkCmdDrawIndexed(command_context->graphic_command_buffers[current_frame], object->indices_count, 1, 0, 0, 0);
    }
    objects_to_draw->size = 0;

    ext_func->vkCmdEndRenderingKHR(command_context->graphic_command_buffers[current_frame]);
    ssp_vulkan_transition_image_layout(ext_func, &(swapchain->images[pipeline_context->image_index]), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, 0, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, command_context->graphic_command_buffers[current_frame], VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED);
    ext_func->vkEndCommandBuffer(command_context->graphic_command_buffers[current_frame]);

    return SSP_ERROR_CODE_SUCCESS;
}

enum SSP_ERROR_CODE ssp_vulkan_pipeline_destroy(struct SSPVulkanContextExtFunc *ext_func, struct SSPVulkanPipelineContext *pipeline_context, VkDevice logical_device)
{
    ext_func->vkDestroyDescriptorPool(logical_device, pipeline_context->descriptor_pool, NULL);
    ext_func->vkDestroyDescriptorSetLayout(logical_device, pipeline_context->descriptor_set_layout, NULL);
    ext_func->vkDestroyPipeline(logical_device, pipeline_context->graphic_pipeline, NULL);
    ext_func->vkDestroyPipelineLayout(logical_device, pipeline_context->pipeline_layout, NULL);

    if (pipeline_context->uniform_buffers_mapped) {
        for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
            ext_func->vkUnmapMemory(logical_device, pipeline_context->uniform_buffers_memory[i]);
        free(pipeline_context->uniform_buffers_mapped);
    }

    if (pipeline_context->uniform_buffers_memory) {
        for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
            ext_func->vkFreeMemory(logical_device, pipeline_context->uniform_buffers_memory[i], NULL);
        free(pipeline_context->uniform_buffers_memory);
    }

    if (pipeline_context->uniform_buffers) {
        for (int i = 0; i < SSP_MAX_FRAMES_IN_FLIGHT; ++i)
            ext_func->vkDestroyBuffer(logical_device, pipeline_context->uniform_buffers[i], NULL);
        free(pipeline_context->uniform_buffers);
    }
}

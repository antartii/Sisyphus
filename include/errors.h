#ifndef _SISYPHUS_ERRORS_H
#define _SISYPHUS_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

enum SSP_ERROR_SEVERITY {
    SSP_ERROR_SEVERITY_INFO = 0,
    SSP_ERROR_SEVERITY_WARNING,
    SSP_ERROR_SEVERITY_CRITICAL
};

#define SSP_ERRORS(X)  \
    X(SSP_ERROR_CODE_SUCCESS, "Success", SSP_ERROR_SEVERITY_INFO) \
    X(SSP_ERROR_CODE_UNDEFINED, "Undefined", SSP_ERROR_SEVERITY_WARNING)    \
    \
    X(SSP_ERROR_CODE_SURFACE_INIT, "Surface context couldn't be initialised", SSP_ERROR_SEVERITY_CRITICAL)  \
    \
    X(SSP_ERROR_CODE_VULKAN_LOADER_SUPPORT_INSTANCE_VERSION, "Vulkan higher instance version supported by the loader is too low", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_VULKAN_INSTANCE_LAYERS, "Vulkan instance layers required aren't available", SSP_ERROR_SEVERITY_CRITICAL)   \
    X(SSP_ERROR_CODE_VULKAN_INSTANCE_EXTENSIONS, "Vulkan instance extensions required aren't available", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_VULKAN_INSTANCE_CREATE, "Vulkan context, couldn't create VkInstance", SSP_ERROR_SEVERITY_CRITICAL) \
    X(SSP_ERROR_CODE_VULKAN_QUEUE_FAMILIES_NOT_VALID, "Vulkan couldn't find a correct queue family for each behavior queried", SSP_ERROR_SEVERITY_WARNING)  \
    X(SSP_ERROR_CODE_VULKAN_INVALID_SURFACE, "Vulkan context couldn't create a surface, no valid preprocessor macros has been set", SSP_ERROR_SEVERITY_CRITICAL)    \
    X(SSP_ERROR_CODE_VULKAN_LOGICAL_DEVICE_CREATION, "Vulkan context couldn't create the logical device", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_VULKAN_COMMAND_POOL_CREATION_GRAPHIC, "Vulkan context couldn't create the command pool for graphic operations", SSP_ERROR_SEVERITY_CRITICAL)   \
    X(SSP_ERROR_CODE_VULKAN_COMMAND_POOL_CREATION_TRANSFER, "Vulkan context couldn't create the command pool for transfer operations", SSP_ERROR_SEVERITY_CRITICAL) \
    X(SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_ALLOCATION_GRAPHIC, "Vulkan context couldn't allocate the command buffers for graphic operations", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_ALLOCATION_TRANSFER, "Vulkan context couldn't allocate the command buffers for transfer operations", SSP_ERROR_SEVERITY_CRITICAL)    \
    X(SSP_ERROR_CODE_VULKAN_COMMAND_BUFFER_RECORD_GRAPHIC, "Vulkan couldn't record the graphic command buffer properly", SSP_ERROR_SEVERITY_WARNING)    \
    X(SSP_ERROR_CODE_VULKAN_SWAPCHAIN_CREATION, "Vulkan context couldn't create the swapchain", SSP_ERROR_SEVERITY_CRITICAL)    \
    X(SSP_ERROR_CODE_VULKAN_SWAPCHAIN_EXTENT, "Vulkan context couldn't choose a valid extent size, either height or width is 0", SSP_ERROR_SEVERITY_CRITICAL)   \
    X(SSP_ERROR_CODE_VULKAN_CREATE_IMAGE_VIEW, "Vulkan context couldn't create the swapchain image views correctly", SSP_ERROR_SEVERITY_CRITICAL)   \
    X(SSP_ERROR_CODE_VULKAN_CREATE_PRESENT_COMPLETE_SEMAPHORES, "Vulkan context couldn't create the semaphores for presentations", SSP_ERROR_SEVERITY_CRITICAL) \
    X(SSP_ERROR_CODE_VULKAN_CREATE_RENDER_FINISHED_SEMAPHORES, "Vulkan context coukldn't create the semaphores for rendering", SSP_ERROR_SEVERITY_CRITICAL) \
    X(SSP_ERROR_CODE_VULKAN_CREATE_FRAMES_IN_FLIGHT_FENCES, "Vulkan context couldn't create the fences for frames", SSP_ERROR_SEVERITY_CRITICAL)    \
    X(SSP_ERROR_CODE_VULKAN_CREATE_DESCRIPTOR_SET_LAYOUT, "Vulkan context couldn't create the descriptors sets layouts", SSP_ERROR_SEVERITY_CRITICAL)   \
    X(SSP_ERROR_CODE_VULKAN_CREATE_GRAPHIC_PIPELINE, "VUlkan context_couldn't create the graphic pipeline", SSP_ERROR_SEVERITY_CRITICAL)    \
    X(SSP_ERROR_CODE_VULKAN_CREATE_UNIFORM_BUFFERS, "Vulkan context couldn't create the uniform buffers", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_VULKAN_CREATE_BUFFER, "Vulkan couldn't create a buffer", SSP_ERROR_SEVERITY_WARNING)   \
    X(SSP_ERROR_CODE_VULKAN_CREATE_DESCRIPTOR_POOL, "Vulkan couldn't create the descriptor pool", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_RENDERER_INIT_CAMERA, "Renderer couldn't initialise the camera", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_VULKAN_CREATE_FENCES, "Vulkan context coldn't create the fences", SSP_ERROR_SEVERITY_CRITICAL)    \
    X(SSP_ERROR_CODE_VULKAN_CREATE_TRANSFER_COPY_BUFFER_SEMAPHORE, "Vulkan context couldn't create the semaphore timeline for transfer copy buffer", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_SKIP_COPY, "Skipping copy buffer for this round", SSP_ERROR_SEVERITY_INFO) \
    X(SSP_ERROR_CODE_CREATE_OBJECT, "Couldn't create the object", SSP_ERROR_SEVERITY_WARNING)   \
    X(SSP_ERROR_CODE_TEXTURE_CREATE, "Couldn't create the texture", SSP_ERROR_SEVERITY_WARNING) \
    \
    X(SSP_ERROR_CODE_IMAGE_LOAD, "Failed to load the texture image", SSP_ERROR_SEVERITY_WARNING)

enum SSP_ERROR_CODE {
    #define X(code, str, severity) code,
    SSP_ERRORS(X)
    #undef X

    SSP_ERROR_COUNT
};

extern const char *SSP_ERROR_STR[];

enum SSP_ERROR_SEVERITY ssp_get_error_severity(enum SSP_ERROR_CODE code);

#ifdef __cplusplus
    }
#endif

#endif

#ifndef _SISYPHUS_ERRORS_H
#define _SISYPHUS_ERRORS_H

enum SSP_ERROR_SEVERITY {
    SSP_ERROR_SEVERITY_INFO = 0,
    SSP_ERROR_SEVERITY_WARNING,
    SSP_ERROR_SEVERITY_CRITICAL
};

#define SSP_ERRORS(X)  \
    X(SSP_ERROR_CODE_SUCCESS, "Success", SSP_ERROR_SEVERITY_INFO) \
    X(SSP_ERROR_CODE_UNDEFINED, "Undefined", SSP_ERROR_SEVERITY_WARNING)    \
    X(SSP_ERROR_CODE_VULKAN_LOADER_SUPPORT_INSTANCE_VERSION, "Vulkan higher instance version supported by the loader is too low", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_VULKAN_INSTANCE_LAYERS, "Vulkan instance layers required aren't available", SSP_ERROR_SEVERITY_CRITICAL)   \
    X(SSP_ERROR_CODE_VULKAN_INSTANCE_EXTENSIONS, "Vulkan instance extensions required aren't available", SSP_ERROR_SEVERITY_CRITICAL)  \
    X(SSP_ERROR_CODE_VULKAN_INSTANCE_CREATE, "Vulkan context, couldn't create VkInstance", SSP_ERROR_SEVERITY_CRITICAL)

enum SSP_ERROR_CODE {
    #define X(code, str, severity) code,
    SSP_ERRORS(X)
    #undef X

    SSP_ERROR_COUNT
};

extern const char *SSP_ERROR_STR[];

enum SSP_ERROR_SEVERITY ssp_get_error_severity(enum SSP_ERROR_CODE code);

#endif

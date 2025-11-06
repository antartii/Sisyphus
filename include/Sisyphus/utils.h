#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PTR_OFFSET(ptr, offset) ((void *)(((char *)(ptr)) + (offset)))
#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

struct SSPVersion {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
};

struct SSPDynamicArray {
    void *data;
    size_t size;
    size_t capacity;
    size_t pools_size;
    size_t elem_size;

    bool storing_ptr;
};

SSP_API struct SSPDynamicArray *ssp_dynamic_array_init(size_t pools_size, size_t elem_size, bool storing_ptr);
SSP_API void ssp_dynamic_array_free(struct SSPDynamicArray *array);
SSP_API void ssp_dynamic_array_push(struct SSPDynamicArray *array, void *data);
SSP_API void *ssp_dynamic_array_get(struct SSPDynamicArray *array, size_t index);
SSP_API bool ssp_dynamic_array_replace(struct SSPDynamicArray *array, size_t src_index, size_t dst_index);

SSP_API bool debuglog(const char *str);

#define SSP_VERSION_DEFAULT (struct SSPVersion) {0, 1, 0};

SSP_API int clamp_int(int val, int min, int max);
SSP_API char *read_file(const char *file_name, uint32_t *buffer_size);

SSP_API void ssp_build_path(char *out, size_t size, const char *path);
SSP_API char *ssp_get_executable_dir();

#ifdef __cplusplus
    }
#endif

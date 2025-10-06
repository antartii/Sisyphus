#ifndef _SISYPHUS_UTILS_H
#define _SISYPHUS_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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

struct SSPDynamicArray *ssp_dynamic_array_init(size_t pools_size, size_t elem_size, bool storing_ptr);
void ssp_dynamic_array_free(struct SSPDynamicArray *array);
void ssp_dynamic_array_push(struct SSPDynamicArray *array, void *data);
void *ssp_dynamic_array_get(struct SSPDynamicArray *array, size_t index);

#define SSP_VERSION_DEFAULT (struct SSPVersion) {0, 1, 0};

int clamp_int(int val, int min, int max);
char *read_file(const char *file_name, uint32_t *buffer_size);

#ifdef __cplusplus
    }
#endif

#endif

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "export.h"

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

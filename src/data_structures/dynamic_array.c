#include "data_structures/dynamic_array.h"

struct SSPDynamicArray *ssp_dynamic_array_init(size_t pools_size, size_t elem_size, bool storing_ptr)
{
    if (pools_size == 0 || elem_size == 0)
        return NULL;

    struct SSPDynamicArray *array = calloc(1, sizeof(struct SSPDynamicArray));

    array->pools_size = pools_size;
    array->elem_size = elem_size;
    array->capacity = pools_size;
    array->data = calloc(array->capacity, array->elem_size);
    array->storing_ptr = storing_ptr;

    return array;
}

bool ssp_dynamic_array_replace(struct SSPDynamicArray *array, size_t src_index, size_t dst_index)
{
    if (src_index >= array->size || dst_index >= array->size)
        return false;

    if (array->storing_ptr)
        ((void **) array->data)[dst_index] = ((void **) array->data)[src_index];
    else
        memcpy((char *) array->data + dst_index * array->elem_size, (char *) array->data + src_index * array->elem_size, array->elem_size);
    
    --array->size;
    return true;
}

void ssp_dynamic_array_push(struct SSPDynamicArray *array, void *data)
{
    if (array->size == array->capacity) {
        array->capacity += array->pools_size;
        array->data = realloc(array->data, array->capacity * array->elem_size);
    }

    if (array->storing_ptr)
        ((void **) array->data)[array->size] = data;
    else
        memcpy((char *) array->data + array->size * array->elem_size, data, array->elem_size);

    ++array->size;
}

void *ssp_dynamic_array_get(struct SSPDynamicArray *array, size_t index)
{
    if (array->storing_ptr)
        return (void *) ((void **) array->data)[index];
    else
        return (char *) array->data + index * array->elem_size;
}

void ssp_dynamic_array_free(struct SSPDynamicArray *array)
{
    if (!array)
        return;

    free(array->data);
    free(array);
}

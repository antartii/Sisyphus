#include "utils.h"

struct SSPDynamicArray *ssp_dynamic_array_init(size_t pools_size)
{
    if (pools_size == 0)
        return NULL;

    struct SSPDynamicArray *array = calloc(1, sizeof(struct SSPDynamicArray));

    array->pools_size = pools_size;
    array->data = calloc(pools_size, sizeof(void *));
    array->capacity = pools_size;

    return array;
}

void ssp_dynamic_array_push(struct SSPDynamicArray *array, void *data)
{
    if (array->size == array->capacity) {
        array->capacity += array->pools_size;
        array->data = realloc(array->data, array->capacity * sizeof(void *));
    }
    array->data[array->size++] = data;
}
void ssp_dynamic_array_free(struct SSPDynamicArray *array)
{
    free(array->data);
    free(array);
}

int clamp_int(int val, int min, int max)
{
    return val < min ? min : (val > max ? max : val);
}

char *read_file(const char *file_name, uint32_t *buffer_size)
{
    FILE *file = fopen(file_name, "rb");
    *buffer_size = 0;
    char *buffer = NULL;

    if (file) {
        fseek(file, 0, SEEK_END);
        *buffer_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer = malloc(sizeof(char) * (*buffer_size));
        if (buffer)
            fread(buffer, 1, *buffer_size, file);
        fclose(file);
    }
    #ifdef DEBUG 
    else
        printf("Couldn't open the file %s\n", file_name);
    #endif

    return buffer;
}

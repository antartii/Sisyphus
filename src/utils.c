#include "utils.h"

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

char *ssp_get_executable_dir()
{
    static char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, PATH_MAX - 1);
    
    if (len == -1)
        return NULL;
    
    path[len] = '\0';
    dirname(path);

    return path;
}

void ssp_build_path(char *out, size_t size, const char *path)
{
    snprintf(out, size, "%s/%s", ssp_get_executable_dir(), path);
}

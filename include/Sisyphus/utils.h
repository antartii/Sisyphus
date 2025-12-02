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
#include <math.h>
#include <cglm/cglm.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#include "export.h"
#include "data_structures/dynamic_array.h"
#include "data_structures/indexed_linked_list.h"

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

SSP_API bool debuglog(const char *str);

#define SSP_VERSION_DEFAULT (struct SSPVersion) {0, 1, 0};

SSP_API int clamp_int(int val, int min, int max);
SSP_API char *ssp_read_file(const char *file_name, uint32_t *buffer_size);

SSP_API void ssp_build_path(char *out, size_t size, const char *path);
SSP_API char *ssp_get_executable_dir();

float get_angle_from_distances(float a, float b, float c);
float distance(float x1, float y1, float x2, float y2);
bool is_point_in_triangle(vec2 point, vec2 a, vec2 b, vec2 c);
float distance_from_segment(vec2 p, vec2 a, vec2 b);
float dot_vec2(const vec2 a, const vec2 b);

#ifdef __cplusplus
    }
#endif

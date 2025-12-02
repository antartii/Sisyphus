#include "utils.h"

float distance(float x1, float y1, float x2, float y2)
{
    float x = x2 - x1;
    float y = y2 - y1;

    return sqrt((x * x) + (y * y));
}

float get_angle_from_distances(float a, float b, float c)
{
    // al kashi theorem
    return acos((b * b) + (c * c) - (a * a) / (2 * b * c));
}

float vec2_orientation(vec2 a, vec2 b, vec2 c)
{
    vec2 ab;
    vec2 ac;

    glm_vec2_sub(b, a, ab);
    glm_vec2_sub(c, a, ac);
    return glm_vec2_cross(ab, ac);
}

float dot_vec2(const vec2 a, const vec2 b)
{
    return a[0] * b[0] + a[1] * b[1];
}

float distance_from_segment(vec2 p, vec2 a, vec2 b)
{
    vec2 ab = {b[0] - a[0], b[1] - a[1]};
    vec2 ap = {p[0] - a[0], p[1] - a[1]};

    float ab_len2 = dot_vec2(ab, ab);
    if (ab_len2 == 0.0f)
        return glm_vec2_distance2(p, a);

    float t = dot_vec2(ap, ab) / ab_len2;
    if (t < 0.0f)
        t = 0.0f;
    if (t > 1.0f)
        t = 1.0f;

    vec2 proj = {
        a[0] + t * ab[0],
        a[1] + t * ab[1]
    };

    return glm_vec2_distance2(p, proj);
}

bool is_point_in_triangle(vec2 point, vec2 a, vec2 b, vec2 c)
{
    float c1 = vec2_orientation(a, b, point);
    float c2 = vec2_orientation(b, c, point);
    float c3 = vec2_orientation(c, a, point);

    const float epsilon = 0.000001;

    bool has_neg = (c1 < -epsilon) || (c2 < -epsilon) || (c3 < -epsilon);
    bool has_pos = (c1 >  epsilon) || (c2 >  epsilon) || (c3 >  epsilon);

    return !(has_neg && has_pos);
}

int clamp_int(int val, int min, int max)
{
    return val < min ? min : (val > max ? max : val);
}

char *ssp_read_file(const char *file_name, uint32_t *buffer_size)
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

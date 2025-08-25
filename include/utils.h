#ifndef _SISYPHUS_UTILS_H
#define _SISYPHUS_UTILS_H

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(A[0]))

struct Version {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
};

#endif

#ifndef _CONFIG_H
#define _CONFIG_H

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SSPConfig {
    char *app_name;
    struct SSPVersion app_version;
    char *window_title;
};

#ifdef __cplusplus
    }
#endif

#endif

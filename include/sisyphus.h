#pragma once

    #include "sisyphus_config.h"
    #include "engine.h"
    #include "config.h"
    #include "object.h"
    #include "texture.h"

    #include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Printing in stdout the version, the description, and the current surface used by the Sisyphus library
 */
SSP_API void ssp_library_print_info(void);

#ifdef __cplusplus
    }
#endif

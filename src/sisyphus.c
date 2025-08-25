#include "sisyphus.h"

void SSPlibrary_print_info(void)
{
    printf("%s - %s\n", SSP_ENGINE_NAME, SSP_DESCRIPTION);
    printf("\tversion: %d.%d.%d\n", SSP_VERSION_MAJOR, SSP_VERSION_MINOR, SSP_VERSION_PATCH);
    printf("\tcurrent surface api: %s\n", SSP_SURFACE_API);
    printf("\tgithub repository: %s\n", SSP_HOMEPAGE_URL);
}

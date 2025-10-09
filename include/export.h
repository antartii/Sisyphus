#ifndef _SISYPHUS_EXPORT_H
#define _SISYPHUS_EXPORT_H

// for linux only
#define SSP_API __attribute__((visibility("default")))
#define SSP_INTERNAL __attribute__((visibility("hidden")))

#endif

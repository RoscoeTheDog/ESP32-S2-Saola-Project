
#include "nvs_flash.h"

extern void nvsWriteBlob(const char *location, const char *key, void *args, size_t size);

extern void* nvsReadBlob(const char *location, const char *key, size_t size);

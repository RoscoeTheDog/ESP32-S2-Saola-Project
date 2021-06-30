
#include "nvs_flash.h"

extern void nvsWriteBlob(char *location, char *key, void *args, size_t size);

extern void* nvsReadBlob(char *location, char *key, size_t size);

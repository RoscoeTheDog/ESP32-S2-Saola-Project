#include "nvs_flash.h"
#include <globals.h>
#include <string.h>
#include <stdio.h>
#include <cJSON.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>

extern esp_err_t nvsRestoreSystemState();

extern void nvsWriteBlob(const char *location, const char *key, void *args, size_t size);

extern void* nvsReadBlob(const char *location, const char *key, size_t size);

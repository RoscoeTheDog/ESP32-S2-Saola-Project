#include <esp_wifi.h>
#include <esp_http_client.h>
#include <settings.h>

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

extern void prototypeHttp();

extern void wifi_init_softtap(void);

extern void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);


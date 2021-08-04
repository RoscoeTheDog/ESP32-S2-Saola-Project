#include <esp_wifi.h>
#include <esp_http_client.h>
#include <globals.h>

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include <freertos/event_groups.h>
#include <esp_smartconfig.h>
#include <configNvs.h>
#include <esp_freertos_hooks.h>

#include <esp_system.h>

#include <rtosTasks.h>

#include "lwip/err.h"
#include "lwip/sys.h"
#include <esp_err.h>
#include <esp_system.h>
#include <esp_event.h>
#include <configNvs.h>

extern volatile bool WIFI_CONNECTED;
extern wifi_config_t wifi_config;

/* FreeRTOS event group to signal when we are connected & ready to make a request */
extern EventGroupHandle_t s_wifi_event_group;

extern void vTaskSmartConfig(void * parm);

extern void updateWifiConfig();

extern void prototypeHttp();

extern void vInitTaskSmartConfig(void *pvParameters);

extern void vTaskSmartConfig(void * pvParameters);

extern void wifi_init_softtap(void);

extern void initialize_wifi(void);

extern void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);


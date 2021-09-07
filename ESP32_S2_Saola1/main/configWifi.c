#include <configWifi.h>
#include <httpRequests.h>
#include <freertos/task.h>
#include <freertos/FreeRTOS.h>

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
wifi_config_t wifi_config;

/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t s_wifi_event_group;
TaskHandle_t xHandleNVSConnect;

volatile bool WIFI_CONNECTED = false;

void vTaskNVSConnect() {
    char *TAG = "vTaskNVSConnect";

    while (1) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    
        wifi_config_t *wifi_config = NULL;
        wifi_config = nvsReadBlob("wifi_settings", "wifi_config_t", sizeof(wifi_config_t)); // returns NULL if failed.

        esp_err_t err = ESP_OK;
        while (wifi_config != NULL && err == ESP_OK) {
            ESP_LOGI(TAG, "saved wifi config found!\n");
            ESP_LOGI(TAG, "SSID: %s", (char*)wifi_config->sta.ssid);
            ESP_LOGI(TAG, "PASSWORD: %s", (char*)wifi_config->sta.password);
            ESP_LOGI(TAG, "BSSID MAC: %s",(char*)wifi_config->sta.bssid);
            
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        }
    }
    
}

void wifiConfigNVSConnect() {
    char *TAG = "wifiConfigNVSConnect";
    ESP_LOGD(TAG, "TASK HEAP REMAINING: %i", xPortGetFreeHeapSize());
    wifi_config_t *wifi_config = NULL;
    wifi_config = nvsReadBlob("wifi_settings", "wifi_config_t", sizeof(wifi_config_t)); // returns NULL if failed.
    // wifi_config_t *w_new = (wifi_config_t*)malloc(sizeof(wifi_config_t));

    // assert(w_new);
    // assert(memcpy(w_new->sta.ssid, wifi_config->sta.ssid, sizeof(wifi_config->sta.ssid)));
    // assert(memcpy(w_new->sta.password, wifi_config->sta.password, sizeof(wifi_config->sta.ssid)));

    esp_err_t err = ESP_OK;
    while (wifi_config != NULL && err == ESP_OK) {
        ESP_LOGI(TAG, "Previous wifi configuration found.");
        ESP_LOGI(TAG, "SSID: %s\nPASSWORD: %s", wifi_config->sta.ssid, wifi_config->sta.password);
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    }

}

void updateWifiConfig() {
    char *TAG = "updateWifiConfig";
    ESP_LOGI(TAG, "active wifi config updated");
    memcpy(&wifi_config.sta.ssid, WIFI_SSID, sizeof(char) * 32);
    memcpy(&wifi_config.sta.password, WIFI_PASSWORD, sizeof(char) * 64);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
}

void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    char *TAG = "event_handler";

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // xTaskNotify(xHandleWifiReconnect, 1, eSetValueWithOverwrite);
        // if (s_wifi_event_group && xEventGroupGetBits(ESPTOUCH_DONE_BIT)) {
        // vTaskSmartConfig(NULL);
        // }
        // if (s_wifi_event_group && eTaskGetState(s_wifi_event_group) != eRunning || eTaskGetState(s_wifi_event_group) != eReady) {
        //     xTaskCreate(vTaskSmartConfig, "vTaskSmartConfig", 4096, NULL, 22, NULL);
        // }
        // xTaskCreate(vTaskNVSConnect, "NVSConnect", 4096, NULL, 1, xHandleNVSConnect);
        // vTaskDelay(pdMS_TO_TICKS(500));
        // xTaskNotifyFromISR(xHandleNVSConnect, 1, eSetValueWithOverwrite, NULL);
        wifiConfigNVSConnect();

    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        // update event bit locks
        WIFI_CONNECTED = true;
        HTTP_ERROR = false;
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
        // ESP_ERROR_CHECK(esp_smartconfig_stop());

        // begin the datetime sync service if not already
        initializeSntpUpdate();

        // Begin polling of web server to fetch new data
        // if (xHandlePollServer) {
        //     xTaskCreate(vTaskPollServer, "vTaskPollServer", 4096, NULL, 10, &xHandlePollServer);
        // }
        // if (xHandlePollServer) {
        //     // if (eTaskGetState(xHandlePollServer) == eSuspended) {
        //     //     vTaskResume(xHandlePollServer);
        //     // }

        //     xTaskNotify(xHandlePollServer, 1, eSetValueWithoutOverwrite);
        // }
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi Disconnected from ap");
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        WIFI_CONNECTED = false;
        HTTP_ERROR = false;

        // vTaskSmartConfig();

        // if (xHandlePollServer && (eTaskGetState(xHandlePollServer) == eRunning || eTaskGetState(xHandlePollServer) == eRunning)) {
        //     xTaskNotify(xHandlePollServer, 0, eSetValueWithOverwrite);
        // }
        // if (xHandleWifiReconnect) {
        //     xTaskNotify(xHandleWifiReconnect, 1, eSetValueWithOverwrite);
        // }

        // xTaskCreate(vTaskWifiReconnect, "vTaskWifiReconnect", 4096, NULL, 22, &xHandleWifiReconnect);
        // xTaskNotify(xHandleWifiReconnect, 1, eSetValueWithOverwrite);
    
        
        // if (eTaskGetState(s_wifi_event_group) != eRunning) {
        //     xTaskCreate(vTaskSmartConfig, "smartconfig_example_task", 4096, NULL, 1, NULL);
        // }
    
        // esp_register_freertos_idle_hook(vTaskIdleHook);
        // printf("starting smartconfig...\n");
        // xTaskCreate(vTaskSmartConfig, "smartconfig_example_task", 4096, NULL, 1, NULL);
        // vTaskDelay(pdMS_TO_TICKS(5000));
        // printf("Trying last known WiFi configuration...\n");
        // wifiConfigNVSConnect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
        WIFI_CONNECTED = true;
        HTTP_ERROR = false;
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;

        // WIFI_SSID = evt->ssid;
        // WIFI_PASSWORD = evt->password;
        // // bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(WIFI_SSID, evt->ssid, sizeof(char) * 32);
        memcpy(WIFI_PASSWORD, evt->password, sizeof(char) * 64);
        // wifi_config.sta.bssid_set = evt->bssid_set;
        
        // if (wifi_config.sta.bssid_set == true) {
        //     memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        // }
        
        ESP_LOGI(TAG, "SSID: %s", WIFI_SSID);
        ESP_LOGI(TAG, "PASSWORD: %s", WIFI_PASSWORD);
        updateWifiConfig();

        // esp_smartconfig_stop();
        // wifi_config.sta.listen_interval = 3;

        // attempt to connect to the AP
        // esp_wifi_connect();
        // if connected to the AP OK, then write the new config to the NVS
        // Otherwise, report the error.
        // if (err == ESP_OK) {
        //     // write to the nvs the wifi configuration
        //     nvsWriteBlob("wifi_settings", "wifi_config_t", &wifi_config, sizeof(wifi_config_t));
        //     // nvsWriteBlob("wifi_settings", "wifi_ssid", &wifi_config.sta.ssid, sizeof(char) * 32);
        //     // nvsWriteBlob("wifi_settings", "wifi_password", &wifi_config.sta.password, sizeof(char) * 64);
        // } else {
        //     printf("Could not connect using smartConfig. Error code (%i)\n", err);
        // }
        
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
        vTaskSmartConfig(NULL);
    }
}

void vInitTaskSmartConfig(void * pvParameters) {
    ;
    // ESP_ERROR_CHECK(esp_smartconfig_stop());
    // ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    // smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    // vTaskSmartConfig(NULL);
}

void vTaskSmartConfig(void * pvParameters) {
    char *TAG = "vTaskSmartConfig";
    EventBits_t uxBits;
    // if (s_wifi_event_group && (eTaskGetState(s_wifi_event_group) == eRunning || eTaskGetState(s_wifi_event_group) == eReady) ) {
    //     xEventGroupClearBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    // }
    // TaskHandle_t localTask = xTaskGetCurrentTaskHandle();
    // vTaskPrioritySet(localTask, 22);
    ESP_ERROR_CHECK(esp_smartconfig_stop());
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    // while(1) {
    //     uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
    //     if(CONNECTED_BIT) {
    //         ESP_LOGI(TAG, "WiFi Connected to ap");
    //     }
    //     if(ESPTOUCH_DONE_BIT) {
    //         ESP_LOGI(TAG, "smartconfig over");
    //         esp_smartconfig_stop(); 
    //         vTaskDelete(NULL);
    //     }
    // }
}

// start the radio and attempt to connect with last known configuration
void initializeWifi(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    // TaskHandle_t handle;
    // char *name = "wifi";
    // handle = xTaskGetHandle("wifi");
    // vTaskPrioritySet(&handle, 1);

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // initialize the NVS flash
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    updateWifiConfig();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );

    // ESP_ERROR_CHECK( esp_wifi_set_ps(WIFI_PS_MAX_MODEM));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );

    // // if successful or fail, an event will be raised in the event_handler callback.
    // // if fails, it will automatically attempt to find the last known config in the NVS and try again.
    // esp_wifi_connect();

}

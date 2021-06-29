/* Esptouch example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"


/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "smartconfig_example";

static void smartconfig_example_task(void * parm);

void nvsReadWifiConfig() {
        // declare a nvs handler and our wifi config to restore.
        nvs_handle_t storage_handle;
        // wifi_config_t wifi_config;
        wifi_config_t wifi_config_copy;
        size_t nvs_buffer = sizeof(wifi_config_t);
        // open the wifi_settings namespace with read/write permissions, passing in the handler.
        ESP_ERROR_CHECK(nvs_open("wifi_settings", NVS_READWRITE, &storage_handle));
        // ESP_ERROR_CHECK(nvs_set_blob(storage_handle, "wifi_config_t", &wifi_config, sizeof(wifi_config_t)));
        // ESP_ERROR_CHECK(nvs_get_blob(storage_handle, "wifi_config_t", &wifi_config_copy, &nvs_buffer));

        ESP_ERROR_CHECK(nvs_get_blob(storage_handle, "wifi_config_t", &wifi_config_copy, &nvs_buffer));
        if (nvs_get_blob(storage_handle, "wifi_config_t", &wifi_config_copy, &nvs_buffer) == ESP_OK) {
            printf("ESP_OK\n");
            printf("Data: %s %s\n", wifi_config_copy.sta.ssid, wifi_config_copy.sta.password);
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_copy));
            ESP_ERROR_CHECK(esp_wifi_connect());
        } 

}

static void vTaskCheckConnection(void * parm) {

    while(true) {
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("Checking WiFi status\n");
        // declare a nvs handler and our wifi config to restore.
        nvs_handle_t storage_handle;
        // wifi_config_t wifi_config;
        wifi_config_t wifi_config_copy;
        size_t nvs_buffer = sizeof(wifi_config_t);
        // open the wifi_settings namespace with read/write permissions, passing in the handler.
        ESP_ERROR_CHECK(nvs_open("wifi_settings", NVS_READWRITE, &storage_handle));
        // ESP_ERROR_CHECK(nvs_set_blob(storage_handle, "wifi_config_t", &wifi_config, sizeof(wifi_config_t)));
        // ESP_ERROR_CHECK(nvs_get_blob(storage_handle, "wifi_config_t", &wifi_config_copy, &nvs_buffer));

        if (nvs_get_blob(storage_handle, "wifi_config_t", &wifi_config_copy, &nvs_buffer) == ESP_OK) {
            printf("ESP_OK\n");
            printf("Data: %s %s\n", wifi_config_copy.sta.ssid, wifi_config_copy.sta.password);
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_copy));
            ESP_ERROR_CHECK(esp_wifi_connect());
        } 

        // ESP_ERROR_CHECK(nvs_get_blob(storage_handle, "wifi_config_t", &k, &z));
        // printf("RESULTING VALUE: %s\n", wifi_config_copy.sta.ssid);
        // if ( nvs_get_blob(storage_handle, "wifi_config_t", readTest, sizeof(char) * (15)) != ESP_ERR_NVS_NOT_FOUND) {
        //     printf("saved ssid: %s saved password: %s\n", wifi_config.sta.ssid, wifi_config.sta.password);
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        // }
    }
    

}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        printf("station connected!\n");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("station disconnected!\n");
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        nvsReadWifiConfig();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);

        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        esp_wifi_connect();

        nvs_handle_t storage_handle;
        esp_err_t err = nvs_open("wifi_settings", NVS_READWRITE, &storage_handle);
        nvs_set_blob(storage_handle, "wifi_config_t", &wifi_config, sizeof(wifi_config));
        err = nvs_commit(storage_handle);
        nvs_close(storage_handle);

        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_ERROR_CHECK(esp_wifi_disconnect());

    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

static void initialize_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    esp_wifi_connect();
}

static void smartconfig_example_task(void * parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if(uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}

void app_main(void)
{

    // initialize NV flash memory storage
    nvs_flash_erase();
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // initialize the wifi (using smartconfig)
    initialize_wifi();

    // // create periodic task that checks for wifi connection.
    // TaskHandle_t xHandleCheckConnection;    
    // assert(xTaskCreate(vTaskCheckConnection, "Check Wifi", 2048, NULL, 1, &xHandleCheckConnection));



}

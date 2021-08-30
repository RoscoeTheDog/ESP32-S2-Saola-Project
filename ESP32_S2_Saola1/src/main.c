/*
	Include C++ headers here.

	If you use C++ libraries, they should be included in this CPP scope OR explicitely wrapped as extern "C++" {} so the compiler knows what conventions to use
	and how to link them together.
*/

/*
	Tell the compiler to wrap main for C convention since most of the IDF framework in standard C but we may want to use C++ occasionally.
*/

#ifdef __cplusplus
extern "C" {
#endif
	// Include C-headers here.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <esp_timer.h>
	#include <freeRTOS/FreeRTOS.h>

	#include <configGpio.h>
	#include <configLedc.h>
	#include <configTimers.h>
	#include <configSteppers.h>
	#include <configWifi.h>
	#include <httpRequests.h>
	#include <esp_err.h>
	#include <string.h>
	#include <esp_sleep.h>
	#include <esp_freertos_hooks.h>
	#include <freertos/task.h>
	#include <esp_sntp.h>
	#include <sys/time.h>
	#include <time.h>
	#include <globals.h>
	#include <esp_pm.h>

	#include <cJSON.h>
	#include <cJSON_Utils.h>

    void app_main(void);

#ifdef __cplusplus
}
#endif

TaskHandle_t xHandleTest;

void test(void *args) {

	char *TAG = "test";

	while (1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		esp_task_wdt_add(xHandleTest);
		esp_task_wdt_reset();
		esp_task_wdt_delete(xHandleTest);
		ESP_LOGI(TAG, "TEST TASK RUNNING");
		xTaskNotify(xHandleTest, 1, eSetValueWithoutOverwrite);
	}

}

// void vTaskSmartConfig(void *args) {
// 	char *TAG = "vTaskSmartConfig";
// 	EventBits_t uxBits;

// 	while(1) {
// 		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

// 		while (!ESP_SMARTCONFIG_STATUS) {
// 			if (!RADIO_INITIALIZED) {
// 				initializeWifi();
// 			}
// 			ESP_LOGI(TAG, "configuring smartconfig service");
// 			ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
// 			smartconfig_start_config_t cfg = {.enable_log = false};

// 			ESP_LOGI(TAG, "starting smartconfig service");
// 			esp_err_t err = esp_smartconfig_start(&cfg);
// 			ESP_ERROR_CHECK(err);
// 			if (err == ESP_OK) {
// 				ESP_SMARTCONFIG_STATUS = true;
// 				ESP_LOGI(TAG, "smartconfig service started successfully");
// 			} else {
// 				ESP_LOGI(TAG, "smartconfig service failed to initialize");
// 			}
			
// 			vTaskDelay(1);			
// 		}

// 		while (1) {
// 			uxBits = xEventGroupWaitBits(s_wifi_event_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);

// 			if(uxBits & ESPTOUCH_DONE_BIT) {
// 				ESP_LOGI(TAG, "smart config completed -- stopping service.");
// 				esp_smartconfig_stop();
// 				ESP_SMARTCONFIG_STATUS = false;
// 				xEventGroupClearBits(xHandleSmartConfig, ESPTOUCH_DONE_BIT);
// 			}
// 			vTaskDelay(1);
// 		}

// 	}
// }

void app_main(void) {
	// nvs_flash_erase();
	esp_task_wdt_init(10, false);

	// configure the JSON library to use FREERTOS thread safe malloc / free methods
    cJSON_Hooks hooks;
    hooks.malloc_fn = pvPortMalloc;
    hooks.free_fn = vPortFree;
    cJSON_InitHooks(&hooks);

	// esp_pm_config_esp32s2_t cfg;
	// esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
	// cfg.light_sleep_enable = true;
	// cfg.max_freq_mhz = 240;
	// cfg.min_freq_mhz = 80;

	/* *************initialize peripherials and resources here****************/
	// configure gpio for peripiherials first
	vInitGpioConfig();					// GPIO pin configuration
	// cold start go status red as soon as GPIO is initialized.
	setStatusLEDRed();
	// continue init...
	initialize_ledc_config_0();				// LED PWM generator
	vInitCurtainMotorConfig_0();		// Primary Motor Control	
	initializeTimerConfig();				// Button interrupts

	// pause for visual indication
	vTaskDelay(pdMS_TO_TICKS(2000));
	if (nvsRestoreSystemState() == ESP_OK) {
		// blink green twice if restored state sucessfully
		for (short i = 0; i < 2; ++i) {
			setStatusLEDGreen();
			vTaskDelay(pdMS_TO_TICKS(100));
			setStatusLEDRed();
			vTaskDelay(pdMS_TO_TICKS(100));
		}
		// flag sync will bypass the parsing of last command from server.
		SYS_SYNC = true;
	}
	vTaskDelay(pdMS_TO_TICKS(1000));

	// system hardware initialized sucessfully. go status yellow
	setStatusLEDYellow();

	strcpy(WIFI_SSID, "wutangLAN");
	strcpy(WIFI_PASSWORD, "c@$T131nTh3$Ky");
	
	// portENTER_CRITICAL(&mux);
	// initializeHttpClient();
	// portEXIT_CRITICAL(&mux);
	// CURTAIN_PERCENTAGE = 0;
	// MOTOR_POSITION_STEPS = 0;

	initializeRTOSTasks();
	vTaskDelay(pdMS_TO_TICKS(1000));
	// initializeWifi();
	// // esp_wifi_connect();

	// initializeHttpClient();



	// cJSON *object = cJSON_CreateObject();
	// cJSON_AddNumberToObject(object, "CURTAIN_PERCENTAGE", CURTAIN_PERCENTAGE);
	// cJSON_AddNumberToObject(object, "CURTAIN_PERCENTAGE_2", 42);
	// cJSON_AddNumberToObject(object, "CURTAIN_PERCENTAGE_3", (double)CURTAIN_PERCENTAGE);
	// CURTAIN_PERCENTAGE = 24.7584;
	// cJSON_AddNumberToObject(object, "CURTAIN_PERCENTAGE_4", CURTAIN_PERCENTAGE);

	// char *str = cJSON_PrintUnformatted(object);
	// ESP_LOGI("SYSTEM", "cJSON: %s", str);
	// cJSON_free(str);
	// cJSON_Delete(object);
}


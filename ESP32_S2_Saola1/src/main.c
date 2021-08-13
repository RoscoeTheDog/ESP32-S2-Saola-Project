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
	#include <esp_pm.h>
	#include <esp_sleep.h>
	#include <esp_freertos_hooks.h>
	#include <freertos/task.h>
	#include <esp_sntp.h>
	#include <sys/time.h>
	#include <time.h>

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

void app_main(void) {
	nvs_flash_erase();

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

	// ESP_ERROR_CHECK(esp_pm_configure(&cfg));
	// vInitTaskSleep();
	// vInitTaskRTOSDebug();
	// initialize peripherials and resources here.

	// configure gpio for peripiherials first
	vInitGpioConfig();					// GPIO pin configuration
	// cold start go status red asap.
	setStatusLEDRed();
	// continue init...
	initialize_ledc_config_0();				// LED PWM generator
	vInitCurtainMotorConfig_0();		// Primary Motor Control	
	initializeTimerConfig();				// Button interrupts

	// pause for visual indication
	vTaskDelay(pdMS_TO_TICKS(2000));

	// system hardware initialized sucessfully. go status yellow
	setStatusLEDYellow();

	// initialize rtos tasks
	initializeTasks();

	// pause for visual indication
	vTaskDelay(pdMS_TO_TICKS(2000));

	// status gets updated in wifi event handler
	initialize_wifi();

	// xTaskCreate(test, "test", 2048, NULL, 25, &xHandleTest);
	// configASSERT(xHandleTest);

	// while (1) {
	// 	while (eTaskGetState(xHandleTest) != eRunning) {

	// 		ESP_LOGI("main", "notifying test task");
	// 		xTaskNotify(xHandleTest, 1, eSetValueWithOverwrite);

	// 		for (int i = 0; i < 10; i++) {
	// 			ESP_LOGI("main", "DOING STUFF IN LOOP");
	// 		}
	// 	}
	// 	ESP_LOGI("main", "WHILE LOOP EXITED");
	// 	vTaskDelay(pdMS_TO_TICKS(1000));
	// }


	// xTaskNotify(xHandleRTOSDebug, 1, eSetValueWithOverwrite);

	// while (1) {

	// 	// if (xHandleTask1 != NULL && eTaskGetState(xHandleTask1) == eSuspended) {
	// 	// 	printf("Resuming Task\n");
	// 	// 	vTaskResume(xHandleTask1);
	// 	// }

	// 	// if (xHandleTask1 == NULL) {
			
	// 		// if (eTaskGetState(xHandleTask1) == eDeleted) {
	// 			printf("Creating Task\n");
	// 			xTaskCreate(task_1, "task_1", 2048, NULL, 25, &xHandleTask1);
	// 		// }
			
	// 	// }
	
	// 	// if (xHandleTask1 != NULL) {
	// 		// if (eTaskGetState(xHandleTask1) == eDeleted) {
	// 			// printf("Creating Task\n");
	// 			// // vTaskResume(xHandleTask1);
	// 			// xTaskCreate(task_1, "task_1", 2048, NULL, 25, &xHandleTask1);
	// 			// assert(xHandleTask1);
	// 		// }
	// 	// } 
	// 	// else {
	// 	// 	printf("Creating Task\n");
	// 	// 	vTaskResume(xHandleTask1);
	// 	// 	// xTaskCreate(task_1, "task_1", 2048, NULL, 25, xHandleTask1);
	// 	// 	configASSERT(xHandleTask1);
	// 	// }

		
	// 	// if (eTaskGetState(xHandleTask1) == eReady) {
	// 		printf("Notifying Task\n");
	// 		xTaskNotify(xHandleTask1, 1, eSetValueWithoutOverwrite);
	// 	// }
	// 	// // vTaskDelay(pdMS_TO_TICKS(100));
	// 	// // printf("Suspending Task\n");
	// 	// // vTaskSuspend(xHandleTask1);
	// 	// // vTaskDelay(pdMS_TO_TICKS(100));
	// 	// // printf("Resuming Task\n");
	// 	// // vTaskResume(xHandleTask1);


	// 	vTaskDelay(pdMS_TO_TICKS(5));
	// 	// if (xHandleTask1 != NULL) {
	// 	// 	printf("Deleting Task\n");
	// 	// 	vTaskDelete(xHandleTask1);
	// 	// }

	// 	printf("Suspending task\n");
	// 	vTaskDelete(xHandleTask1);
		
	// 	// xTaskNotify(xHandleTask1, 0, eSetValueWithOverwrite);

	// 	vTaskDelay(pdMS_TO_TICKS(3000));

	// }


	

	// nvsWriteBlob("wifi_settings", "wifi_ssid", "wuntangLAN", sizeof(char) * 32);
	// char *c = malloc(sizeof(char) * 32);
	// memcpy(c, nvsReadBlob("wifi_settings", "wifi_ssid", sizeof(char) * 32), sizeof(char) * 32);
	// // char *c = nvsReadBlob("wifi_settings", "wifi_ssid", sizeof(char) * 11);
	// printf("%s\n", c); 

	// initialize the NVS.
    // esp_err_t err = nvs_flash_init();
    // if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //     // NVS partition was truncated and needs to be erased
    //     // Retry nvs_flash_init
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     err = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK( err );

	// wifi_config_t foo = {
	// 	.sta = {.ssid = "wutangLan",
	// 			.password = "test"},
	// };
	
	// nvsWriteBlob("wifi_settings", "wifi_config_t", &foo, sizeof(wifi_config_t) );

	// wifi_config_t *bar = NULL;
	// bar = nvsReadBlob("wifi_settings", "wifi_config_t", sizeof(wifi_config_t));
	
	// printf("bar: %s\n", bar->sta.ssid);

	// declare a nvs handler and our wifi config to restore.
	// nvs_handle_t storage_handle;
	// void *nvs_data = (void*)malloc(sizeof(void*));
	// size_t nvs_buffer = sizeof(a);
	// // open the wifi_settings namespace with read/write permissions, passing in the handler.
	// ESP_ERROR_CHECK(nvs_open("wifi_settings", NVS_READWRITE, &storage_handle));
	// ESP_ERROR_CHECK(nvs_get_blob(storage_handle, "wifi_config_t", &nvs_data, &nvs_buffer));
	// printf("value: %i\n", (int)nvs_data);
	// size_t t = sizeof(int);
	// wifi_config_t *b = (wifi_config_t*)nvsReadBlob("wifi_settings", "wifi_config_t", sizeof(wifi_config_t));

	// wifi_config_t bar = {
	// 	.sta = {
	// 		.ssid = "wutanglan",
	// 	}
	// };
	// printf("value b: %s\n", bar.sta.ssid);
	// printf("value b: %s\n", b->sta.ssid);

	// printf("stop");
	// if (b == NULL) {
	// 	printf("b is null\n");
	// } else {
	// 	printf("%i\n", *b);	
	// }


	// xTaskNotify(xHandleCloseCurtains, 1, eSetValueWithOverwrite);

	// // rotate the motor 360 degrees then wait for 3 seconds.
	// while (1) {
	// 	rotate(StepperMotor_1, 360);
	// 	vTaskDelay(pdMS_TO_TICKS(3 * 1000));
	// }

	// xTaskNotify(xHandleRTOSDebug, 1, eSetValueWithOverwrite);
	// printf("heap_caps_get_free_size: %u\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
	// printf("xPortGetFreeHeapSize: %u\n\n", xPortGetFreeHeapSize());

}


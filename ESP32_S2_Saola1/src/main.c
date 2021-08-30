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

void app_main(void) {
	// nvs_flash_erase();
	esp_task_wdt_init(10, false);

	// configure the JSON library to use FREERTOS thread safe malloc / free methods
    cJSON_Hooks hooks;
    hooks.malloc_fn = pvPortMalloc;
    hooks.free_fn = vPortFree;
    cJSON_InitHooks(&hooks);

	/* *************initialize peripherials and resources here****************/
	// configure gpio for peripiherials first
	vInitGpioConfig();					// GPIO pin configuration
	// cold start go status red as soon as GPIO is initialized.
	setStatusLEDRed();
	// continue init...
	initialize_ledc_config_0();				// LED PWM generator
	vInitCurtainMotorConfig_0();		// Primary Motor Control	
	initializeTimerConfig();				// Button interrupts

	// pause for LED visual indication
	vTaskDelay(pdMS_TO_TICKS(2000));

	// begin restoring the system's values from whatever it's previous state was.
	if (nvsRestoreSystemState() == ESP_OK) {
		// blink green twice if restored state sucessfully
		for (short i = 0; i < 2; ++i) {
			setStatusLEDGreen();
			vTaskDelay(pdMS_TO_TICKS(100));
			setStatusLEDRed();
			vTaskDelay(pdMS_TO_TICKS(100));
		}
		// flag sync will bypass the parsing of last command from server for the motor.
		SYS_SYNC = true;
	}

	// pause so that the LED does not instantly change colors
	vTaskDelay(pdMS_TO_TICKS(1000));

	// system hardware initialized sucessfully. go status yellow
	setStatusLEDYellow();

	// put in SSID/PWORD here for testing.
	strcpy(WIFI_SSID, "...");
	strcpy(WIFI_PASSWORD, "...");

	// call rtos initializers. note some tasks are initialized in a blocking state.
	initializeRTOSTasks();
	vTaskDelay(pdMS_TO_TICKS(1000));

	// RTOS task "vTaskWifiReconnect" runs endlessly as a service and reconnects if ap ever disconnected.
	// the new update causes this task/service to block or/and trip the task wdt watchdog, wheras previously in
	// 4.2.1 this was not an issue.
	
	// you can manually initialize the wifi by invoking this initializer. Otherwise, the RTOS service task will
	// automatically configure and initializer the radio if the "RADIO_INITIALIZED" flag is not set to true..
	
	if (!RADIO_INITIALIZED) {
		initializeWifi();
	}
}


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

    void app_main(void);

#ifdef __cplusplus
}
#endif

void app_main(void) {
	nvs_flash_erase();

	// // initialize peripherials and resources here.
	// vInitGpioConfig();					// GPIO pin configuration
	// vInitLedcConfig_0();				// LED PWM generator
	// vInitTimerConfig_0();				// Button interrupts
	// vInitCurtainMotorConfig_0();

	// // initialize RTOS tasks here
	// vInitTaskLEDFade();					// Task to fade the LED buttons
	// vInitTaskCurtainMotor();			// Multiple Tasks to control the stepper motor

	// initialize_wifi();&

	    // initialize the NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

	int a = 17;
	nvsWriteBlob("wifi_settings", "wifi_config_t", &a, sizeof(int) );
	int * b = nvsReadBlob("wifi_settings", "wifi_config_t", sizeof(int));
	printf("stop");
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


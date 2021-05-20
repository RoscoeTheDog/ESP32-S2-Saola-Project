#include <rtosTasks.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
#include <StepperDriver.h>
#include <configSteppers.h>
/*
	Define all of your callback functions here.

	Note: no function should ever return anything. Make use of blocks or simply uninstall the task when you want it to stop.
*/ 

TaskHandle_t xHandleRTOSDebug = NULL;
TaskHandle_t xHandleLEDFade = NULL;
TaskHandle_t xHandleCurtainStepper = NULL;
TimerHandle_t xHandleTimerLED = NULL;

inline void vTaskCurtainStepper(void * pvPerameters) {

	while(1) {
		// Block and wait for message to unlock
		ulTaskNotifyTake(0, portMAX_DELAY);
		esp_task_wdt_add(xHandleCurtainStepper);
		printf("Hello world!\n");
		esp_task_wdt_reset();
		rotate(&stepperMotor_1, 1);
		xTaskNotify(xHandleCurtainStepper, 0, eSetValueWithOverwrite);
		esp_task_wdt_delete(xHandleCurtainStepper);
	}

}

inline void vInitTaskCurtainStepper() {
	xTaskCreate(vTaskCurtainStepper, "curtainStepper", 2048, NULL, 25, &xHandleCurtainStepper);
	configASSERT(xHandleCurtainStepper);
}

inline void vTaskLEDFade( void * pvParameters)
{
	int64_t ticsElapsed = 0;
	const unsigned fadeTime = 3000000; // fade time in microseconds
	const unsigned ticInterval = truncf(fadeTime/xGetDutyResolutionMax(LEDC_CHANNEL_0_DUTY_BITS)) ;	// (Every number of microseconds to update duty cycle by 1) x number of seconds
	// Take a factor of the number of elapsed tics, just in case this task did not receive CPU time for an elapsed period for some reason. Truncate floating points down.
	// unsigned amountChange = trunc(xGetDutyResolutionMax()/ticInterval);
	bool firstRun = true;

	while(1) {
		// blocks and waits for a notification. See peram details for more behavior info.
		// xTaskNotifyWait(0, ULONG_MAX, NULL, portMAX_DELAY);
		ulTaskNotifyTake(0, ULONG_MAX);
		esp_task_wdt_add(xHandleLEDFade);

		// print some stuff for debugging.
		printf("%s%i\n", "Duty Value: ", (int)LEDC_CHANNEL_0_DUTY);

		if (firstRun) {
			ticsElapsed = esp_timer_get_time();
			firstRun = false;
		}
		// Find the time delta between the last time this task got CPU usage.
		ticsElapsed += esp_timer_get_time() - ticsElapsed;

		if (ticsElapsed >= ticInterval) {

#ifdef BTN_0_LED_DRIVER_N
			// This prevents unsigned from wrapping around to uint max
			if ((int)LEDC_CHANNEL_0_DUTY > 0) {
				LEDC_CHANNEL_0_DUTY--;
				ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (uint32_t)LEDC_CHANNEL_0_DUTY, 0);
			}
#endif

#ifdef BTN_0_LED_DRIVER_P
			// This prevents unsigned from wrapping around to uint max
			if ((int)LEDC_CHANNEL_0_DUTY <= LEDC_CHANNEL_0_DUTY_MAX) {
				LEDC_CHANNEL_0_DUTY++;
				ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (uint32_t)LEDC_CHANNEL_0_DUTY);
			}
#endif

#ifdef BTN_0_LED_DRIVER_N
			if ((int)LEDC_CHANNEL_0_DUTY <= 0) {
				LEDC_CHANNEL_0_DUTY = 0;
				ticsElapsed = 0;
				firstRun = true;
				
				// Tell task to block task when fade is complete.
				xTaskNotify(xHandleLEDFade, 0, eSetValueWithOverwrite);
				esp_task_wdt_delete(xHandleLEDFade);
			}
			else {	// Continue while loop
				xTaskNotify(xHandleLEDFade, 1, eSetValueWithOverwrite);
			}
#endif

#ifdef BTN_0_LED_DRIVER_P
			if ((int)LEDC_CHANNEL_0_DUTY >= LEDC_CHANNEL_0_DUTY_MAX) {
				LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
				ticsElapsed = 0;
				firstRun = true;
				// Tell task to block task when fade is complete.
				xTaskNotify(xHandleLEDFade, 0, eSetValueWithOverwrite);
			}
			else {	// Continue while loop
				xTaskNotify(xHandleLEDFade, 1, eSetValueWithOverwrite);
			}
#endif
			// Reset the task.
			esp_task_wdt_reset();
			vTaskDelay(1);
		}
	}

}

inline void vInitTaskLEDFade( void )
{
	// Init the task, passing in the callback, a name, stack size, callback params, priority and finally-- the handler itself.
	xTaskCreate( vTaskLEDFade, "LEDFade", 2048, NULL, 1, &xHandleLEDFade);
	// assert the task to ensure it was created succesfully. it will be thrown in console otherwise.
	configASSERT(xHandleLEDFade);
}

// inline void vTaskLEDFade( void * args) {

// 	if (LEDC_CHANNEL_0_DUTY > 0) {
// 		LEDC_CHANNEL_0_DUTY--;
// 	}
	
// 	ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY, 0);
// }

// inline void vInitTimerLEDFade( int ms ) {
// 	int period_ms = APB_CLK_FREQ/1000;
// 	int target_period_us = ms * period_ms;
// 	int alarm_value = target_period_us/LEDC_CHANNEL_0_DUTY_MAX;

// 	printf("alarm_value %i\n", alarm_value);

// 	xHandleTimerLED = xTimerCreate("LED Timer", pdMS_TO_TICKS(1), true, xHandleTimerLED, vTaskLEDFade);
// 	xTimerStart(xHandleTimerLED, 0 );
// }

inline void vInitTaskRTOSDebug( void ) {
	// Init the task, passing in the callback, a name, stack size, callback params, priority and finally-- the handler itself.
	xTaskCreate(vTaskRTOSDebug, "RTOSDebug", 2048, NULL, 1, &xHandleRTOSDebug);
	// assert the task to ensure it was created succesfully. it will be thrown in console otherwise.
	configASSERT(xHandleRTOSDebug);
}

inline void vTaskRTOSDebug( void * pvParameters){

	while(1) {

		if (xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) == pdTRUE) {
			char buffer[500];
			vTaskList(buffer);

			printf("%s\n", "**********************************");
			printf("%s\n", "Task  State   Prio    Stack    Num");
			printf("%s\n", buffer);
			printf("%s\n", "**********************************");
		}
		vTaskDelay(5000 / portTICK_PERIOD_MS);
		
		xTaskNotify(xHandleRTOSDebug, 1, eSetValueWithOverwrite);
	}

}

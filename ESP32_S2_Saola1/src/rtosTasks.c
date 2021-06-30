#include <rtosTasks.h>

/*
	Define all of your callback functions here.

	Note: no function should ever return anything. Make use of blocking tasks or simply uninstall the task after your done.
*/ 

TaskHandle_t xHandleRTOSDebug = NULL;
TaskHandle_t xHandleLEDFade = NULL;
TimerHandle_t xHandleTimerLED = NULL;
TaskHandle_t xHandleOpenCurtains = NULL;
TaskHandle_t xHandleCloseCurtains = NULL;
TaskHandle_t xHandleCurtainStepperForward = NULL;
TaskHandle_t xHandleCurtainStepperReverse = NULL;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;



void vInitTaskRTOSDebug( void ) {
	// Init the task, passing in the callback, a name, stack size, callback params, priority and finally-- the handler itself.
	xTaskCreate(vTaskRTOSDebug, "RTOSDebug", 2048, NULL, 1, &xHandleRTOSDebug);
	// assert the task to ensure it was created succesfully. it will be thrown in console otherwise.
	configASSERT(xHandleRTOSDebug);
}

void vInitTaskLEDFade( void )
{
	// Init the task, passing in the callback, a name, stack size, callback params, priority and finally-- the handler itself.
	xTaskCreate( vTaskLEDFade, "LEDFade", 2048, NULL, 1, &xHandleLEDFade);
	// assert the task to ensure it was created succesfully. it will be thrown in console otherwise.
	configASSERT(xHandleLEDFade);
}

void vInitTaskCurtainMotor() {
	xTaskCreate(vTaskRotateStepperForward, "curtainStepperForward", 2048, NULL, 2, &xHandleCurtainStepperForward);
	xTaskCreate(vTaskRotateStepperReverse, "curtainStepperReverse", 2048, NULL, 2, &xHandleCurtainStepperReverse);
	xTaskCreate(vTaskOpenCurtains, "curtainOpen", 2048, NULL, 2, &xHandleOpenCurtains);
	configASSERT(xHandleOpenCurtains);
	xTaskCreate(vTaskCloseCurtains, "curtainClose", 2048, NULL, 2, &xHandleCloseCurtains);
	configASSERT(xHandleCloseCurtains);
	configASSERT(xHandleCurtainStepperForward);
	configASSERT(xHandleCurtainStepperReverse);
}

void vTaskOpenCurtains(void * pvPerameters) {

	while(1) {
		// Block and wait for message to unlock
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		
		
		// Schedule task to wdt, so we can reset timeout periodically.
		esp_task_wdt_add(xHandleCurtainStepperForward);

		// Rotate the stepper motor forward.
		// We use an interval of 360 degrees so that it has time to run until the ISR checks button state again.
		move(StepperMotor_1, calcStepsForRotation(StepperMotor_1, 360));

		// Reset the wdt from this running task
		esp_task_wdt_reset();
		// unschedule/delete the task from wdt, so blocking does not cause timeout of wdt
		esp_task_wdt_delete(xHandleCurtainStepperForward);
		// Give idle task a moment to free up any resources
		// vTaskDelay(1);
		
	}

}

void vTaskCloseCurtains( void * pvPerameters) {
	float distance = HEIGHT_INCHES * 25.4;	// 25.4mm per inch.
	// printf("distance (inches): %f\n", distance);
	float circumference = 2 * M_PI * (DIAMETER_MM/2);	// find the circumference (2 * pi * r)
	// printf("circumference (mm): %f\n", circumference);
	int revolutionsToTarget = distance / circumference;
	// printf("revolution to target (revs): %i\n", revolutionsToTarget);
	short completed = 0;

	while(1) {
		// Block and wait for message to unlock
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		// https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/system/wdts.html
		// portENTER_CRITICAL(&mux);
		// portDISABLE_INTERRUPTS();

		// Schedule task to wdt, so we can reset timeout periodically.
		esp_task_wdt_add(xHandleCloseCurtains);

		move(StepperMotor_1, calcStepsForRotation(StepperMotor_1, 360));
		completed++;

		if (completed < revolutionsToTarget) {
			// Reset the wdt from this running task
			esp_task_wdt_reset();
			// Continue the task until the amount of revolutions has been reached
			xTaskNotify(xHandleCloseCurtains, 1, eIncrement);
			// Give idle task a moment to free up any resources
			vTaskDelay(1);
		} else {
			// Tell the task to stop running.
			xTaskNotify(xHandleCloseCurtains, 0, eSetValueWithOverwrite);
			// unschedule/delete the task from wdt, so blocking does not cause timeout of wdt
			esp_task_wdt_delete(xHandleCloseCurtains);
			// reset the completed revolution counter
			completed = 0;

			// portEXIT_CRITICAL_ISR(&mux);
			// portENABLE_INTERRUPTS();
			// esp_int_wdt_cpu_init();
			// esp_int_wdt_init();
		}

	}

}

void vTaskRotateStepperForward(void * pvPerameters) {

	while(1) {
		// Block and wait for message to unlock
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		// Schedule task to wdt, so we can reset timeout periodically.
		esp_task_wdt_add(xHandleCurtainStepperForward);

		// Rotate the stepper motor forward.
		// We use an interval of 360 degrees so that it has time to run until the ISR checks button state again.
		move(StepperMotor_1, calcStepsForRotation(StepperMotor_1, 360));

		// Reset the wdt from this running task
		esp_task_wdt_reset();
		// unschedule/delete the task from wdt, so blocking does not cause timeout of wdt
		esp_task_wdt_delete(xHandleCurtainStepperForward);
		// Give idle task a moment to free up any resources
		vTaskDelay(1);
	}

}

void vTaskRotateStepperReverse(void * pvPerameters) {

	while(1) {
		// Block and wait for message to unlock
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		// Schedule task to wdt, so we can reset timeout periodically.
		esp_task_wdt_add(xHandleCurtainStepperReverse);

		// Rotate the stepper motor reverse.
		// We use an interval of 360 degrees so that it has time to run until the ISR checks button state again.
		move(StepperMotor_1, -(calcStepsForRotation(StepperMotor_1, 360)));

		// Reset the wdt from this running task
		esp_task_wdt_reset();
		// unschedule/delete the task from wdt, so blocking does not cause timeout of wdt
		esp_task_wdt_delete(xHandleCurtainStepperReverse);
		// Give idle task a moment to free up any resources
		vTaskDelay(1);
	}

}



void vTaskLEDFade( void * pvParameters)
{
	int64_t ticsElapsed = 0;
	const unsigned fadeTime = 3000000; // fade time in microseconds
	const unsigned ticInterval = truncf(fadeTime/xGetDutyResolutionMax(LEDC_CHANNEL_0_DUTY_BITS)) ;	// (Every number of microseconds to update duty cycle by 1) x number of seconds
	// Take a factor of the number of elapsed tics, just in case this task did not receive CPU time for an elapsed period for some reason. Truncate floating points down.
	// unsigned amountChange = trunc(xGetDutyResolutionMax()/ticInterval);
	bool firstRun = true;

	while(1) {
		// blocks and waits for a notification. See peram details for more behavior info.
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		// schedule task to wdt, so we can manually reset periodically
		esp_task_wdt_add(xHandleLEDFade);

		// update tics at time of invocation of task
		if (firstRun) {
			ticsElapsed = esp_timer_get_time();
			firstRun = false;
		}

		// Find the delta between the last time the task ran, and the current point in time.
		ticsElapsed += esp_timer_get_time() - ticsElapsed;

		if (ticsElapsed >= ticInterval) {

#ifdef BTN_0_LED_DRIVER_N

			if (LEDC_CHANNEL_0_DUTY > 0) {
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
			// block if fade is completed.
			if ((int)LEDC_CHANNEL_0_DUTY >= LEDC_CHANNEL_0_DUTY_MAX) {
				// update state
				LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
				// reset initializers
				ticsElapsed = 0;
				firstRun = true;
				// This shouldn't be needed, but enforce blocking.
				xTaskNotify(xHandleLEDFade, 0, eSetValueWithOverwrite);
			}
			else {	// Continue looping
				xTaskNotify(xHandleLEDFade, 1, eIncrement);
			}
#endif
		}

		// tell the wdt that this task is OK to be consuming lots of CPU time (it's low priority, but high consumption)
		esp_task_wdt_reset();

		// delay for 1ms to let idle task free up any resources
		// This is actually 0.1 ms when CONFIG_FREERTOS_HZ = 1000.
		vTaskDelay(1);
	}

}

void vTaskRTOSDebug( void * pvParameters){

	while(1) {

		if (ulTaskNotifyTake(pdFALSE, portMAX_DELAY) == pdTRUE) {
			char buffer[500];
			vTaskList(buffer);

			printf("%s\n", "**********************************");
			printf("%s\n", "Task  State   Prio    Stack    Num");
			printf("%s\n", buffer);
			printf("%s\n", "**********************************");
		}
	}

}

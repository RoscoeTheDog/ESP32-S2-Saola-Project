#include <rtosTasks.h>
#include <esp_int_wdt.h>
/*
	Define all of your callback functions here.

	Note: no function should ever return anything. Make use of blocks or simply uninstall the task when you want it to stop.
*/ 

TaskHandle_t xHandleRTOSDebug = NULL;
TaskHandle_t xHandleLEDFade = NULL;

inline void vTaskLEDFade( void * pvParameters)
{
	int64_t ticsElapsed = 0;
	const unsigned fadeTime = 3000000; // fade time in microseconds
	const unsigned ticInterval = truncf(fadeTime/xGetDutyResolutionMax()) ;	// (Every number of microseconds to update duty cycle by 1) x number of seconds
	// Take a factor of the number of elapsed tics, just in case this task did not receive CPU time for an elapsed period for some reason. Truncate floating points down.
	// unsigned amountChange = trunc(xGetDutyResolutionMax()/ticInterval);
	bool firstRun = true;

	while(1){
		// blocks and waits for a notification. See peram details for more behavior info.
		// xTaskNotifyWait(0, ULONG_MAX, NULL, portMAX_DELAY);

		ulTaskNotifyTake(0, ULONG_MAX);

		// print some display info.
		printf("%s%i\n", "Duty Value: ", (int)LEDC_CHANNEL_0_DUTY);

		if (firstRun){
			ticsElapsed = esp_timer_get_time();
			firstRun = false;
		}

		// we can't neccessarily initialize outside of while loop since we want the time since it last unblocked.
		// ticsLastLoop = esp_timer_get_time(); 
		// Find the time delta between the last time this task got CPU usage.
		ticsElapsed += esp_timer_get_time() - ticsElapsed;
		// printf("%s%llu\n", "ticsElapsed: ", ticsElapsed);

		esp_task_wdt_reset();

		if (ticsElapsed >= ticInterval) {
			
			// This prevents unsigned from wrapping around to uint max
			if ((int)LEDC_CHANNEL_0_DUTY > 0) {
				LEDC_CHANNEL_0_DUTY--;
				ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (uint32_t)LEDC_CHANNEL_0_DUTY);
			}
			
			if ((int)LEDC_CHANNEL_0_DUTY <= 0)
			{
				LEDC_CHANNEL_0_DUTY = 0;
				ticsElapsed = 0;
				firstRun = true;
				// Tell task to block task when fade is complete.
				xTaskNotify(xHandleLEDFade, 0, eSetValueWithOverwrite);
			}
			else	// Continue while loop
				xTaskNotify(xHandleLEDFade, 1, eSetValueWithOverwrite);
			}
	}
	
}

inline void vInitTaskLEDFade( void )
{
	// Init the task, passing in the callback, a name, stack size, callback params, priority and finally-- the handler itself.
	xTaskCreate( vTaskLEDFade, "LEDFade", 2048, NULL, 25, &xHandleLEDFade);
	// assert the task to ensure it was created succesfully. it will be thrown in console otherwise.
	configASSERT(xHandleLEDFade);
}

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

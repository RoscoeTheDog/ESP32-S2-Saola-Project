#include <rtosTasks.h>
#include <configWifi.h>
#include <freertos/task.h>

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
TaskHandle_t xHandleSleepTask = NULL;
TaskHandle_t xHandlePollWebServer = NULL;
TaskHandle_t xHandleSntpStatus = NULL;
TaskHandle_t xHandleUpdateMotor = NULL;
TaskHandle_t xHandleFoo = NULL;
TaskHandle_t xHandleStatusLEDWatchdog = NULL;
TaskHandle_t xHandleWifiReconnect = NULL;
TaskHandle_t xHandleTask1 = NULL;
TaskHandle_t xHandleTask2 = NULL;
TaskHandle_t xHandleHttpRequestServerData = NULL;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void initializeTasks() {
	char *TAG = "initializeTasks";

	ESP_LOGI(TAG, "initializing vTaskRTOSDebug");
	xTaskCreate(vTaskRTOSDebug, "vTaskRTOSDebug", 4096, NULL, 25, &xHandleRTOSDebug);
	configASSERT(xHandleRTOSDebug);
	// xTaskNotify(xHandleRTOSDebug, 1, eSetValueWithOverwrite);

	ESP_LOGI(TAG, "initializing vTaskSleep");
	// ESP_ERROR_CHECK(esp_sleep_enable_wifi_wakeup());
	xTaskCreate(vTaskSleep, "vTaskSleep", 2048, NULL, 25, &xHandleSleepTask);
	configASSERT(xHandleSleepTask);

	ESP_LOGI(TAG, "initializing vTaskStatusLEDWatchdog");
	xTaskCreate(vTaskStatusLEDWatchdog, "vTaskStatusLEDWatchdog", 2048, NULL, 25, &xHandleStatusLEDWatchdog);
	configASSERT(xHandleStatusLEDWatchdog);
	xTaskNotify(xHandleStatusLEDWatchdog, 1, eSetValueWithoutOverwrite);

	ESP_LOGI(TAG, "initializing vTaskWifiReconnect");
	xTaskCreate(vTaskWifiReconnect, "vTaskWifiReconnect", 2048, NULL, 22, &xHandleWifiReconnect);
	configASSERT(xHandleWifiReconnect);

	ESP_LOGI(TAG, "initializing vTaskSntpStatus");
	xTaskCreate(vTaskSntpStatus, "vTaskSntpStatus", 2048, NULL, 20, &xHandleSntpStatus);
	configASSERT(xHandleSntpStatus);

	ESP_LOGI(TAG, "initializing vTaskUpdateMotor");
	xTaskCreate(vTaskUpdateMotor, "vTaskUpdateMotor", 4096, NULL, 20, &xHandleUpdateMotor);
	configASSERT(xHandleUpdateMotor);
	ESP_LOGI(TAG, "initializing vTaskPollServer");
	xTaskCreate(vTaskPollServer, "vTaskPollServer", 4096, NULL, 10, &xHandlePollWebServer);
	configASSERT(xHandlePollWebServer);

	ESP_LOGI(TAG, "initializing vTaskLEDFade");
	// Init the task, passing in the callback, a name, stack size, callback params, priority and finally-- the handler itself.
	xTaskCreate( vTaskLEDFade, "vTaskLEDFade", 2048, NULL, 20, &xHandleLEDFade);
	// assert the task to ensure it was created succesfully. it will be thrown in console otherwise.
	configASSERT(xHandleLEDFade);

	ESP_LOGI(TAG, "initializing vTaskCurtainMotor");
	xTaskCreate(vTaskRotateStepperForward, "curtainStepperForward", 2048, NULL, 20, &xHandleCurtainStepperForward);
	xTaskCreate(vTaskRotateStepperReverse, "curtainStepperReverse", 2048, NULL, 20, &xHandleCurtainStepperReverse);
	xTaskCreate(vTaskOpenCurtains, "curtainOpen", 2048, NULL, 20, &xHandleOpenCurtains);
	xTaskCreate(vTaskCloseCurtains, "curtainClose", 2048, NULL, 20, &xHandleCloseCurtains);
	configASSERT(xHandleOpenCurtains);
	configASSERT(xHandleCloseCurtains);
	configASSERT(xHandleCurtainStepperForward);
	configASSERT(xHandleCurtainStepperReverse);

}

void vTaskWifiReconnect(void *args) {
	char *TAG = "vTaskWifiReconnect";
	memcpy(wifi_config.sta.ssid, WIFI_SSID, sizeof(char) * strlen(WIFI_SSID) + 1);
	memcpy(wifi_config.sta.password, WIFI_PASSWORD, sizeof(char) * strlen(WIFI_PASSWORD) + 1);
	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        while (!WIFI_CONNECTED) {
			// Suspend the polling of the server immediately if not already performed
			// vTaskSuspend(xHandlePollWebServer);
			xTaskNotify(xHandlePollWebServer, 0, eSetValueWithOverwrite);

			// update the wifi config with whatever is saved in globals.h
			updateWifiConfig();
			ESP_LOGI(TAG, "Attempting to connect to AP...");
			ESP_LOGI(TAG, "wifi_config.sta.ssid: %s", (char*)wifi_config.sta.ssid);
            ESP_LOGI(TAG, "wifi_config.sta.password: %s", (char*)wifi_config.sta.password);
			
            esp_err_t err = esp_wifi_connect();
            if (err == ESP_OK) {
                WIFI_CONNECTED = true;
            }
			if (err == ESP_ERR_WIFI_SSID) {
				WIFI_CONNECTED = false;
				ESP_LOGI(TAG, "ESP_ERR_WIFI_SSID returned. Incorrrect wifi credentials saved.");
			}

            vTaskDelay(pdMS_TO_TICKS(1000));
        }

		xTaskNotify(xHandleWifiReconnect, 1, eSetValueWithoutOverwrite);

	}
}

void vTaskUpdateMotor(void * args) {
	char *TAG = "vTaskUpdateMotor";
	long motor_position_steps_prev = MOTOR_POSITION_STEPS;
	long steps_remaining = 0;

	while (1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		
		if (motor_position_steps_prev != MOTOR_POSITION_STEPS) {
			ESP_LOGI(TAG, "Coordinates Received. Updating Motor to %f Percent", CURTAIN_PERCENTAGE);
			esp_task_wdt_add(xHandleUpdateMotor);
			long steps_to_travel = MOTOR_POSITION_STEPS - motor_position_steps_prev;
			steps_remaining = steps_to_travel;

			// DISTANCE / RPM / SECONDS
			if (steps_to_travel / StepperConfig_1->microstepping * StepperConfig_1->rpm * 60 * 360 > CONFIG_TASK_WDT_TIMEOUT_S) {
				
				// ESP_LOGI(TAG, "steps_to_travel may be greater than task timeout period. Executing in segments.");
				// there may be some associated overhead from calling and using the stepper API.
				// take 80% of the normal travel distance in a single action instead to avoid tripping the task wdt
				int val = 0.9 * (steps_remaining / CONFIG_TASK_WDT_TIMEOUT_S);
				while (steps_remaining) {
					// printf("steps_remaining: %li\n", steps_remaining);

					if (steps_remaining > val) {
						// printf("moving number of steps: %i\n", val);
						move(StepperMotor_1, val);
						steps_remaining -= val;
					} else {
						// printf("moving last number of steps: %li\n", steps_remaining);
						move(StepperMotor_1, steps_remaining);
						steps_remaining = 0;
					}
					esp_task_wdt_reset();
				}

			} else {
				// printf("steps_to_travel being executed once\n");
				move(StepperMotor_1, steps_to_travel);
				esp_task_wdt_reset();
			}
			
			// update value of last traveled
			motor_position_steps_prev = MOTOR_POSITION_STEPS;
			
			// TODO: make this account for the material thickness around the rod dynamically
			int length_mm = CURTAIN_LENGTH_INCH * 25.4;
			int circumference = 2 * M_PI * (ROD_DIAMETER_MM/2);
			// update globals (motor_position_steps / maximum_step_length)
			CURTAIN_PERCENTAGE = MOTOR_POSITION_STEPS / calcStepsForRotation(StepperMotor_1, (length_mm / circumference) * 360);

			// just to make things more readable.
			// int curtain_length_mm = CURTAIN_LENGTH_INCH * 25.4;
			// int circumference = 2 * M_PI * ((CURTAIN_LENGTH_INCH * 25.4) / 2) + MATERIAL_THICKNESS_MM)
			// (MATERIAL_THICKNESS_MM * (CURTAIN_LENGTH_INCH * 25.4) / (2 * M_PI * (ROD_DIAMETER_MM / 2) ) 
			
			// enschedule the task when done to prevent panic during thread lock.
			esp_task_wdt_delete(xHandleUpdateMotor);
		}
		
	}

}

void vTaskSntpStatus(void * args) {
	char *TAG = "vTaskSntpStatus";
	while (1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		if (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
			ESP_LOGI(TAG, "Waiting for system time to be set...");
		}
		if (sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS) {
			ESP_LOGI(TAG, "Smooth time sync in progress...");
		}

		vTaskDelay(pdMS_TO_TICKS(1));
    }

}

void vTaskStatusLEDWatchdog( void *args) {
	char *TAG = "vTaskStatusLEDWatchdog";
	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		if (WIFI_CONNECTED && !DATETIME_SYNCED) {
			ESP_LOGI(TAG, "SYSTEM WAITING ON DATETIME SYNC");
			setStatusLEDYellow();
			vTaskDelay(pdMS_TO_TICKS(200));
			setStatusLEDBlue();
		}
		if (HTTP_ERROR) {
			ESP_LOGI(TAG, "SYSTEM WAITING ON HTTP_ERROR");
			setStatusLEDYellow();
			vTaskDelay(pdMS_TO_TICKS(100));
			setStatusLEDBlue();
			vTaskDelay(pdMS_TO_TICKS(100));

			setStatusLEDYellow();
			vTaskDelay(pdMS_TO_TICKS(100));
			setStatusLEDBlue();
		}
		if (WIFI_CONNECTED) {
			setStatusLEDBlue();
		}
		if (!WIFI_CONNECTED) {
			setStatusLEDGreen();
		}

		// the frequency of the light indicator in MS
		vTaskDelay(pdMS_TO_TICKS(1000));
		xTaskNotify(xHandleStatusLEDWatchdog, 1, eSetValueWithOverwrite);		
	}

}

// simple wrapped function to ensure the resources are freed 
void vTaskHttpRequestServerData(void *args) {

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		http_request_server_data();
		xTaskNotify(xHandleHttpRequestServerData, 1, eSetValueWithOverwrite);
	}
	
}

void vTaskPollServer(void * args) {
	char *TAG = "vTaskPollServer";

	while(true) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		esp_task_wdt_add(xHandlePollWebServer);
		
		// wrap in while loop for faster exit in event disconnect (shouldn't be needed but safety)
		if (WIFI_CONNECTED && DATETIME_SYNCED) { 
			ESP_LOGI(TAG, "STARTING POLL WEB SERVER");
			// only flag the HTTP_ERROR system macro after a timeout period of 5 
			uint64_t timeout_count_start = esp_timer_get_time();
			esp_err_t err = http_request_server_data();
			while (err != ESP_OK) {
				// 100ms delay between requests
				vTaskDelay(pdMS_TO_TICKS(100));	
				// perform request again
				err = http_request_server_data();
				// exit if time took > then timeout period
				if (esp_timer_get_time() - timeout_count_start > 5000000) {
					HTTP_ERROR = true;
					break;
				}
			}

			if (err == ESP_OK && DATETIME_SYNCED) {

				if (http_parse_server_data() == ESP_OK) {
					xTaskNotify(xHandleUpdateMotor, 1, eSetValueWithOverwrite);
				}
				
			}
			
		}
		xTaskNotify(xHandlePollWebServer, 1, eSetValueWithoutOverwrite);
		vTaskDelay(pdMS_TO_TICKS(100));
		esp_task_wdt_reset();
		esp_task_wdt_delete(xHandlePollWebServer);
	}

}

bool vTaskIdleHook() {
	xTaskNotify(xHandleSleepTask, 1, eSetValueWithoutOverwrite);

	// true for RTOS tic, false for a real-time tic
	return false;
}

void vTaskSleep(void *pvParameters) {
	char *TAG = "vTaskSleep";
// 	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
// 	vPortEnterCriticalSafe(&mux)
	// ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL));
	// enable modem sleep mode
	int interval_sec = 1;
	ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(interval_sec * 1000000));

	while (true) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		ESP_ERROR_CHECK(esp_task_wdt_add(xHandleSleepTask));
		ESP_LOGI(TAG, "Entering light sleep");
        /* To make sure the complete line is printed before entering sleep mode,
         * need to wait until UART TX FIFO is empty:
         */
		ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_ERROR_CHECK(uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM));
		ESP_ERROR_CHECK(esp_task_wdt_reset());

        /* Get timestamp before entering sleep */
        int64_t t_before_us = esp_timer_get_time();
		esp_wifi_disconnect();
		// esp_wifi_stop();
        /* Enter sleep mode */
        ESP_ERROR_CHECK(esp_light_sleep_start());
        /* Execution continues here after wakeup */

        /* Get timestamp after waking up from sleep */
        int64_t t_after_us = esp_timer_get_time();
		esp_task_wdt_reset();
		esp_wifi_connect();

        /* Determine wake up reason */
        const char* wakeup_reason;
        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                wakeup_reason = "timer";
                break;
            case ESP_SLEEP_WAKEUP_GPIO:
                wakeup_reason = "pin";
                break;
			case ESP_SLEEP_WAKEUP_WIFI:
				wakeup_reason = "wifi";
				break;
            default:
                wakeup_reason = "other";
                break;
        }
		esp_task_wdt_reset();
        // printf("Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
                // wakeup_reason, t_after_us / 1000, (t_after_us - t_before_us) / 1000);

		// xTaskCreate(vTaskSmartConfig, "smartconfig_example_task", 4096 * 3, NULL, 1, NULL);
		ESP_ERROR_CHECK(esp_task_wdt_delete(xHandleSleepTask));
		vTaskDelay(pdMS_TO_TICKS(1));
		xTaskNotify(xHandleSleepTask, 1, eSetValueWithOverwrite);
	}

	// vPortExitCritical(&mux);

}

void vTaskOpenCurtains(void * pvPerameters) {
	char *TAG = "vTaskOpenCurtains";
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
	char *TAG = "vTaskCloseCurtains";
	float distance = CURTAIN_LENGTH_INCH * 25.4;	// 25.4mm per inch.
	// printf("distance (inches): %f\n", distance);
	float circumference = 2 * M_PI * (ROD_DIAMETER_MM/2);	// find the circumference (2 * pi * r)
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
	char *TAG = "vTaskRotateStepperForward";
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
	char *TAG = "vTaskRotateStepperReverse";
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



void vTaskLEDFade( void * pvParameters) {
	char *TAG = "vTaskLEDFade";
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
	char *TAG = "vTaskRTOSDebug";
	

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		char buffer[2048];
		vTaskList(buffer);
		printf("%s\n", "**********************************");
		printf("%s\n", "Task  State   Prio    Stack    Num");
		printf("%s\n", buffer);
		printf("%s\n", "**********************************");

		vTaskDelay(pdMS_TO_TICKS(500));
		xTaskNotify(xHandleRTOSDebug, 1, eSetValueWithoutOverwrite);
	}

}

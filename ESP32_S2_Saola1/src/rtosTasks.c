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
TaskHandle_t xHandleMoveStepperForward = NULL;
TaskHandle_t xHandleMoveStepperReverse = NULL;
TaskHandle_t xHandleSleepTask = NULL;
TaskHandle_t xHandlePollServer = NULL;
TaskHandle_t xHandleSntpStatus = NULL;
TaskHandle_t xHandleUpdateMotor = NULL;
TaskHandle_t xHandleFoo = NULL;
TaskHandle_t xHandleStatusLEDWatchdog = NULL;
TaskHandle_t xHandleWifiReconnect = NULL;
TaskHandle_t xHandleHttpRequestServerData = NULL;
TaskHandle_t xHandleSubmitLocalData = NULL;
TaskHandle_t xHandleWifiPersistingTasks = NULL;
TaskHandle_t xHandleSmartConfig = NULL;
TaskHandle_t xHandleHomeCurtains = NULL;

void initializeRTOSTasks() {
	char *TAG = "initializeRTOSTasks";

	ESP_LOGI(TAG, "initializing vTaskRTOSDebug");
	xTaskCreate(vTaskRTOSDebug, "vTaskRTOSDebug", 4096, NULL, 24, &xHandleRTOSDebug);
	configASSERT(xHandleRTOSDebug);
	// xTaskNotify(xHandleRTOSDebug, 1, eSetValueWithOverwrite);

	ESP_LOGI(TAG, "initializing vTaskSleep");
	// ESP_ERROR_CHECK(esp_sleep_enable_wifi_wakeup());
	xTaskCreate(vTaskSleep, "vTaskSleep", 2048, NULL, 24, &xHandleSleepTask);
	configASSERT(xHandleSleepTask);

	ESP_LOGI(TAG, "initializing vTaskStatusLEDWatchdog");
	xTaskCreate(vTaskStatusLEDWatchdog, "vTaskStatusLEDWatchdog", 2048, NULL, 23, &xHandleStatusLEDWatchdog);
	configASSERT(xHandleStatusLEDWatchdog);
	xTaskNotify(xHandleStatusLEDWatchdog, 1, eSetValueWithoutOverwrite);

	ESP_LOGI(TAG, "initializing vTaskLEDFade");
	xTaskCreate( vTaskLEDFade, "vTaskLEDFade", 2048, NULL, 23, &xHandleLEDFade);
	configASSERT(xHandleLEDFade);

	ESP_LOGI(TAG, "initializing vTaskMoveStepperForward");
	xTaskCreate(vTaskMoveStepperForward, "moveStepperForward", 2048, NULL, 22, &xHandleMoveStepperForward);
	configASSERT(xHandleMoveStepperForward);

	ESP_LOGI(TAG, "initializing vTaskMoveStepperReverse");
	xTaskCreate(vTaskMoveStepperReverse, "curtainStepperReverse", 2048, NULL, 22, &xHandleMoveStepperReverse);
	configASSERT(xHandleMoveStepperReverse);

	ESP_LOGI(TAG, "initializing vTaskWifiReconnect");
	xTaskCreate(vTaskWifiReconnect, "vTaskWifiReconnect", 4096, NULL, 22, &xHandleWifiReconnect);
	configASSERT(xHandleWifiReconnect);
	xTaskNotify(xHandleWifiReconnect, 1, eSetValueWithoutOverwrite);

	ESP_LOGI(TAG, "initializing vTaskHomeCurtains");
	xTaskCreate(vTaskHomeCurtains, "vTaskHomeCurtains", 2048, NULL, 20, &xHandleHomeCurtains);
	configASSERT(xHandleHomeCurtains);

	ESP_LOGI(TAG, "initializing vTaskPersistingWifiTasks");
	xTaskCreate(vTaskPersistingWifiTasks, "vTaskPersistingWifiTasks", 2048, NULL, 20, &xHandleWifiPersistingTasks);
	configASSERT(xHandleWifiPersistingTasks);
	// xTaskNotify(xHandleWifiPersistingTasks, 1, eSetValueWithoutOverwrite);

	ESP_LOGI(TAG, "initializing vTaskSubmitLocalData");
	xTaskCreate(vTaskSubmitLocalData, "vTaskSubmitLocalData", 4096, NULL, 10, &xHandleSubmitLocalData);
	configASSERT(xHandleSubmitLocalData);

	ESP_LOGI(TAG, "initializing vTaskUpdateMotor");
	xTaskCreate(vTaskUpdateMotor, "vTaskUpdateMotor", 4096, NULL, 9, &xHandleUpdateMotor);
	configASSERT(xHandleUpdateMotor);

	ESP_LOGI(TAG, "initializing vTaskPollServer");
	xTaskCreate(vTaskPollServer, "vTaskPollServer", 4096, NULL, 9, &xHandlePollServer);
	configASSERT(xHandlePollServer);

	// ESP_LOGI(TAG, "initializing vTaskSmartConfig");
	// xTaskCreate(vTaskSmartConfig, "vTaskSmartConfig", 4096, NULL, 20, &xHandleSmartConfig);
	// configASSERT(xHandleSmartConfig);
}

void vTaskHomeCurtains(void *args) {

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		if(!LIMIT_SWITCH_STATE) {
			
			if (xHandleMoveStepperForward) {
				// notify max number of times. will run indefinately until ISR stops it
				xTaskNotify(xHandleMoveStepperForward, ULONG_MAX - 1, eSetValueWithOverwrite);
			}
			if (xHandleHomeCurtains) {
				xTaskNotify(xHandleHomeCurtains, 1, eSetValueWithoutOverwrite);
			}
			
		}	else {
			if (xHandleHomeCurtains) {
				xTaskNotify(xHandleHomeCurtains, 0, eSetValueWithoutOverwrite);
			}
		}
		
 	}

}

void vTaskPersistingWifiTasks(void *args) {

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		if(WIFI_CONNECTED) {

			if (xHandlePollServer && (eTaskGetState(xHandleMoveStepperForward) != eRunning || eTaskGetState(xHandleMoveStepperForward) != eReady) ) {
				xTaskNotify(xHandlePollServer, 1, eSetValueWithoutOverwrite);
			}

			vTaskDelay(pdMS_TO_TICKS(1000));
		}
		
		vTaskDelay(pdMS_TO_TICKS(1));
		xTaskNotify(xHandleWifiPersistingTasks, 1, eSetValueWithoutOverwrite);

	}

}

void vTaskWifiReconnect(void *args) {
	char *TAG = "vTaskWifiReconnect";
	memcpy(wifi_config.sta.ssid, WIFI_SSID, sizeof(char) * 64);
	memcpy(wifi_config.sta.password, WIFI_PASSWORD, sizeof(char) * 32);
	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		esp_task_wdt_add(xHandleWifiReconnect);

        if (!WIFI_CONNECTED) {
			// update the wifi config with whatever is saved in globals.h
			updateWifiConfig();
			ESP_LOGI(TAG, "Attempting connect to AP...");
			ESP_LOGI(TAG, "SSID: %s", (char*)wifi_config.sta.ssid);
            ESP_LOGI(TAG, "PASSWORD: %s", (char*)wifi_config.sta.password);

			esp_task_wdt_reset();

			if (!RADIO_INITIALIZED) {
				initializeWifi();
			}

            esp_err_t err = esp_wifi_connect();
            if (err == ESP_OK) {
                WIFI_CONNECTED = true;
            }
			if (err == ESP_ERR_WIFI_SSID) {
				WIFI_CONNECTED = false;
				ESP_LOGI(TAG, "ESP_ERR_WIFI_SSID returned. Incorrrect wifi credentials saved.");
			}
			if (err != ESP_OK) {
				ESP_ERROR_CHECK(err);
			}			

			esp_task_wdt_reset();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
		// enforce task wdt reset
		esp_task_wdt_reset();
		// unschedule before getting blocked
		esp_task_wdt_delete(xHandleWifiReconnect);
		// delay for 1 ms, allowing other tasks with lower priority to run during this time.
		vTaskDelay(pdMS_TO_TICKS(1));
		// continue running the task unless otherwise blocked or deleted
		xTaskNotify(xHandleWifiReconnect, 1, eSetValueWithoutOverwrite);
	}
}

void vTaskSmartConfig(void *args) {
	char *TAG = "vTaskSmartConfig";
	EventBits_t uxBits;

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		while (!ESP_SMARTCONFIG_STATUS) {
			if (!RADIO_INITIALIZED) {
				initializeWifi();
			}
			ESP_LOGI(TAG, "configuring smartconfig service");
			ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
			smartconfig_start_config_t cfg = {.enable_log = false};

			ESP_LOGI(TAG, "starting smartconfig service");
			esp_err_t err = esp_smartconfig_start(&cfg);
			ESP_ERROR_CHECK(err);
			if (err == ESP_OK) {
				ESP_SMARTCONFIG_STATUS = true;
				ESP_LOGI(TAG, "smartconfig service started successfully");
			} else {
				ESP_LOGI(TAG, "smartconfig service failed to initialize");
			}
			
			vTaskDelay(1);			
		}

		// while (1) {
		// 	uxBits = xEventGroupWaitBits(s_wifi_event_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);

		// 	if(uxBits & ESPTOUCH_DONE_BIT) {
		// 		ESP_LOGI(TAG, "smart config completed -- stopping service.");
		// 		esp_smartconfig_stop();
		// 		ESP_SMARTCONFIG_STATUS = false;
		// 		xEventGroupClearBits(xHandleSmartConfig, ESPTOUCH_DONE_BIT);
		// 	}
		// 	vTaskDelay(1);
		// }

	}
}

// updates the stepper to the set % of length periodically
void vTaskUpdateMotor(void * args) {
	char *TAG = "vTaskUpdateMotor";
	long steps_remaining = 0;
	long distance = 0;

	while (1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		// cancel move command if system is not synced with server.
		if (!SYS_SYNC) {
			xTaskNotify(xHandleUpdateMotor, 0, eSetValueWithOverwrite);
			continue;
		}

		// TODO: optimize this code more
		// always recalculate the assigned percentage in steps.
		int length_mm = CURTAIN_LENGTH_INCH * 25.4;
		int circumference_mm = 2 * M_PI * (ROD_DIAMETER_MM/2);
		long motor_percentage_steps = (CURTAIN_PERCENTAGE * .01) * (double)calcStepsForRotation(StepperMotor_1, (length_mm / circumference_mm) * 360);
		
		// server is read, parsed and new percentage is set.
		// calculate the delta between current position and the new position in %
		steps_remaining = motor_percentage_steps - MOTOR_POSITION_STEPS;
		long steps_to_travel = abs(steps_remaining);

		long move_interval = 0;
		if(steps_remaining > 0) {
			move_interval = calcStepsForRotation(StepperMotor_1, 1);
		} else {
			move_interval = calcStepsForRotation(StepperMotor_1, -1);
		}

		while (abs(steps_remaining)) {
			// enter a critical section to prevent losing track of steps if task is interrupted.
			portENTER_CRITICAL(&mux);
			if (abs(steps_remaining) > abs(move_interval)) {
				move(StepperMotor_1, move_interval);
				MOTOR_POSITION_STEPS += move_interval;
				steps_remaining -= move_interval;
			} else {
				move(StepperMotor_1, steps_remaining);
				MOTOR_POSITION_STEPS += steps_remaining;
				steps_remaining = 0;
			}

			portEXIT_CRITICAL(&mux);
		}

		// update local nvs value after completion
		nvsWriteBlob("init", "MOTOR_STEPS", &MOTOR_POSITION_STEPS, sizeof(long));
	}

}

void vTaskStatusLEDWatchdog( void *args) {
	char *TAG = "vTaskStatusLEDWatchdog";
	short num_blinks;
	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		
		if (WIFI_CONNECTED && !DATETIME_SYNC) {
			ESP_LOGI(TAG, "SYSTEM WAITING ON DATETIME SYNC");
			setStatusLEDYellow();
			vTaskDelay(pdMS_TO_TICKS(200));
			setStatusLEDBlue();
		}
		if (WIFI_CONNECTED && HTTP_ERROR) {
			ESP_LOGI(TAG, "SYSTEM WAITING ON SERVER READ HTTP_ERROR");
			num_blinks = 2;

			for (short i = 0; i < num_blinks; ++i) {
				setStatusLEDYellow();
				vTaskDelay(pdMS_TO_TICKS(100));
				setStatusLEDBlue();
				vTaskDelay(pdMS_TO_TICKS(100));
			}

		}
		if (WIFI_CONNECTED && UPLOADING) {
			ESP_LOGI(TAG, "SYSTEM UPLOAD IN PROGRESS");
			setStatusLEDYellow();
		}
		if (WIFI_CONNECTED) {
			setStatusLEDBlue();
		}
		if (!WIFI_CONNECTED) {
			setStatusLEDGreen();
		}
		if (HOMING) {
			setStatusLEDYellow();
		}

		// the frequency of the light indicator in MS
		vTaskDelay(pdMS_TO_TICKS(1000));
		xTaskNotify(xHandleStatusLEDWatchdog, 1, eSetValueWithOverwrite);		
	}

}

void vTaskPollServer(void * args) {
	char *TAG = "vTaskPollServer";

	while(true) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		// wrap in while loop for faster exit in event disconnect (shouldn't be needed but safety)
		while (xHandleSubmitLocalData != NULL && eTaskGetState(xHandleSubmitLocalData) != eReady && eTaskGetState(xHandleSubmitLocalData) != eRunning) {
			if (WIFI_CONNECTED && DATETIME_SYNC) { 
				if (httpFetchServerData() != ESP_OK) {
					HTTP_ERROR = true;
					initializeHttpClient();
				} else {
					HTTP_ERROR = false;
				}
				if (!HTTP_ERROR && DATETIME_SYNC) {

					if (httpParseServerData() == ESP_OK) {
						SYS_SYNC = true;
						if (xHandleUpdateMotor) {
							portENTER_CRITICAL(&mux);
							vTaskDelete(xHandleUpdateMotor);
							xHandleUpdateMotor = NULL;
							// update local nvs value in case the task was mid-operation
							nvsWriteBlob("init", "MOTOR_STEPS", &MOTOR_POSITION_STEPS, sizeof(long));
							xTaskCreate(vTaskUpdateMotor, "vTaskUpdateMotor", 4096, NULL, 9, &xHandleUpdateMotor);
							portEXIT_CRITICAL(&mux);
							xTaskNotify(xHandleUpdateMotor, 1, eSetValueWithoutOverwrite);
							
						}
					}
					
				}

			}
			break;
			// vTaskDelay(pdMS_TO_TICKS(1000));
		}
		HTTP_ERROR = false;

		// ESP_LOGI(TAG, "REQUEST CANCELED BY HIGHER PRIORITY TASK (xHandleSubmitLocalData)");
		// vTaskDelay(1);	
		
		// if (xHandlePollServer) {
		// 	xTaskNotify(xHandlePollServer, 1, eSetValueWithoutOverwrite);
		// }


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

void vTaskSubmitLocalData(void *args) {
	char *TAG = "vTaskSubmitLocalData";

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		cJSON_Hooks hooks;
		hooks.malloc_fn = pvPortMalloc;
		hooks.free_fn = vPortFree;
		cJSON_InitHooks(&hooks);

		// if notified of a local move update, then update the local nvs values as well.
		nvsWriteBlob("init", "MOTOR_STEPS", &MOTOR_POSITION_STEPS, sizeof(long));
		nvsWriteBlob("init", "CURTAIN_PERC", &CURTAIN_PERCENTAGE, sizeof(float));

		// cancel request if the system is not synced with the server
		if (!SYS_SYNC) {
			continue;
		}

		portENTER_CRITICAL(&mux);
		if (formSent) {
			cJSON_Delete(formSent);
			formSent = NULL;
		}
		portEXIT_CRITICAL(&mux); 
		formSent = cJSON_CreateObject();

		cJSON_AddStringToObject(formSent, "WRITE_KEY", WRITE_KEY);
		cJSON_AddStringToObject(formSent, "DeviceID", LOCAL_DEVICE_ID);
		cJSON_AddStringToObject(formSent, "USERNAME", USERNAME);
		cJSON_AddNumberToObject(formSent, "CURTAIN_PERCENTAGE", CURTAIN_PERCENTAGE);
		// // // https://www.cplusplus.com/reference/cstdio/sprintf/
		// // char buffer[512];
		// // sprintf(buffer, "%f", CURTAIN_PERCENTAGE);
		// // cJSON_AddStringToObject(formSent, "CURTAIN_PERCENTAGE", buffer);

		portENTER_CRITICAL(&mux); 
		if (jsonStringBuffer) {
			cJSON_free(jsonStringBuffer);
			jsonStringBuffer = NULL;
		}
		portEXIT_CRITICAL(&mux);
		jsonStringBuffer = cJSON_PrintUnformatted(formSent);

		esp_err_t err = httpPostData(jsonStringBuffer);
		// only try to validate the data if the http request went through.
		// otherwise parsing of the http response will fail.
		if (err == ESP_OK) {
			// enter critical section to avoid shared json resource becoming corrupt during validation
			// portENTER_CRITICAL(&mux); 
			err = httpValidateFormSubmission(jsonStringBuffer);
			// portEXIT_CRITICAL(&mux);
		}
		if (err == ESP_OK) {
			portENTER_CRITICAL(&mux); 
			if (jsonStringBuffer) {
				cJSON_free(jsonStringBuffer);
				jsonStringBuffer = NULL;
			}
			portEXIT_CRITICAL(&mux); 
			jsonStringBuffer = cJSON_PrintUnformatted(formSent);
			ESP_LOGI(TAG, "REQUEST SUCCESSFULL -- %s", jsonStringBuffer);
			
			// if (xHandleWifiPersistingTasks) {
			// 	vTaskDelay(pdMS_TO_TICKS(5000));
			// 	xTaskNotify(xHandleWifiPersistingTasks, 1, eSetValueWithoutOverwrite);
			// }
			
			if (xHandleWifiPersistingTasks) {
				vTaskDelay(pdMS_TO_TICKS(5000));
				vTaskResume(xHandleWifiPersistingTasks);
			}
			
			// if (xHandlePollServer && eTaskGetState(xHandlePollServer) == eSuspended) {
			// 	// reinitialize the deleted task and notify it to resume
			// 	ESP_LOGI(TAG, "RESUMING TASK vTaskPollWebServer");
			// 	vTaskResume(xHandlePollServer);
			// 	xTaskNotify(xHandlePollServer, 1, eSetValueWithoutOverwrite);
			// }
			// if (xHandlePollServer) {
			// 	xTaskNotify(xHandlePollServer, 1, eSetValueWithoutOverwrite);
			// }
		} else {
			ESP_LOGI(TAG, "REQUEST FAILED. RETRYING...");
			initializeHttpClient();
			// esp_task_wdt_reset();
			xTaskNotify(xHandleSubmitLocalData, 1, eSetValueWithoutOverwrite);
		}

		portENTER_CRITICAL(&mux);
		if (xHandlePollServer == NULL) {
			xTaskCreate(vTaskPollServer, "vTaskPollServer", 4096, NULL, 10, &xHandlePollServer);
			configASSERT(xHandlePollServer);
			xTaskNotify(xHandlePollServer, 1, eSetValueWithOverwrite);
		}
		portEXIT_CRITICAL(&mux);

		vTaskDelay(1);
		esp_task_wdt_reset();
		esp_task_wdt_delete(xHandleSubmitLocalData);
	}


}

void vTaskMoveStepperForward(void * pvPerameters) {
	char *TAG = "vTaskMoveStepperForward";
	float length_mm = CURTAIN_LENGTH_INCH * 25.4;
	float circumference_mm = 2 * M_PI * (ROD_DIAMETER_MM/2);
	int MOTOR_STEP_LIMIT = StepperMotor_1->Config->microstepping * StepperMotor_1->Config->motor_steps * (length_mm/circumference_mm);
	int interval = calcStepsForRotation(StepperMotor_1, 1);

	while(true) {
		// Block and wait for message to unlock
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		// Schedule task to wdt, so we can reset timeout periodically.
		esp_task_wdt_add(xHandleMoveStepperForward);

		if (xHandleSubmitLocalData && (eTaskGetState(xHandleSubmitLocalData) == eRunning || eTaskGetState(xHandleSubmitLocalData) == eReady) ) {
			xTaskNotify(xHandleSubmitLocalData, 0, eSetValueWithOverwrite);
		}

		if (xHandleWifiPersistingTasks) {
			xTaskNotify(xHandleWifiPersistingTasks, 0, eSetValueWithOverwrite);
		}

		// if (xHandlePollServer && eTaskGetState(xHandlePollServer) != eSuspended) {
		// 	vTaskSuspend(xHandlePollServer);
		// }

		// if (xHandleWifiPersistingTasks && eTaskGetState(xHandleWifiPersistingTasks) != eSuspended) {
		// 	vTaskSuspend(xHandleWifiPersistingTasks);
		// }

		// TODO: make this account for the material thickness around the rod dynamically
		length_mm = CURTAIN_LENGTH_INCH * 25.4;
		circumference_mm = 2 * M_PI * (ROD_DIAMETER_MM/2);
		MOTOR_STEP_LIMIT = StepperMotor_1->Config->microstepping * StepperMotor_1->Config->motor_steps * (length_mm/circumference_mm);
		
		if (HOMING) {
			MOTOR_STEP_LIMIT = INT_MAX;
		}
		portENTER_CRITICAL(&mux);
		if (MOTOR_POSITION_STEPS < MOTOR_STEP_LIMIT) {
			// keep moving motor in intervals unless it is smaller than a single interval period
			if (interval < MOTOR_STEP_LIMIT - MOTOR_POSITION_STEPS) {
				move(StepperMotor_1, interval);
				MOTOR_POSITION_STEPS += interval;
			} else {
				move(StepperMotor_1, MOTOR_STEP_LIMIT - MOTOR_POSITION_STEPS);
				MOTOR_POSITION_STEPS += MOTOR_STEP_LIMIT - MOTOR_POSITION_STEPS;
			}
			if (!HOMING) {
				// update globals (motor_position_steps / maximum_step_length)
				CURTAIN_PERCENTAGE = MOTOR_POSITION_STEPS / (float)calcStepsForRotation(StepperMotor_1, (length_mm / circumference_mm) * 360) * 100;
			}
		}
		portEXIT_CRITICAL(&mux);

		// Reset the wdt from this running task
		esp_task_wdt_reset();

		if (xHandleSubmitLocalData) {
			xTaskNotify(xHandleSubmitLocalData, 1, eSetValueWithOverwrite);
		}

		// block this task for just a moment to allow other ready tasks not to starve out the watchdog.
		// vTaskDelay(1);

		// unschedule/delete the task from wdt, so blocking does not cause timeout of wdt
		esp_task_wdt_delete(xHandleMoveStepperForward);
	}

}

void vTaskMoveStepperReverse(void * pvPerameters) {
	char *TAG = "vTaskMoveStepperReverse";
	int	length_mm = CURTAIN_LENGTH_INCH * 25.4;
	int	circumference_mm = 2 * M_PI * (ROD_DIAMETER_MM/2);
	int MOTOR_STEP_LIMIT = StepperMotor_1->Config->microstepping * StepperMotor_1->Config->motor_steps * (length_mm/circumference_mm);
	int interval = calcStepsForRotation(StepperMotor_1, 1);

	while(1) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
		esp_task_wdt_add(xHandleMoveStepperReverse);

		if (xHandleSubmitLocalData && (eTaskGetState(xHandleSubmitLocalData) == eRunning || eTaskGetState(xHandleSubmitLocalData) == eReady) ) {
			xTaskNotify(xHandleSubmitLocalData, 0, eSetValueWithOverwrite);
		}

		// if (xHandlePollServer && eTaskGetState(xHandlePollServer) != eSuspended) {
		// 	vTaskSuspend(xHandlePollServer);
		// }

		// TODO: make this account for the material thickness around the rod dynamically
		length_mm = CURTAIN_LENGTH_INCH * 25.4;
		circumference_mm = 2 * M_PI * (ROD_DIAMETER_MM/2);
		MOTOR_STEP_LIMIT = StepperMotor_1->Config->microstepping * StepperMotor_1->Config->motor_steps * (length_mm/circumference_mm);
		
		// Reset the wdt from this running task
		esp_task_wdt_reset();
		portENTER_CRITICAL(&mux);
		if (MOTOR_POSITION_STEPS > 0) {
			// keep moving motor in intervals unless it is smaller than a single interval period
			if (interval < MOTOR_POSITION_STEPS) {
				move(StepperMotor_1, -interval);
				MOTOR_POSITION_STEPS -= interval;
			} else {
				move(StepperMotor_1, -MOTOR_POSITION_STEPS);
				MOTOR_POSITION_STEPS -= MOTOR_POSITION_STEPS;
			}
			CURTAIN_PERCENTAGE = MOTOR_POSITION_STEPS / (float)calcStepsForRotation(StepperMotor_1, (length_mm / circumference_mm) * 360) * 100;
		}
		portEXIT_CRITICAL(&mux);	

		if (xHandleSubmitLocalData) {
			xTaskNotify(xHandleSubmitLocalData, 1, eSetValueWithOverwrite);
		}
		// block this task for just a moment to allow other ready tasks not to starve out the watchdog.
		// vTaskDelay(1);

		// unschedule/delete the task from wdt, so blocking does not cause timeout of wdt
		esp_task_wdt_delete(xHandleMoveStepperReverse);
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

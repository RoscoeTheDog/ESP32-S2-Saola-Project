#include <espInterrupts.h>
// #include <configGpio.h>
// #include <StepperDriver.h>
// #include <configSteppers.h>
// #include <rtosTasks.h>
// #include <freertos/task.h>
// #include <freertos/FreeRTOS.h>

BaseType_t xHigherPriorityTaskWoken = pdTRUE;
volatile bool BTN_0_PIN_STATE = false;
volatile bool BTN_1_PIN_STATE = false;
volatile bool LIMIT_SWITCH_STATE = false;
volatile bool SMARTCONFIG_SWITCH_STATE = false;

void updateButtonsState() {
	// do this once every interrupt, as to avoid checking the pin logic multiple times.
	LIMIT_SWITCH_STATE = gpio_get_level(LIMIT_SWITCH_PIN);
	BTN_0_PIN_STATE = gpio_get_level(BTN_0_INPUT_PIN);
	BTN_1_PIN_STATE = gpio_get_level(BTN_1_INPUT_PIN);
	SMARTCONFIG_SWITCH_STATE = gpio_get_level(SMARTCONFIG_PAIR_SWITCH);
}

// All interrupts must be declared as a boolean to signify if they yield or not.
// IRAM_ATTR flag tells the compiler to keep it in internal memory rather than flash. It is much faster this way.
// *args are not neccessary, but can be used to pass in an object with multiple arguments.
inline bool IRAM_ATTR xISR_button_0(void * args) {
	char *TAG = "xISR_button_0";
	// Poll the buttons for any changes.
	updateButtonsState();
	
// 	if(SMARTCONFIG_SWITCH_STATE) {

// 		// if (!ESP_SMARTCONFIG_STATUS && RADIO_INITIALIZED) {

// 		// 	// if (!RADIO_INITIALIZED) {
// 		// 	// 	initializeWifi();
// 		// 	// }
// 		// 	esp_smartconfig_set_type(SC_TYPE_ESPTOUCH);
// 		// 	smartconfig_start_config_t cfg = {.enable_log = false};

// 		// 	esp_err_t err = esp_smartconfig_start(&cfg);
// 		// 	// ESP_ERROR_CHECK(err);
// 		// 	if (err == ESP_OK) {
// 		// 		ESP_SMARTCONFIG_STATUS = true;
// 		// 	}
// 		// }

// 		if (xHandleSmartConfig && (eTaskGetState(xHandleSmartConfig) != eRunning || eTaskGetState(xHandleSmartConfig) != eReady)) {
// 			xTaskNotify(xHandleSmartConfig, 1, eSetValueWithOverwrite);
// 		}
		

// 		// if (!ESP_SMARTCONFIG_STATUS) {

// 		// 	if (!RADIO_INITIALIZED) {
// 		// 		initializeWifi();u
// 		// 	}
// 		// 	ESP_LOGI(TAG, "configuring smartconfig service");
// 		// 	ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
// 		// 	smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();

// 		// 	ESP_LOGI(TAG, "starting smartconfig service");
// 		// 	esp_err_t err = esp_smartconfig_start(&cfg);
// 		// 	ESP_ERROR_CHECK(err);
// 		// 	if (err == ESP_OK) {
// 		// 		ESP_SMARTCONFIG_STATUS = true;
// 		// 		ESP_LOGI(TAG, "smartconfig service started successfully!");
// 		// 	}
			
// 	} else {
// ;
// 		if (xHandleSmartConfig && (eTaskGetState(xHandleSmartConfig) == eRunning || eTaskGetState(xHandleSmartConfig) == eReady)) {
// 			xTaskNotify(xHandleSmartConfig, 0, eSetValueWithOverwrite);
// 		}

// 		// esp_err_t err = esp_smartconfig_stop();
// 		// ESP_ERROR_CHECK(err);
// 		// if (err == ESP_OK) {
// 		// 	ESP_LOGI(TAG, "smartconfig service stopped!");
// 		// 	ESP_SMARTCONFIG_STATUS = false;
// 		// }
// 	}
	
	
	if (LIMIT_SWITCH_STATE) {

		if (xHandleMoveStepperForward) {
			xTaskNotifyFromISR(xHandleMoveStepperForward, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		}

		if (xHandleHomeCurtains) {
			xTaskNotifyFromISR(xHandleHomeCurtains, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		}

		float length_mm = CURTAIN_LENGTH_INCH * 25.4;
		float circumference_mm = 2 * M_PI * (ROD_DIAMETER_MM/2);
		int MOTOR_STEP_LIMIT = StepperMotor_1->Config->microstepping * StepperMotor_1->Config->motor_steps * (length_mm/circumference_mm);
		CURTAIN_PERCENTAGE = 100;
		HOMING = false;
	}

	if (BTN_0_PIN_STATE && !BTN_1_PIN_STATE){
		setLEDHigh();

		// immediately suspend motor tasks if running.
		if (xHandleUpdateMotor && (eTaskGetState(xHandleUpdateMotor) == eRunning || eTaskGetState(xHandleUpdateMotor) == eReady) ) {
			nvsWriteBlob("init", "MOTOR_STEPS", &MOTOR_POSITION_STEPS, sizeof(long));
			vTaskDelete(xHandleUpdateMotor);
			xTaskCreate(vTaskUpdateMotor, "vTaskUpdateMotor", 4096, NULL, 9, &xHandleUpdateMotor);
		}

		if (xHandleHomeCurtains) {
			xTaskNotifyFromISR(xHandleHomeCurtains, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			vTaskDelete(xHandleHomeCurtains);
			xHandleHomeCurtains = NULL;
		}
		
		if (xHandleMoveStepperForward) {
			// Notify task to rotate the motor. Incriment by two in case it finishes before the buttons state is updated.
			xTaskNotifyFromISR(xHandleMoveStepperForward, 1, eIncrement, &xHigherPriorityTaskWoken);
		}
		
	}

	// TODO: Button 2
	if (BTN_1_PIN_STATE && !BTN_0_PIN_STATE) {
		// Update the duty cycle of the LED PWM
		setLEDHigh();

		// immediately suspend motor tasks if running.
		if (xHandleUpdateMotor && (eTaskGetState(xHandleUpdateMotor) == eRunning || eTaskGetState(xHandleUpdateMotor) == eReady) ) {
			nvsWriteBlob("init", "MOTOR_STEPS", &MOTOR_POSITION_STEPS, sizeof(long));
			vTaskDelete(xHandleUpdateMotor);
			xTaskCreate(vTaskUpdateMotor, "vTaskUpdateMotor", 4096, NULL, 9, &xHandleUpdateMotor);
		}

		if (xHandleHomeCurtains) {
			xTaskNotifyFromISR(xHandleHomeCurtains, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			vTaskDelete(xHandleHomeCurtains);
			xHandleHomeCurtains = NULL;
		}

		// if (xHandleMoveStepperForward && (eTaskGetState(xHandleMoveStepperForward) != eRunning || eTaskGetState(xHandleMoveStepperForward) != eReady) ) {
		// 	xTaskNotifyFromISR(xHandleMoveStepperForward, 0, eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken);
		// }

		if (xHandleMoveStepperReverse) {
			// Notify task to rotate the motor. Incriment by two in case it finishes before the buttons state is updated.
			xTaskNotifyFromISR(xHandleMoveStepperReverse, 1, eIncrement, &xHigherPriorityTaskWoken);
		}
	}

	// When both buttons are released...
	if (!BTN_0_PIN_STATE && !BTN_1_PIN_STATE) {
		
		if(xHandleHomeCurtains && (eTaskGetState(xHandleHomeCurtains) != eRunning || eTaskGetState(xHandleHomeCurtains) != eReady) ) {
			
			if (xHandleMoveStepperForward && (eTaskGetState(xHandleMoveStepperForward) == eRunning || eTaskGetState(xHandleMoveStepperForward) == eReady) ) {
				xTaskNotifyFromISR(xHandleMoveStepperForward, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			}

			if (xHandleMoveStepperReverse && (eTaskGetState(xHandleMoveStepperReverse) == eRunning || eTaskGetState(xHandleMoveStepperReverse) == eReady) ) {
				xTaskNotifyFromISR(xHandleMoveStepperReverse, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			} 

		}

		if(xHandleUpdateMotor && (eTaskGetState(xHandleUpdateMotor) != eRunning || eTaskGetState(xHandleUpdateMotor) != eReady ) ) {

			if (xHandleMoveStepperForward && (eTaskGetState(xHandleMoveStepperForward) == eRunning || eTaskGetState(xHandleMoveStepperForward) == eReady) ) {
				xTaskNotifyFromISR(xHandleMoveStepperForward, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			}

			if (xHandleMoveStepperReverse && (eTaskGetState(xHandleMoveStepperReverse) == eRunning || eTaskGetState(xHandleMoveStepperReverse) == eReady) ) {
				xTaskNotifyFromISR(xHandleMoveStepperReverse, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			} 
			
		}
		


		// if (xHandleMoveStepperForward != NULL && (eTaskGetState(xHandleMoveStepperForward) == eRunning || eTaskGetState(xHandleMoveStepperForward) == eReady) ) {
		// 	// clear whatever counts remain on the running forward/reverse tasks
		// 	// xTaskNotify(xHandleMoveStepperForward, 1, eSetValueWithOverwrite);
		// 	stop(StepperMotor_1);
		// }


		// if (xHandleMoveStepperReverse != NULL && (eTaskGetState(xHandleMoveStepperReverse) == eRunning || eTaskGetState(xHandleMoveStepperReverse) == eReady) ) {
		// 	// clear whatever counts remain on the running forward/reverse tasks
		// 	xTaskNotify(xHandleMoveStepperReverse, 1, eSetValueWithOverwrite);
		// }

		// if (xHandlePollServer == NULL) {
		// 	xTaskCreate(vTaskPollServer, "vTaskPollServer", 4096, NULL, 10, &xHandlePollServer);
		// 	xTaskNotify(xHandlePollServer, 1, eSetValueWithOverwrite);
		// 	configASSERT(xHandlePollServer);
		// }

		// if (xHandlePollServer) {
		// 	// vTaskDelete(xHandlePollServer);
		// 	xTaskNotify(xHandlePollServer, 1, eSetValueWithoutOverwrite);
		// }

		// Check if any prioritized tasks are running.
		// if (eTaskGetState(xHandleCloseCurtains) != eRplosunning && eTaskGetState(xHandlePollServer) != eRunning) {
		// 	// Immediate stop stepper from running tasks.
		// 	// stop(StepperMotor_1);	
		// 	;
		// }

		// See if LED is on/off
		if (getLEDState()) {
			BaseType_t xHigherPriorityTaskWoken = pdTRUE;
			// Signal to task to start fading.
			xTaskNotifyFromISR(xHandleLEDFade, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		}

	}

	if (BTN_0_PIN_STATE && BTN_1_PIN_STATE) {
		
		// set initial value if not done
		if(!BUTTON_HOLD_TIMER) {
			BUTTON_HOLD_TIMER = esp_timer_get_time();
		}

		// TODO: FIX DEBOUNCING ISSUES.
		// check if buttons have been held 5 seconds
		if (esp_timer_get_time() - BUTTON_HOLD_TIMER > 5 * 1000000) {

			if (!LIMIT_SWITCH_STATE) {
				HOMING = true;
				xTaskNotify(xHandleMoveStepperForward, ULONG_MAX - 1, eSetValueWithOverwrite);
			}

		}

	} else {
		// reset the timer if both buttons are unpressed
		BUTTON_HOLD_TIMER = 0;
	}

	return true;
}

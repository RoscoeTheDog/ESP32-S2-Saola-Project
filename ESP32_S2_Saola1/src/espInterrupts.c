#include <espInterrupts.h>
// #include <configGpio.h>
// #include <StepperDriver.h>
// #include <configSteppers.h>
// #include <rtosTasks.h>
// #include <freertos/task.h>
// #include <freertos/FreeRTOS.h>

BaseType_t xHigherPriorityTaskWoken = pdTRUE;
volatile bool BTN_0_PIN_STATE;
volatile bool BTN_1_PIN_STATE;

void updateButtonsState() {
	// do this once every interrupt, as to avoid checking the pin logic multiple times.
	BTN_0_PIN_STATE = gpio_get_level(BTN_0_INPUT_PIN);
	BTN_1_PIN_STATE = gpio_get_level(BTN_1_INPUT_PIN);
}

// All interrupts must be declared as a boolean to signify if they yield or not.
// IRAM_ATTR flag tells the compiler to keep it in internal memory rather than flash. It is much faster this way.
// *args are not neccessary, but can be used to pass in an object with multiple arguments.
extern inline bool IRAM_ATTR xISR_button_0(void * args) {
	char *TAG = "xISR_button_0";
	// Poll the buttons for any changes.
	updateButtonsState();

	if (BTN_0_PIN_STATE){
		setLEDHigh();

		// immediately suspend motor tasks if running.
		if (xHandleUpdateMotor != NULL && (eTaskGetState(xHandleUpdateMotor) == eRunning || eTaskGetState(xHandleUpdateMotor) == eReady) ) {
			nvsWriteBlob("init", "MOTOR_STEPS", &MOTOR_POSITION_STEPS, sizeof(long));
			vTaskDelete(xHandleUpdateMotor);
			xTaskCreate(vTaskUpdateMotor, "vTaskUpdateMotor", 4096, NULL, 9, &xHandleUpdateMotor);
		}
		
		if (xHandleMoveStepperForward != NULL) {
			// Notify task to rotate the motor. Incriment by two in case it finishes before the buttons state is updated.
			xTaskNotifyFromISR(xHandleMoveStepperForward, 1, eIncrement, &xHigherPriorityTaskWoken);
		}
		
	}

	// TODO: Button 2
	if (BTN_1_PIN_STATE) {
		// Update the duty cycle of the LED PWM
		setLEDHigh();

		// immediately suspend motor tasks if running.
		if (xHandleUpdateMotor != NULL && (eTaskGetState(xHandleUpdateMotor) == eRunning || eTaskGetState(xHandleUpdateMotor) == eReady) ) {
			nvsWriteBlob("init", "MOTOR_STEPS", &MOTOR_POSITION_STEPS, sizeof(long));
			vTaskDelete(xHandleUpdateMotor);
			xTaskCreate(vTaskUpdateMotor, "vTaskUpdateMotor", 4096, NULL, 9, &xHandleUpdateMotor);
		}

		if (xHandleMoveStepperReverse != NULL) {
			// Notify task to rotate the motor. Incriment by two in case it finishes before the buttons state is updated.
			xTaskNotifyFromISR(xHandleMoveStepperReverse, 1, eIncrement, &xHigherPriorityTaskWoken);
		}
	}

	// When both buttons are released...
	if (!BTN_0_PIN_STATE && !BTN_1_PIN_STATE) {

		if (xHandleMoveStepperForward && (eTaskGetState(xHandleMoveStepperForward) == eRunning || eTaskGetState(xHandleMoveStepperForward) == eReady) ) {
			xTaskNotifyFromISR(xHandleMoveStepperForward, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		}

		if (xHandleMoveStepperReverse && (eTaskGetState(xHandleMoveStepperReverse) == eRunning || eTaskGetState(xHandleMoveStepperReverse) == eReady) ) {
			xTaskNotifyFromISR(xHandleMoveStepperReverse, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
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
		// if (eTaskGetState(xHandleCloseCurtains) != eRunning && eTaskGetState(xHandlePollServer) != eRunning) {
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

	return true;
}

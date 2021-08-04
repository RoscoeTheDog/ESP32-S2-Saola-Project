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

	// Poll the buttons for any changes.
	updateButtonsState();

	if (BTN_0_PIN_STATE){
		// TODO: test this code on esp32 not S2 version and use hardware on ledc peripherial.
		
		// the low-speed software based peripherial on esp32-s2 seems to not multiplex while async tasks are running (cpu starved?)
		// if (LEDC_CHANNEL_0_DUTY < 2047) {
			// Update the duty cycle of the LED PWM
			setLEDHigh();
		// }
		// Notify task to rotate the motor. Incriment by two in case it finishes before the buttons state is updated.
		xTaskNotifyFromISR(xHandleCurtainStepperForward, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
	}

	// TODO: Button 2
	if (BTN_1_PIN_STATE) {
		// Update the duty cycle of the LED PWM
		setLEDHigh();
		// Notify task to rotate the motor. Incriment by two in case it finishes before the buttons state is updated.
		xTaskNotifyFromISR(xHandleCurtainStepperReverse, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
	}

	// When both buttons are released...
	if (!BTN_0_PIN_STATE && !BTN_1_PIN_STATE) {

		// // Check if any prioritized tasks are running.
		// if (eTaskGetState(xHandleCloseCurtains) != eRunning && eTaskGetState(xHandlePollWebServer) != eRunning) {
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

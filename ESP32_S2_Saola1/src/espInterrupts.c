#include <espInterrupts.h>
#include <configGpio.h>
#include <StepperDriver.h>
#include <configSteppers.h>
#include <rtosTasks.h>

extern inline bool IRAM_ATTR xISR_button_0(void * args) {

	if (gpio_get_level(BTN_0_PIN)){
// Check the switch driver type. Change the output logic accordingly.
#ifdef BTN_0_LED_DRIVER_N
		LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
#endif
#ifdef BTN_0_LED_DRIVER_P
		LEDC_CHANNEL_0_DUTY = 0;
#endif
		// xTimerStop(xHandleTimerLED, 0);
		ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY, 0);
		BaseType_t xHigherPriorityTaskWoken = pdTRUE;
		xTaskNotifyFromISR(xHandleCurtainStepper, 1, eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken);
		// rotate(&stepperMotor_1, 1); // TODO: make this asyncronous. just give some large amount of steps. We will invoke stop when done.
	}

	// TODO: Button 2
	// if (gpio_get_level(BTN_1_PIN)) {
	// 	LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
	// 	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY);		
	// 	// StepperMotorTest.rotate(1);
	// }

	if (!gpio_get_level(BTN_0_PIN)) {
#ifdef BTN_0_LED_DRIVER_N

		// xTimerStart(xHandleTimerLED, 0);

		// stop(&stepperMotor_1);	// Immediate stop and clear steps. Returns leftover steps if needed.

		// if (LEDC_CHANNEL_0_DUTY > 0) {
		// 	vUpdateLEDFade();
		// }

		if (LEDC_CHANNEL_0_DUTY == LEDC_CHANNEL_0_DUTY_MAX) {
			BaseType_t xHigherPriorityTaskWoken = pdTRUE;
			// xTaskNotify(xHandleLEDFade, 1, eSetValueWithOverwrite);
			xTaskNotifyFromISR(xHandleLEDFade, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			stop(&stepperMotor_1);
		}

		portYIELD_FROM_ISR();
		
#endif
#ifdef BTN_0_LED_DRIVER_P
		vUpdateLEDFade();
		if (LEDC_CHANNEL_0_DUTY == 0) {
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			// xTaskNotify(xHandleLEDFade, 1, eSetValueWithOverwrite);
			xTaskNotifyFromISR(xHandleLEDFade, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR();
		}
#endif
	}

	return false;
}

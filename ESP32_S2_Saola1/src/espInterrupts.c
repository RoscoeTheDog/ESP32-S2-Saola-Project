#include <espInterrupts.h>
#include <configGpio.h>

extern inline bool IRAM_ATTR xISR_button_0(void * args) {

	if (gpio_get_level(BTN_0_PIN)){
// Check the switch driver type. Change the output logic accordingly.
#ifdef BTN_0_LED_DRIVER_N
		LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
#endif
#ifdef BTN_0_LED_DRIVER_P
		LEDC_CHANNEL_0_DUTY = 0;
#endif
		// StepperMotorTest.rotate(1);
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY);
	}

	// if (gpio_get_level(BTN_1_PIN)) {
	// 	LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
	// 	ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY);		
	// 	// StepperMotorTest.rotate(1);
	// }

	if (!gpio_get_level(BTN_0_PIN)) {
#ifdef BTN_0_LED_DRIVER_N
		if (LEDC_CHANNEL_0_DUTY == LEDC_CHANNEL_0_DUTY_MAX) {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		// xTaskNotify(xHandleLEDFade, 1, eSetValueWithOverwrite);
		xTaskNotifyFromISR(xHandleLEDFade, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR();
		}
#endif
#ifdef BTN_0_LED_DRIVER_P
		if (LEDC_CHANNEL_0_DUTY == 0) {
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			// xTaskNotify(xHandleLEDFade, 1, eSetValueWithOverwrite);
			xTaskNotifyFromISR(xHandleLEDFade, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR();
		}
#endif
	}
	
	return true;
}

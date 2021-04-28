#include <espInterrupts.h>

extern inline bool IRAM_ATTR xISR_button_0(void * args) {

	if (gpio_get_level(BTN_0_PIN)){
		LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY_MAX);
		// StepperMotorTest.rotate(1);
	}

	if (gpio_get_level(BTN_1_PIN)) {
		LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY_MAX);
		
		// StepperMotorTest.rotate(1);
	}

	if ((int)LEDC_CHANNEL_0_DUTY == LEDC_CHANNEL_0_DUTY_MAX){
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		// xTaskNotify(xHandleLEDFade, 1, eSetValueWithOverwrite);
		xTaskNotifyFromISR(xHandleLEDFade, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR();
	}

	return true;
}

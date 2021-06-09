/*
	Include C++ headers here.

	If you want to use C++ libraries, they must be included in this CPP OR explicitely wrapped as extern "C++" {} so the compiler knows what conventions to use
	and how to link them.
*/

/*
	Tell the compiler to wrap main as C convention since most of the IDF framework in standard C.
*/

extern "C" {
	// Include C-headers here.
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <configGpio.h>
	#include <configLedc.h>
	#include <configTimers.h>
	#include <configSteppers.h>
	#include <esp_timer.h>

    void app_main(void);
}

void app_main(void) {
	// Call all ESP-IDF initializers first.
	vInitTaskRTOSDebug();
	vInitGPIO();
	vInitLEDC_0();
	vInitTaskLEDFade();
	vInitCurtainStepper();
	vInitTaskCurtainStepper();
	vInitTaskCloseCurtains();
	vInitTimer_0();	// starts button interrupts.
	xTaskNotify(xHandleCloseCurtains, 1, eSetValueWithOverwrite);

	StepperConfig_t *sc = (StepperConfig_t*)malloc(sizeof(StepperConfig_t));
	printf("sizeof: %u", sizeof(StepperConfig_t));

	sc->step_pin = STEP_PIN;
	sc->direction_pin = DIR_PIN;
	sc->enable_pin = EN_PIN;
	sc->enable_active_state = 1;
	sc->motor_steps = 200;
	sc->rpm = 200;
	sc->microstepping = 64;
	sc->mode = CONSTANT_SPEED;

	StepperHandle_t *stepper = createStepperHandler(sc);

	// bug is not caused by interrupts, tested like this:
	while (1) {
		rotate(stepper, 360);
		vTaskDelay(pdMS_TO_TICKS(3 * 1000));
		// printf("step_pulse: %lo\n", stepper->step_pulse);
		// printf("steps_remaining: %lo\n", stepper->steps_remaining);
		// printf("step_count: %lo\n", stepper->step_count);
		xTaskNotify(xHandleRTOSDebug, 1, eSetValueWithOverwrite);
	}

}


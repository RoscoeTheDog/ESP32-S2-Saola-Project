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
	// Call all initializers first.

	// NOTE: For debugging the stepper driver issue, disable all other microcontroller features/tasks
	//		 except for what is required (like initializing output pins).

	// vInitTaskRTOSDebug();
	vInitGPIO();
	// vInitLEDC_0();
	// vInitTaskLEDFade();
	// vInitCurtainStepper();
	// vInitTaskCurtainStepper();
	// vInitTaskCloseCurtains();
	// vInitTimer_0();	// starts button interrupts.
	// xTaskNotify(xHandleCloseCurtains, 1, eSetValueWithOverwrite);

	// Allocated and create a pointer to a stepper config struct.
	StepperConfig_t *sc = (StepperConfig_t*)malloc(sizeof(StepperConfig_t));
	// set the used pins and the settings for the motor.
	sc->step_pin = STEP_PIN;
	sc->direction_pin = DIR_PIN;
	sc->enable_pin = EN_PIN;
	sc->enable_active_state = 1;
	sc->motor_steps = 200;
	sc->rpm = 200;
	sc->microstepping = 64;
	sc->mode = CONSTANT_SPEED;

	// pass the configuration to the create handle method. returns a pointer to a handle type.
	StepperHandle_t *stepper = createStepperHandler(sc);

	// rotate the motor 360 degrees then wait for 3 seconds.
	while (1) {
		rotate(stepper, 360);
		vTaskDelay(pdMS_TO_TICKS(3 * 1000));
		// ignore this. the below task can print Task information such as stack size etc but revealed anything useful.
		// xTaskNotify(xHandleRTOSDebug, 1, eSetValueWithOverwrite);
	}

}


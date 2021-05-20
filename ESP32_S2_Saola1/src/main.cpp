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
	vInitGPIO();
	// vInitLEDC_0();
	// vInitTimer_0();
	// vInitTaskLEDFade();
	vInitCurtainStepper();
	rotate(&stepperMotor_1, 360);
	// vInitTaskCurtainStepper();

	// while(1){
	// 	delayMicros(2000 * 1000, 0);
	// 	xTaskNotify(xHandleCurtainStepper, 1, eSetValueWithOverwrite);
	// 	// rotate(&stepperMotor_1, 1/32);
	// 	// vTaskDelay(1);
	// }

	// rotate(&stepperMotor_1, 360);


	// vInitTimerLEDFade(3000);
	// vSetLEDFadePeriod(3000);
	// vInitTaskTimer0(NULL);
	// vInitCurtainStepper();


	// StepperConfig_t stepperConfig;
	// stepperConfig.motor_steps = 200;
	// stepperConfig.direction_pin = DIR_PIN;
	// stepperConfig.enable_active_state = 1;
	// stepperConfig.enable_pin = EN_PIN;
	// stepperConfig.microstepping = 64;
	// stepperConfig.mode = CONSTANT_SPEED;
	// stepperConfig.rpm = 200;
	// stepperConfig.step_pin = STEP_PIN;
	// StepperHandler_t myStepper = createStepperHandler(&stepperConfig);

	// while(1) {
	// 	rotate(&myStepper, 360 * 5);
	// 	delayMicros(1000 * 3000, 0);
	// }
	// BasicStepperDriver S(200, DIR_PIN, DIR_PIN_SEL, STEP_PIN, STEP_PIN_SEL, EN_PIN, EN_PIN_SEL);

	// S.begin(200, 64);
	// S.rotate(360);
	// Call all FreeRTOS initializers after.
	// vInitTaskLEDFade();
	// vInitTaskRTOSDebug();	// use for debugging only. warning: slow and takes long time to process
	
}


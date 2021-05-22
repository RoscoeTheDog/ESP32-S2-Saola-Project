#include <configSteppers.h>

StepperHandler_t stepperMotor_1;

inline void vInitCurtainStepper() {

	StepperConfig_t stepperConfig;
	stepperConfig.step_pin = 1;
	stepperConfig.direction_pin = DIR_PIN;
	stepperConfig.enable_pin = EN_PIN;
	stepperConfig.enable_active_state = 1;
	stepperConfig.motor_steps = 200;
	stepperConfig.rpm = 200;
	stepperConfig.microstepping = 64;
	stepperConfig.mode = CONSTANT_SPEED;

	stepperMotor_1 = createStepperHandler(&stepperConfig);
}

#include <configSteppers.h>

StepperConfig_t *StepperConfig_1 = NULL;
StepperHandle_t *StepperMotor_1 = NULL;

void vInitCurtainMotorConfig_0() {
	StepperConfig_1 = (StepperConfig_t*)malloc(sizeof(StepperConfig_t));

	StepperConfig_1->step_pin = STEP_PIN;
	StepperConfig_1->direction_pin = DIR_PIN;
	StepperConfig_1->enable_pin = EN_PIN;
	StepperConfig_1->enable_active_state = 1;
	StepperConfig_1->motor_steps = 200;
	StepperConfig_1->rpm = 450;
	StepperConfig_1->microstepping = 64;
	StepperConfig_1->mode = CONSTANT_SPEED;

	StepperMotor_1 = createStepperHandle(StepperConfig_1);
}

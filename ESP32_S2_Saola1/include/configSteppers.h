#ifndef CONFIG_STEPPERDRIVER_H
#define CONFIG_STEPPERDRIVER_H

#include <StepperDriver.h>
#include <configGpio.h>

extern StepperHandle_t *StepperMotor_1;
extern StepperConfig_t *StepperConfig_1;

extern void vInitCurtainMotorConfig_0();

#endif

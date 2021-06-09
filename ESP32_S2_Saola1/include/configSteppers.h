#ifndef CONFIG_STEPPERDRIVER_H
#define CONFIG_STEPPERDRIVER_H

#include <StepperDriver.h>
#include <configGpio.h>

extern volatile StepperHandle_t *stepperMotor_1;
extern volatile StepperConfig_t *stepperConfig;

extern void vInitCurtainStepper();

#endif

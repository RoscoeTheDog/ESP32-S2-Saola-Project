#ifndef CONFIG_STEPPERDRIVER_H
#define CONFIG_STEPPERDRIVER_H

#include <StepperDriver.h>
#include <configGpio.h>

extern StepperHandler_t stepperMotor_1;
extern StepperConfig_t stepperConfig;

extern void vInitCurtainStepper();

#endif

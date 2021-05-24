#ifndef CONFIG_GPIO_H
#define CONFIG_GPIO_H

#include <driver/gpio.h>
#include "driverTypes.h"
#include "espInterrupts.h"

#define STEP_PIN GPIO_NUM_1
#define STEP_PIN_SEL GPIO_SEL_1

#define DIR_PIN GPIO_NUM_2
#define DIR_PIN_SEL GPIO_SEL_2

#define EN_PIN GPIO_NUM_0
#define EN_PIN_SEL GPIO_SEL_0

#define BTN_0_PIN GPIO_NUM_21
#define BTN_0_PIN_SEL GPIO_SEL_21
extern volatile bool BTN_0_PIN_STATE;

#define BTN_0_LED_PIN GPIO_NUM_20
#define BTN_0_LED_PIN_SEL GPIO_SEL_20
// if using a transistor or mosfet to switch a higher power source, uncomment the appropriate method.
// #define BTN_0_LED_DRIVER_P
#define BTN_0_LED_DRIVER_N

#define BTN_1_PIN GPIO_NUM_26
#define BTN_1_PIN_SEL GPIO_SEL_26
extern volatile bool BTN_1_PIN_STATE;

extern void vInitGPIO();

#endif

#ifndef CONFIG_SMARTBLINDS_H
#define CONFIG_SMARTBLINDS_H

#include <driver/gpio.h>
#include <driverTypes.h>

#define STEP_PIN GPIO_NUM_1
#define STEP_PIN_SEL GPIO_SEL_1

#define DIR_PIN GPIO_NUM_2
#define DIR_PIN_SEL GPIO_SEL_2

#define EN_PIN GPIO_NUM_0
#define EN_PIN_SEL GPIO_SEL_0

#define BTN_0_PIN GPIO_NUM_21
#define BTN_0_PIN_SEL GPIO_SEL_21

#define BTN_0_LED_PIN GPIO_NUM_20
#define BTN_0_LED_PIN_SEL GPIO_SEL_20

// if using a transistor or mosfet to switch a higher power source, uncomment the appropriate method.
// #define BTN_0_LED_DRIVER_P
#define BTN_0_LED_DRIVER_N

#define BTN_1_PIN GPIO_NUM_8
#define BTN_1_PIN_SEL GPIO_SEL_8

inline void vInitGPIO(){ 
	// Configure all GPIO here.
	// Note: avoid designated initializers since they do not play friendly with C++ syntax & compiler
	gpio_config_t STEP_CONF;
	STEP_CONF.pin_bit_mask = STEP_PIN_SEL;
	STEP_CONF.mode = GPIO_MODE_OUTPUT;
	STEP_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	STEP_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	STEP_CONF.intr_type = GPIO_INTR_DISABLE;

	gpio_config_t DIR_CONF;
	DIR_CONF.pin_bit_mask = DIR_PIN_SEL;
	DIR_CONF.mode = GPIO_MODE_OUTPUT;
	DIR_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	DIR_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	DIR_CONF.intr_type = GPIO_INTR_DISABLE;

	gpio_config_t EN_CONF;
	EN_CONF.pin_bit_mask = EN_PIN_SEL;
	EN_CONF.mode = GPIO_MODE_OUTPUT;
	EN_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	EN_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	EN_CONF.intr_type = GPIO_INTR_DISABLE;

	gpio_config_t BTN_0_CONF;
	BTN_0_CONF.pin_bit_mask = BTN_0_PIN_SEL;
	BTN_0_CONF.mode = GPIO_MODE_INPUT;
	BTN_0_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	BTN_0_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	BTN_0_CONF.intr_type = GPIO_INTR_DISABLE;
	
	gpio_config_t BTN_0_LED_CONF;
	BTN_0_LED_CONF.pin_bit_mask = BTN_0_LED_PIN_SEL;
	BTN_0_LED_CONF.mode = GPIO_MODE_OUTPUT;
#ifdef BTN_0_LED_DRIVER_P
	BTN_0_LED_CONF.pull_up_en = GPIO_PULLUP_ENABLE;
	BTN_0_LED_CONF.pull_down_en = GPIO_PULLDOWN_DISABLE;
#endif
#ifdef BTN_0_LED_DRIVER_N
	BTN_0_LED_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	BTN_0_LED_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
#endif
	BTN_0_LED_CONF.intr_type = GPIO_INTR_DISABLE;

	gpio_config_t BTN_1_CONF;
	BTN_1_CONF.pin_bit_mask = BTN_1_PIN_SEL;
	BTN_1_CONF.mode = GPIO_MODE_OUTPUT;
	BTN_1_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	BTN_1_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	BTN_1_CONF.intr_type = GPIO_INTR_DISABLE;
	
	// Pass structs to GPIO configurator
	gpio_config(&STEP_CONF);
	gpio_config(&DIR_CONF);
	gpio_config(&EN_CONF);
	gpio_config(&BTN_0_CONF);
	gpio_config(&BTN_0_LED_CONF);
	gpio_config(&BTN_1_CONF);
}

#endif

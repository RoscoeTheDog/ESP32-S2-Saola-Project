#include <configGpio.h>

void vInitGpioConfig(){ 
	// Configure all GPIO here.
	// Note: avoid designated initializers since they do not play friendly with C++ syntax & compiler
	gpio_config_t STEP_CONF;
	STEP_CONF.pin_bit_mask = STEP_PIN_SEL;
	STEP_CONF.mode = GPIO_MODE_OUTPUT;
	STEP_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	STEP_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	STEP_CONF.intr_type = GPIO_PIN_INTR_DISABLE;

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
	BTN_0_CONF.pin_bit_mask = BTN_0_INPUT_PIN_SEL;
	BTN_0_CONF.mode = GPIO_MODE_INPUT;
	BTN_0_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	BTN_0_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	BTN_0_CONF.intr_type = GPIO_INTR_DISABLE;
	
	gpio_config_t BTN_0_LED_CONF;
	BTN_0_LED_CONF.pin_bit_mask = BTN_0_LED_OUT_PIN_SEL;
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

	// TODO: Solve how to do a boolean GPIO Interrupt without cuasing kernal panic loop
	// gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
	// gpio_isr_handler_add(BTN_0_INPUT_PIN, xISR_button_0, 0);

	gpio_config_t BTN_1_CONF;
	BTN_1_CONF.pin_bit_mask = BTN_1_INPUT_PIN_SEL;
	BTN_1_CONF.mode = GPIO_MODE_INPUT;
	BTN_1_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	BTN_1_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	BTN_1_CONF.intr_type = GPIO_INTR_DISABLE;

	gpio_config_t RGB_RED_CONF;
	RGB_RED_CONF.pin_bit_mask = RGB_PIN_RED_SEL;
	RGB_RED_CONF.mode = GPIO_MODE_OUTPUT;
	RGB_RED_CONF.pull_up_en = GPIO_PULLUP_ENABLE;
	RGB_RED_CONF.pull_down_en = GPIO_PULLDOWN_DISABLE;
	RGB_RED_CONF.intr_type = GPIO_INTR_DISABLE;

	gpio_config_t RGB_GREEN_CONF;
	RGB_GREEN_CONF.pin_bit_mask = RGB_PIN_GREEN_SEL;
	RGB_GREEN_CONF.mode = GPIO_MODE_OUTPUT;
	RGB_GREEN_CONF.pull_up_en = GPIO_PULLUP_ENABLE;
	RGB_GREEN_CONF.pull_down_en = GPIO_PULLDOWN_DISABLE;
	RGB_GREEN_CONF.intr_type = GPIO_INTR_DISABLE;

	gpio_config_t RGB_BLUE_CONF;
	RGB_BLUE_CONF.pin_bit_mask = RGB_PIN_BLUE_SEL;
	RGB_BLUE_CONF.mode = GPIO_MODE_OUTPUT;
	RGB_BLUE_CONF.pull_up_en = GPIO_PULLUP_ENABLE;
	RGB_BLUE_CONF.pull_down_en = GPIO_PULLDOWN_DISABLE;
	RGB_BLUE_CONF.intr_type = GPIO_INTR_DISABLE;
	
	// Pass structs to GPIO configurator
	gpio_config(&STEP_CONF);
	gpio_config(&DIR_CONF);
	gpio_config(&EN_CONF);
	gpio_config(&BTN_0_CONF);
	gpio_config(&BTN_0_LED_CONF);
	gpio_config(&BTN_1_CONF);
	gpio_config(&RGB_RED_CONF);
	gpio_config(&RGB_GREEN_CONF);
	gpio_config(&RGB_BLUE_CONF);

}

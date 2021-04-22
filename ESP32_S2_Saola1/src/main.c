#include <driver/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_task_wdt.h>
#include <driver/timer.h>
#include <driver/ledc.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include <freertos/queue.h>

#define STEP_PIN GPIO_NUM_1
#define STEP_PIN_SEL GPIO_SEL_1
#define DIR_PIN GPIO_NUM_2
#define DIR_PIN_SEL GPIO_SEL_2
#define BTN_0_PIN GPIO_NUM_5
#define BTN_0_PIN_SEL GPIO_SEL_5
#define BTN_0_LED_PIN GPIO_NUM_8
#define BTN_0_LED_PIN_SEL GPIO_SEL_8
#define BTN_1_PIN GPIO_NUM_21
#define BTN_1_PIN_SEL GPIO_SEL_21

// static xQueueHandle gpio_evt_queue = NULL;

volatile bool BUTTON_STATE = 0;
volatile bool STEP_PULSE_STATE = 0;

timer_idx_t timer_1 = TIMER_0;
timer_idx_t timer_2 = TIMER_1;
timer_group_t group_1 = TIMER_GROUP_0;
timer_group_t group_2 = TIMER_GROUP_1;

bool IRAM_ATTR handler_button_0(void * args) {
	
	if (gpio_get_level(BTN_0_PIN)){
		gpio_set_level(BTN_0_LED_PIN, 1);
		// timer_start(group_2, timer_2);
	}
	else {
		gpio_set_level(BTN_0_LED_PIN, 0);
		// timer_pause(group_2, timer_2);
	}

	return false;
}

bool IRAM_ATTR handler_stepPin(void * args) {
	// toggle pulse state.
	STEP_PULSE_STATE = !STEP_PULSE_STATE;
	// write to pin.
	gpio_set_level(STEP_PIN, STEP_PULSE_STATE);

	return false;
}

void initializeGPIO(){ 
	// Configure all GPIO here
	gpio_config_t STEP_CONF = {
		.pin_bit_mask = STEP_PIN_SEL,
		.mode = GPIO_MODE_OUTPUT,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};

	gpio_config_t DIR_CONF = {
	.pin_bit_mask = DIR_PIN_SEL,
	.mode = GPIO_MODE_OUTPUT,
	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	.intr_type = GPIO_INTR_DISABLE,
	};

	gpio_config_t BTN_0_CONF = {
	.pin_bit_mask = BTN_0_PIN_SEL,
	.mode = GPIO_MODE_INPUT,
	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	.intr_type = GPIO_INTR_DISABLE,
	};

	gpio_config_t BTN_0_LED_CONF = {
	.pin_bit_mask = BTN_0_LED_PIN_SEL,
	.mode = GPIO_MODE_OUTPUT,
	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	.intr_type = GPIO_INTR_DISABLE,
	};

	gpio_config_t BTN_1_CONF = {
	.pin_bit_mask = BTN_1_PIN_SEL,
	.mode = GPIO_MODE_INPUT,
	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	.intr_type = GPIO_INTR_DISABLE,
	};
	
	// Pass structs to GPIO configurator
	gpio_config(&STEP_CONF);

	gpio_config(&DIR_CONF);

	gpio_config(&BTN_0_CONF);

	gpio_config(&BTN_0_LED_CONF);

	gpio_config(&BTN_1_CONF);

	timer_config_t led_test_config = {
		.clk_src = TIMER_SRC_CLK_APB,
		.divider = 65536,
		.auto_reload = TIMER_AUTORELOAD_EN,
		.intr_type = TIMER_INTR_LEVEL,
		.counter_dir = TIMER_COUNT_UP,
		.counter_en = TIMER_PAUSE,
		.alarm_en = TIMER_ALARM_EN,
	};

	timer_config_t stepper_driver_config = {
	.clk_src = TIMER_SRC_CLK_APB,
	.divider = 33,
	.auto_reload = TIMER_AUTORELOAD_EN,
	.intr_type = TIMER_INTR_LEVEL,
	.counter_dir = TIMER_COUNT_UP,
	.counter_en = TIMER_PAUSE,
	.alarm_en = TIMER_ALARM_EN,
	};

	// timer_init(group_1, timer_1, &led_test_config);
	// timer_init(group_2, timer_2, &stepper_driver_config);

	// alarm value == number of tics of the timer that has occured.
	// So if set at 5khz, then 5,000hz/5 tics = 1khz calls/s or 0.001 second (1ms)
	// timer_set_alarm_value(group_1, timer_1, 1);
	// timer_set_alarm_value(group_2, timer_2, 1);

	// timer_isr_callback_add(group_1, timer_1, handler_button_0, NULL, ESP_INTR_FLAG_SHARED);
	// timer_isr_callback_add(group_2, timer_2, handler_stepPin, NULL, ESP_INTR_FLAG_SHARED);

	// timer_start(group_1, timer_1);
	// timer_start(group_2, timer_2);

	ledc_timer_config_t ledc_timer_conf_0 = {
	.speed_mode = LEDC_LOW_SPEED_MODE,
	.timer_num = LEDC_TIMER_0,
	.freq_hz = 6000000,
	.duty_resolution = LEDC_TIMER_3_BIT,
	};

	ledc_channel_config_t ledc_channel_conf_0 = {
		.gpio_num = STEP_PIN,
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.channel = LEDC_CHANNEL_0,
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel = LEDC_TIMER_0,
		.duty = 3,
		.hpoint = 0,
	};

	ledc_timer_config(&ledc_timer_conf_0);

	ledc_channel_config(&ledc_channel_conf_0);
}

void app_main(void) {
	// Initialize stuff here.
	bool RUN_LOOP = true;
	// Setup GPIO
	initializeGPIO();

	while(RUN_LOOP) {
		// gpio_set_level(BTN_0_LED_PIN, 0);
	}
}

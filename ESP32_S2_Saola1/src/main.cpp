#include <driver/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_task_wdt.h>
#include <driver/timer.h>
#include <driver/ledc.h>
#include <StepperDriver.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

// Tell the compiler to wrap main as C-- this way it knows how to handle the C conventions from the espidf framework.
extern "C" {
    void app_main(void);
}

#define STEP_PIN GPIO_NUM_1
#define STEP_PIN_SEL GPIO_SEL_1
#define DIR_PIN GPIO_NUM_2
#define DIR_PIN_SEL GPIO_SEL_2
#define EN_PIN GPIO_NUM_0
#define EN_PIN_SEL GPIO_SEL_0
#define BTN_0_PIN GPIO_NUM_5
#define BTN_0_PIN_SEL GPIO_SEL_5
#define BTN_0_LED_PIN GPIO_NUM_8
#define BTN_0_LED_PIN_SEL GPIO_SEL_8
#define BTN_1_PIN GPIO_NUM_21
#define BTN_1_PIN_SEL GPIO_SEL_21

BasicStepperDriver StepperMotorTest(200, DIR_PIN, DIR_PIN_SEL, STEP_PIN, STEP_PIN_SEL, EN_PIN, EN_PIN_SEL);
QueueHandle_t TaskQueue = xQueueCreate(10, 2048);



// static xQueueHandle gpio_evt_queue = NULL;
volatile bool toggleON = false;
volatile bool BUTTON_STATE = 0;
volatile bool STEP_PULSE_STATE = 0;

timer_idx_t timer_1 = TIMER_0;
timer_idx_t timer_2 = TIMER_1;
timer_group_t group_1 = TIMER_GROUP_0;
timer_group_t group_2 = TIMER_GROUP_1;

bool IRAM_ATTR ISR_button_0(void * args) {
	// ledc_fade_func_install(ESP_INTR_FLAG_SHARED);
	
	if (gpio_get_level(BTN_0_PIN)){
		if (ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) < 1)
			ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048-1, 25, LEDC_FADE_NO_WAIT);


		StepperMotorTest.rotate(1);
		// StepperMotorTest.move(10);
	}
	else
		toggleON = true;

	if (gpio_get_level(BTN_1_PIN)){
		if (ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) < 1)
			ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048-1, 25, LEDC_FADE_NO_WAIT);

		gpio_set_level(DIR_PIN, 0);
		StepperMotorTest.rotate(1);
		// StepperMotorTest.move(-10);
	}
	else
		toggleON = true;



	StepperMotorTest.stop();
	return false;
}

// bool IRAM_ATTR ISR_button_1(void * args) {

// 	if (gpio_get_level(BTN_1_PIN)){
// 		gpio_set_level(BTN_0_LED_PIN, 1);
// 		StepperMotorTest.rotate(-360);
// 		// timer_start(group_2, timer_2);
// 	}
// 	else {
// 		gpio_set_level(BTN_0_LED_PIN, 0);
// 		StepperMotorTest.stop();
// 		// timer_pause(group_2, timer_2);
// 	}

// 	return false;
// }

// bool IRAM_ATTR handler_stepPin(void * args) {
// 	// toggle pulse state.
// 	STEP_PULSE_STATE = !STEP_PULSE_STATE;
// 	// write to pin.
// 	gpio_set_level(STEP_PIN, STEP_PULSE_STATE);

// 	return false;
// }

void initializeGPIO(){ 
	// Configure all GPIO here
	gpio_config_t STEP_CONF = {
		.pin_bit_mask = STEP_PIN_SEL,
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};

	gpio_config_t DIR_CONF = {
	.pin_bit_mask = DIR_PIN_SEL,
	.mode = GPIO_MODE_OUTPUT,
	.pull_up_en = GPIO_PULLUP_DISABLE,
	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	.intr_type = GPIO_INTR_DISABLE,
	};

	gpio_config_t EN_CONF = {
	.pin_bit_mask = EN_PIN_SEL,
	.mode = GPIO_MODE_OUTPUT,
	.pull_up_en = GPIO_PULLUP_DISABLE,
	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	.intr_type = GPIO_INTR_DISABLE,
	};

	gpio_config_t BTN_0_CONF = {
	.pin_bit_mask = BTN_0_PIN_SEL,
	.mode = GPIO_MODE_INPUT,
	.pull_up_en = GPIO_PULLUP_DISABLE,
	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	.intr_type = GPIO_INTR_DISABLE,
	};

	gpio_config_t BTN_0_LED_CONF;
	BTN_0_LED_CONF.pin_bit_mask = BTN_0_LED_PIN_SEL;
	BTN_0_LED_CONF.mode = GPIO_MODE_OUTPUT;
	BTN_0_LED_CONF.pull_up_en = GPIO_PULLUP_DISABLE;
	BTN_0_LED_CONF.pull_down_en = GPIO_PULLDOWN_ENABLE;
	BTN_0_LED_CONF.intr_type = GPIO_INTR_DISABLE;

	gpio_config_t BTN_1_CONF = {
	.pin_bit_mask = BTN_1_PIN_SEL,
	.mode = GPIO_MODE_INPUT,
	.pull_up_en = GPIO_PULLUP_DISABLE,
	.pull_down_en = GPIO_PULLDOWN_ENABLE,
	.intr_type = GPIO_INTR_DISABLE,
	};
	
	// Pass structs to GPIO configurator
	gpio_config(&STEP_CONF);
	gpio_config(&DIR_CONF);
	gpio_config(&EN_CONF);
	gpio_config(&BTN_0_CONF);
	gpio_config(&BTN_0_LED_CONF);
	gpio_config(&BTN_1_CONF);

	// // Note: do not use C designated initializers as they are not friendly with C++
	timer_config_t timer_0_cfg;
	timer_0_cfg.clk_src = TIMER_SRC_CLK_APB;
	timer_0_cfg.divider = 65536;					 // gives 1.222khz frequency: aprox 1ms frequency.
	timer_0_cfg.auto_reload = TIMER_AUTORELOAD_EN;
	timer_0_cfg.intr_type = TIMER_INTR_LEVEL;
	timer_0_cfg.counter_dir = TIMER_COUNT_UP;
	timer_0_cfg.alarm_en = TIMER_ALARM_EN;

	timer_init(group_1, timer_1, &timer_0_cfg);

	// alarm value == number of tics of the timer that has occured.
	// So if set at 5khz, then 5,000hz/5 tics = 1khz calls/s or 0.001 second (1ms)
	timer_set_alarm_value(group_1, timer_1, 2);
	// Attach a callback (isr)
	timer_isr_callback_add(group_1, timer_1, ISR_button_0, NULL, ESP_INTR_FLAG_IRAM);
	// Start the timer-- if cfg has alarm EN alarm will auto-start
	timer_start(group_1, timer_1);

	// define config objects
	ledc_timer_config_t ledc_timer_conf_0;
	ledc_timer_conf_0.clk_cfg = LEDC_AUTO_CLK;
	ledc_timer_conf_0.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_timer_conf_0.timer_num = LEDC_TIMER_0;
	ledc_timer_conf_0.freq_hz = 25000;
	ledc_timer_conf_0.duty_resolution = LEDC_TIMER_11_BIT;
	
	ledc_channel_config_t ledc_channel_conf_0;
	ledc_channel_conf_0.gpio_num = BTN_0_LED_PIN;
	ledc_channel_conf_0.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_channel_conf_0.timer_sel = LEDC_TIMER_0;
	ledc_channel_conf_0.duty = 0;
	ledc_channel_conf_0.channel = LEDC_CHANNEL_0;
	ledc_channel_conf_0.hpoint = 0;
	ledc_channel_conf_0.intr_type = LEDC_INTR_DISABLE;

	// pass configs to config initializers.
	ledc_timer_config(&ledc_timer_conf_0);
	ledc_channel_config(&ledc_channel_conf_0);

	ledc_fade_func_install(0);
}

void app_main(void) {
	// Initialize stuff here.
	bool RUN_LOOP = true;
	// Setup GPIO
	initializeGPIO();
	// init the motor.
	StepperMotorTest.begin(700, 64);
	
	while(RUN_LOOP) {

		// if (toggleON)
		// 	ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048-1, 25, LEDC_FADE_NO_WAIT);
		if (toggleON && ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) > 1)
			ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 3000, LEDC_FADE_NO_WAIT);

		// if (ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) < 1)
		// {
		// 	ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048-1, 25, LEDC_FADE_NO_WAIT);
		// 	notificationLED = false;
		// }
			
		// if (ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) > 1)
		// {
		// 	ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 3000, LEDC_FADE_NO_WAIT);
		// 	notificationLED = true;
		// }
			
		// BasicStepperDriver::delayMicros(5*100000);
	
		// ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048-1, 25);
		// ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);
		// ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 3000);
		// ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
		// printf("%s", "Hello World!");
		// ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 3000);
		// ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
		// BasicStepperDriver::delayMicros(5000000);
		
		
		// gpio_set_level(BTN_0_LED_PIN, 0);
		// printf("%llu\n", esp_timer_get_time());
		// StepperMotorTest.rotate(360);
		// StepperMotorTest.move(360);
		// StepperMotorTest.startMove(1);

		// BasicStepperDriver::delayMicros(4 * 1000000);
		// vTaskDelay(80000000);
	}
}


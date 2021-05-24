#include <configTimers.h>

timer_config_t config_timer_0;
timer_config_t config_timer_1;

inline void vSetLEDFadePeriod(int ms) {

	int timer_0_frequency = APB_CLK_FREQ/config_timer_0.divider;
	int period_ms = timer_0_frequency/1000;
	int target_period_us = ms * period_ms;
	uint64_t alarm_value = target_period_us/LEDC_CHANNEL_0_DUTY_MAX;

	// Pause and reset the timer before setting the alarm.
	printf("%s %lld\n", "FADE ALARM(64) INTERVAL: ", alarm_value);
	timer_pause(TIMER_GROUP_0, TIMER_0);
	timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
	timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, alarm_value);
	timer_start(TIMER_GROUP_0, TIMER_0);
}

inline void vInitTimer_0() {
	// Note: do not use C designated initializers as they are not friendly with C++ syntax.
	config_timer_0.clk_src = TIMER_SRC_CLK_APB;
	config_timer_0.divider = 2;					 // 40hz
	config_timer_0.auto_reload = TIMER_AUTORELOAD_EN;
	config_timer_0.intr_type = TIMER_INTR_NONE;
	config_timer_0.counter_dir = TIMER_COUNT_UP;
	config_timer_0.alarm_en = TIMER_ALARM_EN;

	// Initialize and start the timer. Note: if alarm_en set to TIMER_ALARM_EN macro the alarm will start with the timer.
	timer_init(TIMER_GROUP_0, TIMER_0, &config_timer_0);

	// alarm value is number of cycle periods the timer has elapsed.
	// example: if set at 5khz speed, then 5,000hz/5 tics = 1,000 us/period or 0.001 second (1ms)
	timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000 * 120);
	// Attach a callback (isr)
	timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, xISR_button_0, NULL, ESP_INTR_FLAG_IRAM);

	timer_start(TIMER_GROUP_0, TIMER_0);
} 

inline void vInitTimer_1() {
	// Note: do not use C designated initializers as they are not friendly with C++ syntax.
	timer_config_t config_timer_1;
	config_timer_1.clk_src = TIMER_SRC_CLK_APB;
	config_timer_1.divider = 2;					 // 40mhz
	config_timer_1.auto_reload = TIMER_AUTORELOAD_EN;
	config_timer_1.intr_type = TIMER_INTR_LEVEL;
	config_timer_1.counter_dir = TIMER_COUNT_UP;
	config_timer_1.alarm_en = TIMER_ALARM_EN;

	// Initialize and start the timer. Note: if alarm_en set to TIMER_ALARM_EN macro the alarm will start with the timer.
	// timer_init(TIMER_GROUP_1, TIMER_1, &config_timer_1);

	// alarm value is number of cycle periods the timer has elapsed.
	// example: if set at 5khz speed, then 5,000hz/5 tics = 1,000 us/period or 0.001 second (1ms)
	// timer_set_alarm_value(TIMER_GROUP_1, TIMER_1, ( 80000000 / config_timer_1.divider ) / 3000000);
	// Attach a callback (isr)
	// timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, xISR_button_0, NULL, ESP_INTR_FLAG_LEVEL1);

	// timer_start(TIMER_GROUP_1, TIMER_1);
}

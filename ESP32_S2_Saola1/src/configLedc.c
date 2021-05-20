#include <configLedc.h>

int volatile LEDC_CHANNEL_0_DUTY = 0;

inline int xGetDutyResolutionMax() {
	return (pow(2, LEDC_CHANNEL_0_DUTY_BITS) - 1);
}

inline void vInitLEDC_0( void ) {
	// config ledc peripherials and channels here.
	// Note: when needing to reference a channel, use the appropriate C macros instead of the direct objects.
	ledc_timer_config_t ledc_timer_conf_0;
	ledc_timer_conf_0.clk_cfg = LEDC_AUTO_CLK;
	ledc_timer_conf_0.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_timer_conf_0.timer_num = LEDC_TIMER_0;
	ledc_timer_conf_0.freq_hz = LEDC_TIMER_0_FREQUENCY;
	ledc_timer_conf_0.duty_resolution = LEDC_CHANNEL_0_DUTY_BITS;

	ledc_channel_config_t ledc_channel_conf_0;
	ledc_channel_conf_0.gpio_num = BTN_0_LED_PIN;
	ledc_channel_conf_0.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_channel_conf_0.timer_sel = LEDC_TIMER_0;
	ledc_channel_conf_0.duty = LEDC_CHANNEL_0_DUTY;
	ledc_channel_conf_0.channel = LEDC_CHANNEL_0;
	ledc_channel_conf_0.hpoint = 0;
	ledc_channel_conf_0.intr_type = LEDC_INTR_DISABLE;

	// pass configs to config initializers.
	ledc_timer_config(&ledc_timer_conf_0);
	ledc_channel_config(&ledc_channel_conf_0);

	printf("%s %i\n", "LEDC FREQUENCY: ", LEDC_TIMER_0_FREQUENCY);
	printf("%s %i\n", "LEDC BIT RESOLUTION: ", LEDC_CHANNEL_0_DUTY_BITS);
	printf("%s %i\n", "LEDC BIT INTEGER MAX: ", LEDC_CHANNEL_0_DUTY_MAX);

	// ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2047, 0);
	// ledc_set_duty_and_update
}

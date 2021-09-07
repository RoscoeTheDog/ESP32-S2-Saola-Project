#include <configLedc.h>

int volatile LEDC_CHANNEL_0_DUTY = 0;

void setStatusLEDOff() {
	gpio_set_level(RGB_PIN_RED, 0);
	gpio_set_level(RGB_PIN_GREEN, 0);
	gpio_set_level(RGB_PIN_BLUE, 0);
}

void setStatusLEDRed() {
	gpio_set_level(RGB_PIN_RED, 0);
	gpio_set_level(RGB_PIN_GREEN, 1);
	gpio_set_level(RGB_PIN_BLUE, 1);
}

void setStatusLEDYellow() {
	gpio_set_level(RGB_PIN_RED, 0);
	gpio_set_level(RGB_PIN_GREEN, 0);
	gpio_set_level(RGB_PIN_BLUE, 1);
}

void setStatusLEDGreen() {
	gpio_set_level(RGB_PIN_RED, 1);
	gpio_set_level(RGB_PIN_GREEN, 0);
	gpio_set_level(RGB_PIN_BLUE, 1);
}

void setStatusLEDBlue() {
	gpio_set_level(RGB_PIN_RED, 1);
	gpio_set_level(RGB_PIN_GREEN, 1);
	gpio_set_level(RGB_PIN_BLUE, 0);
}

inline int xGetDutyResolutionMax() {
	return (pow(2, LEDC_CHANNEL_0_DUTY_BITS) - 1);
}

void initialize_ledc_config_0( void ) {
	char *TAG = "initialize_ledc_config_0";

	// config ledc peripherials and channels here.
	// Note: when needing to reference a channel, use the appropriate C macros instead of the direct objects.
	ledc_timer_config_t ledc_timer_conf_0;
	ledc_timer_conf_0.clk_cfg = LEDC_AUTO_CLK;
	ledc_timer_conf_0.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_timer_conf_0.timer_num = LEDC_TIMER_0;
	ledc_timer_conf_0.freq_hz = LEDC_TIMER_0_FREQUENCY;
	ledc_timer_conf_0.duty_resolution = LEDC_CHANNEL_0_DUTY_BITS;

	ledc_channel_config_t ledc_channel_conf_0;
	ledc_channel_conf_0.gpio_num = BTN_0_LED_OUT_PIN;
	ledc_channel_conf_0.speed_mode = LEDC_LOW_SPEED_MODE;
	ledc_channel_conf_0.timer_sel = LEDC_TIMER_0;
	ledc_channel_conf_0.duty = LEDC_CHANNEL_0_DUTY;
	ledc_channel_conf_0.channel = LEDC_CHANNEL_0;
	ledc_channel_conf_0.hpoint = 0;
	ledc_channel_conf_0.intr_type = LEDC_INTR_DISABLE;

	// pass configs to config initializers.
	ledc_timer_config(&ledc_timer_conf_0);
	ledc_channel_config(&ledc_channel_conf_0);

	ESP_LOGI(TAG, "%s %i", "LEDC FREQUENCY: ", LEDC_TIMER_0_FREQUENCY);
	ESP_LOGI(TAG, "%s %i", "LEDC BIT RESOLUTION: ", LEDC_CHANNEL_0_DUTY_BITS);
	ESP_LOGI(TAG, "%s %i", "LEDC BIT INTEGER MAX: ", LEDC_CHANNEL_0_DUTY_MAX);

	// ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2047, 0);
	// ledc_set_duty_and_update
}

// returns true if led duty cycle is active. false if led is off.
inline bool getLEDState() {
// Check the switch type. Change the logic accordingly.
#ifdef BTN_0_LED_DRIVER_N
	if (LEDC_CHANNEL_0_DUTY == 0) {
		return false;
	}
#endif

#ifdef BTN_0_LED_DRIVER_P
	if (LEDC_CHANNEL_0_DUTY == LEDC_CHANNEL_0_DUTY_MAX) {
		return false;
	}
#endif

	return true;
}

inline void setLEDHigh() {
// Check the switch driver type. Change the output logic accordingly.
#ifdef BTN_0_LED_DRIVER_N
	LEDC_CHANNEL_0_DUTY = LEDC_CHANNEL_0_DUTY_MAX;
#endif

#ifdef BTN_0_LED_DRIVER_P
	LEDC_CHANNEL_0_DUTY = 0;
#endif

	ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY, 0);
}

inline void fadeUpdate() {

#ifdef BTN_0_LED_DRIVER_N
	LEDC_CHANNEL_0_DUTY--;
#endif

#ifdef BTN_0_LED_DRIVER_P
	LEDC_CHANNEL_0_DUTY++;
#endif
	ledc_set_duty_with_hpoint(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_CHANNEL_0_DUTY, 0);
}


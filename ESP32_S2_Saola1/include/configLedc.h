#ifndef CONFIG_LEDC_H
#define CONFIG_LEDC_H

#include <driver/ledc.h>
#include <math.h>
#include <configGpio.h>
#include <esp_log.h>

#define LEDC_TIMER_0_FREQUENCY 100
#define LEDC_CHANNEL_0_DUTY_BITS LEDC_TIMER_11_BIT
#define LEDC_CHANNEL_0_DUTY_MAX xGetDutyResolutionMax()		// TODO: How can pre-calculate and save the return value of this function @ compile time?

extern volatile int LEDC_CHANNEL_0_DUTY;

extern int xGetDutyResolutionMax();

extern void initialize_ledc_config_0( void );

extern bool getLEDState();

extern void setLEDHigh();

extern void fadeUpdate();

extern void setStatusLEDOff();

extern void setStatusLEDRed();

extern void setStatusLEDYellow();

extern void setStatusLEDGreen();

extern void setStatusLEDBlue();

#endif

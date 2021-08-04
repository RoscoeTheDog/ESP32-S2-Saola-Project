#ifndef CONFIG_TIMERS_H
#define CONFIG_TIMERS_H

#include <driver/timer.h>
// #include <configLedc.h>
#include <espInterrupts.h>

extern timer_config_t config_timer_0;
extern timer_config_t config_timer_1;

extern void initializeTimerConfig();

extern void initializeTimerConfig_1();

extern void fadeUpdate();

extern void setLEDFadePeriod(int ms);

#endif

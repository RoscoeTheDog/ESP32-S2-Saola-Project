#ifndef CONFIG_TIMERS_H
#define CONFIG_TIMERS_H

#include <driver/timer.h>
#include <espInterrupts.h>

extern timer_config_t config_timer_0;
extern timer_config_t config_timer_1;

extern void vInitTimerConfig_0();

extern void vInitTimerConfig_1();

extern void fadeUpdate();

extern void vSetLEDFadePeriod(int ms);

#endif

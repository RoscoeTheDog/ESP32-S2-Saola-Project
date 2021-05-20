#ifndef CONFIG_TIMERS_H
#define CONFIG_TIMERS_H

#include <driver/timer.h>
#include <espInterrupts.h>

extern timer_config_t config_timer_0;
extern timer_config_t config_timer_1;

extern void vInitTimer_0();

extern void vInitTimer_1();

extern void vUpdateLEDFade();

extern void vSetLEDFadePeriod(int ms);

#endif

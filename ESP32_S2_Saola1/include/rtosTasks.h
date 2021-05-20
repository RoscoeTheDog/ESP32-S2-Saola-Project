#ifndef RTOSTASKS_HPP
#define RTOSTASKS_HPP

#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <configLedc.h>
#include <freertos/timers.h>

// Reconfigure default settings for this project via header. More reliable if framework gets reset to defaults somehow.
// #ifdef CONFIG_FREERTOS_HZ
// #undef CONFIG_FREERTOS_HZ
// #endif
// #define CONFIG_FREERTOS_HZ (1000)


// Create all handlers for task function callbacks. It is a pointer, so we can init it to null.
extern TaskHandle_t xHandleLEDFade;
extern TaskHandle_t xHandleRTOSDebug;
extern TaskHandle_t xHandleCurtainStepper;
extern TimerHandle_t xHandleTimerLED;

extern void vTaskCurtainStepper(void * pvPerameters);

extern void vInitTaskCurtainStepper();

extern void vInitTaskLEDFade( void );

extern void vInitTaskRTOSDebug( void );

extern void vTaskLEDFade( void * pvParameters);

extern void vTaskRTOSDebug( void * pvParameters);

extern void vInitTimerLEDFade( int ms );

#endif

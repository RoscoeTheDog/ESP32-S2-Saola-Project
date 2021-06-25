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
extern TimerHandle_t xHandleTimerLED;
extern TaskHandle_t xHandleRTOSDebug;
extern TaskHandle_t xHandleCurtainStepperForward;
extern TaskHandle_t xHandleCurtainStepperReverse;
extern TaskHandle_t xHandleOpenCurtains;
extern TaskHandle_t xHandleCloseCurtains;

extern void vInitTaskOpenCurtains();

extern void vInitTaskCloseCurtains();

extern void vTaskOpenCurtains( void * pvPerameters);

extern void vTaskCloseCurtains( void * pvPerameters);

extern void vTaskRotateStepperForward(void * pvPerameters);

extern void vTaskRotateStepperReverse(void * pvPerameters);

extern void vInitTaskCurtainMotor();

extern void vInitTaskLEDFade( void );

extern void vInitTaskRTOSDebug( void );

extern void vTaskLEDFade( void * pvParameters);

extern void vTaskRTOSDebug( void * pvParameters);

extern void vInitTimerLEDFade( int milliseconds );

#endif

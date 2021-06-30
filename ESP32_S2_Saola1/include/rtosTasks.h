#ifndef RTOSTASKS_HPP
#define RTOSTASKS_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/queue.h>
#include <configLedc.h>
#include <settings.h>
#include <StepperDriver.h>
#include <esp_task_wdt.h>
#include <esp_int_wdt.h>
#include <configSteppers.h>
#include <esp_wifi.h>
#include <freertos/event_groups.h>

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

/* FreeRTOS event group to signal when we are connected & ready to make a request */
extern EventGroupHandle_t s_wifi_event_group;

extern void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

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

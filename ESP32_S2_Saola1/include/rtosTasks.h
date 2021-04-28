#ifndef RTOSTASKS_HPP
#define RTOSTASKS_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <configLedc.h>

// Create all handlers for task function callbacks. It is a pointer, so we can init it to null.
extern TaskHandle_t xHandleLEDFade;
extern TaskHandle_t xHandleRTOSDebug;

extern void vInitTaskLEDFade( void );

extern void vInitTaskRTOSDebug( void );

extern void vTaskLEDFade( void * pvParameters);

extern void vTaskRTOSDebug( void * pvParameters);

#endif

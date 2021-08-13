#ifndef ESP_ISR_H
#define ESP_ISR_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <rtosTasks.h>
#include <configGpio.h>
#include <StepperDriver.h>
#include <configSteppers.h>
#include <httpRequests.h>

extern volatile bool BTN_0_PIN_STATE;
extern volatile bool BTN_1_PIN_STATE;

/*
	If you called FreeRTOS functions in callback, you need to return true or false based on the retrun value of argument pxHigherPriorityTaskWoken. 
	For example, xQueueSendFromISR is called in callback, if the return value pxHigherPriorityTaskWoken of any FreeRTOS calls is pdTRUE, return true; otherwise return false.
*/
extern bool IRAM_ATTR xISR_button_0(void * args);

#endif

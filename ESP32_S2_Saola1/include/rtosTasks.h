#ifndef RTOSTASKS_HPP
#define RTOSTASKS_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/queue.h>
#include <configLedc.h>
#include <globals.h>
#include <StepperDriver.h>
#include <esp_task_wdt.h>
#include <esp_int_wdt.h>
#include <configSteppers.h>
#include <esp_wifi.h>
#include <freertos/event_groups.h>
#include <driver/uart.h>
#include <esp_sleep.h>
#include <httpRequests.h>
#include <freertos/timers.h>
#include <configSntp.h>
#include <globals.h>
#include <configWifi.h>
#include <httpRequests.h>
#include <cJSON.h>
#include <cJSON_Utils.h>
#include <espInterrupts.h>

// Reconfigure default settings for this project via header. More reliable if framework gets reset to defaults somehow.
// #ifdef CONFIG_FREERTOS_HZ
// #undef CONFIG_FREERTOS_HZ
// #endif
// #define CONFIG_FREERTOS_HZ (1000)


// Create all handlers for task function callbacks. It is a pointer, so we can init it to null.
extern TaskHandle_t xHandleLEDFade;
extern TimerHandle_t xHandleTimerLED;
extern TaskHandle_t xHandleRTOSDebug;
extern TaskHandle_t xHandleMoveStepperForward;
extern TaskHandle_t xHandleMoveStepperReverse;
extern TaskHandle_t xHandleSleepTask;
extern TimerHandle_t xHandlePollServer;
extern TaskHandle_t xHandleSntpStatus;
extern TaskHandle_t xHandleStatusLEDWatchdog;
extern TaskHandle_t xHandleWifiReconnect;
extern TaskHandle_t xHandleHttpRequestServerData;
extern TaskHandle_t xHandleUpdateMotor;
extern TaskHandle_t xHandleSubmitLocalData;
extern TaskHandle_t xHandleWifiPersistingTasks;
extern TaskHandle_t xHandleSmartConfig;

/* FreeRTOS event group to signal when we are connected & ready to make a request */
extern EventGroupHandle_t s_wifi_event_group;

extern void vTaskWifiPersistingTasks(void *args);

extern void vTaskSubmitLocalData(void *args);

extern void initializeTasks();

extern void vInitTaskWifiReconnect();

extern void vTaskWifiReconnect(void *args);

extern void vInitTaskUpdateMotor();

extern void vTaskUpdateMotor();

extern void vTaskUpdateDatetimeStatus( void *args);

extern void vInitTaskPollServer();

extern void vInitTaskStatusLEDWatchdog();

extern void vTaskStatusLEDWatchdog(void *args);

extern void vTaskPollServer(void * args);

extern void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

extern void vInitTaskSleep();

extern bool vTaskIdleHook();

extern void vTaskSleep();

extern void vInitTaskOpenCurtains();

extern void vInitTaskCloseCurtains();

extern void vTaskMoveStepperForward(void * pvPerameters);

extern void vTaskMoveStepperReverse(void * pvPerameters);

extern void vInitTaskCurtainMotor();

extern void vInitTaskLEDFade( void );

extern void vInitTaskRTOSDebug( void );

extern void vTaskLEDFade( void * pvParameters);

extern void vTaskRTOSDebug( void * pvParameters);

extern void vInitTimerLEDFade( int milliseconds );

#endif

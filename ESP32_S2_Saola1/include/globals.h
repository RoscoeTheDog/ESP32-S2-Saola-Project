#ifndef SETTINGS_H
#define SETTINGS_H

#include <StepperDriver.h>
#include <freertos/FreeRTOS.h>

extern volatile float CURTAIN_LENGTH_INCH;
extern volatile float ROD_DIAMETER_MM;
extern volatile float BATTERY_PERCENTAGE;
extern volatile float CURTAIN_PERCENTAGE;
extern volatile int MOTOR_SPEED_RPM;
extern volatile long MOTOR_POSITION_STEPS;
extern volatile float MATERIAL_THICKNESS_MM;

extern char* WEBHOOK_URL;
extern char *LOCAL_SERVER_ADDRESS;
extern char *REMOTE_SERVER_ADDRESS;
extern char *WIFI_SSID;
extern char *WIFI_PASSWORD;
extern char *WRITE_KEY;
extern char *READ_KEY;
extern char *LOCAL_DEVICE_ID;
extern char *USERNAME;

extern volatile int SYS_SYNC;
extern volatile int UPLOADING;
extern volatile int DATETIME_SYNC;

extern portMUX_TYPE mux;

// #define CURTAIN_LENGTH_INCH 59.0
// #define ROD_DIAMETER_MM 25.4
// #define WIFI_SSID "WutangLAN"
// #define WIFI_PASSWORD "c@$T131nTh3$Ky"
// #define WRITE_KEY "abc"
// #define READ_KEY "abc"
// #define DEVICE_ID "1"

#endif	// SETTINGS_H

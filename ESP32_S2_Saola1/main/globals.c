#include <globals.h>

volatile float CURTAIN_LENGTH_INCH = 59.0;
volatile float ROD_DIAMETER_MM = 25.4;
volatile float MATERIAL_THICKNESS_MM = 0.2;
volatile float BATTERY_PERCENTAGE = 0;
volatile float CURTAIN_PERCENTAGE = 0;
volatile int MOTOR_SPEED_RPM = 200;
volatile long MOTOR_POSITION_STEPS = 0;

char *WEBHOOK_URL = "http://192.168.1.84:8000/devices/smartblinds/";
// char *WEBHOOK_URL = "http://roscoethedog.pythonanywhere.com/devices/smartblinds/";
char *LOCAL_SERVER_ADDRESS = "http://192.168.1.84:8000/devices/smartblinds/";
char *REMOTE_SERVER_ADDRESS = "http://roscoethedog.pythonanywhere.com/devices/smartblinds/";
// strcpy(WIFI_SSID, "wutangLAN");
// strcat(WIFI_PASSWORD, "c@$T131nTh3$Ky");
// char WIFI_PASSWORD = "c@$T131nTh3$Ky";
char *WRITE_KEY = "abc";
char *READ_KEY = "abc";
char *LOCAL_DEVICE_ID = "1";
char *USERNAME = "smartblinds_1";

volatile int UPLOADING = 0;
volatile int DATETIME_SYNC = 0;
volatile int SYS_SYNC = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

#include <globals.h>

volatile float CURTAIN_LENGTH_INCH = 59.0;
volatile float ROD_DIAMETER_MM = 25.4;
volatile float MATERIAL_THICKNESS_MM = 0.2;
volatile float BATTERY_PERCENTAGE = 0;
volatile float CURTAIN_PERCENTAGE = 0;
volatile int MOTOR_SPEED_RPM = 60;
volatile long MOTOR_POSITION_STEPS = 0;
// volatile int MOTOR_POSITION_PERCENT = 0;

char *WEBHOOK_URL = "http://192.168.1.84:8000/devices/smartblinds/";
// char *WEBHOOK_URL = "http://roscoethedog.pythonanywhere.com/devices/smartblinds/";
char *LOCAL_SERVER_ADDRESS = "http://192.168.1.84:8000/devices/smartblinds/";
char *REMOTE_SERVER_ADDRESS = "http://roscoethedog.pythonanywhere.com/devices/smartblinds/";
char *WIFI_SSID = "wutangLAN";
char *WIFI_PASSWORD = "c@$T131nTh3$Ky";
char *WRITE_KEY = "abc";
char *READ_KEY = "abc";
char *LOCAL_DEVICE_ID = "1";

volatile int DATETIME_SYNCED = 0;

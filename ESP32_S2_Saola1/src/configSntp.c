#include <configSntp.h>



void initializeSntpUpdate() {
	// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html
	char* TAG = "initializeSntpUpdate";

	// "PST8PDT,M3.2.0,M11.1.0"

	ESP_LOGI(TAG, "initializing sntpUpdate service");

	sntp_stop();
	// Set timezone to PST Standard Time (USA WEST)
	setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
	tzset();
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	// sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
	sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
	sntp_setservername(0, "pool.ntp.org");
	sntp_set_time_sync_notification_cb(sntpUpdateNotification);
	
	sntp_init();

	// while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
    //     ESP_LOGI(TAG, "Waiting for system time to be set...");
    //     vTaskDelay(pdMS_TO_TICKS(2000));
    // }

	// struct timeval T;

	// while(1)  {

	// 	if (WIFI_CONNECTED) {
	// 		time(&now);
	// 		localtime_r(&now, &timeinfo);
	// 		strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	// 		ESP_LOGI(TAG, "The current date/time in PST is: %s", strftime_buf);
	// 		// printf("%i\n", gettimeofday(&now, &timeinfo));
	// 	}

	// 	vTaskDelay(pdMS_TO_TICKS(1000));
	// }
}

void sntpUpdateNotification() {
	char *TAG = "sntpUpdateNotificiation";
	time_t now;
	char strftime_buf[64];
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(TAG, "server updated date/time to %s PST", strftime_buf);
	DATETIME_SYNCED = true;
}

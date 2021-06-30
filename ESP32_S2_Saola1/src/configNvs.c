#include <configNvs.h>

void nvsWriteBlob(char *location, char *key, void *args, size_t size) {
	nvs_flash_init();
	nvs_handle_t storage_handle;
	ESP_ERROR_CHECK(nvs_open(location, NVS_READWRITE, &storage_handle));
	ESP_ERROR_CHECK(nvs_set_blob(storage_handle, key, args, size));
	ESP_ERROR_CHECK(nvs_commit(storage_handle));
	nvs_close(storage_handle);
}

void* nvsReadBlob(char *location, char *key, size_t size) {
	nvs_flash_init();
	// declare a nvs handler and our wifi config to restore.
	nvs_handle_t storage_handle;
	// wifi_config_t wifi_config;
	void *nvs_data = (void*)malloc(sizeof(void*));
	size_t nvs_buffer = size;
	// open the wifi_settings namespace with read/write permissions, passing in the handler.
	ESP_ERROR_CHECK(nvs_open(location, NVS_READWRITE, &storage_handle));

	esp_err_t err = nvs_get_blob(storage_handle, key, &nvs_data, &nvs_buffer);
	if (err == ESP_OK) {
		return nvs_data;
	} else {
		// if didn't read data, report to serial
		ESP_ERROR_CHECK(err);
		return NULL;
	}

}

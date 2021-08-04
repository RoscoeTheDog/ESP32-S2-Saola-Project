#include <configNvs.h>
#include <esp_wifi.h>

void nvsWriteBlob(const char *location, const char *key, void *args, size_t size) {
	nvs_flash_init();
	nvs_handle_t storage_handle;
	ESP_ERROR_CHECK(nvs_open(location, NVS_READWRITE, &storage_handle));
	ESP_ERROR_CHECK(nvs_set_blob(storage_handle, key, args, size));
	ESP_ERROR_CHECK(nvs_commit(storage_handle));
	nvs_close(storage_handle);
}

void* nvsReadBlob(const char *location, const char *key, size_t size) {
	nvs_flash_init();

	nvs_handle_t storage_handle;
	size_t nvs_buffer = size;
	void *nvs_data = NULL;

	esp_err_t err = nvs_open(location, NVS_READWRITE, &storage_handle);
	while (err == ESP_OK) {
		nvs_data = (void*)malloc(sizeof(void*));
		err = nvs_get_blob(storage_handle, key, nvs_data, &nvs_buffer);

		if (err != ESP_OK) {
			return NULL;
		}
		return nvs_data;
	}

	return NULL;
}

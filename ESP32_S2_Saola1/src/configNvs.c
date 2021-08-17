#include <configNvs.h>
#include <esp_wifi.h>

// key maximum length is 15 characters. 
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
		nvs_data = (void*)pvPortMalloc(sizeof(void*));
		err = nvs_get_blob(storage_handle, key, nvs_data, &nvs_buffer);

		if (err != ESP_OK) {
			return NULL;
		}
		return nvs_data;
	}

	return NULL;
}

esp_err_t nvsRestoreSystemState() {
	char *TAG = "nvsRestoreSystemState";
	ESP_LOGI(TAG, "RESTORING PREVIOUS STATE");
	cJSON *container = cJSON_CreateObject();
	void *validator = NULL;
	esp_err_t err = ESP_OK;

	validator = nvsReadBlob("init", "ROD_DIAMETER", sizeof(float));
	if (validator) {
		memcpy(&ROD_DIAMETER_MM, validator, sizeof(float));
		cJSON_AddNumberToObject(container, "ROD_DIAMETER_MM", ROD_DIAMETER_MM);
		vPortFree(validator);
	}
	
	validator = nvsReadBlob("init", "MATERIAL_THICK", sizeof(float));
	if (validator) {
		memcpy(&MATERIAL_THICKNESS_MM, validator, sizeof(float));
		cJSON_AddNumberToObject(container, "MATERIAL_THICKNESS", MATERIAL_THICKNESS_MM);
		vPortFree(validator);
	}
	
	validator = nvsReadBlob("init", "MOTOR_SPEED", sizeof(int));
	if (validator) {
		memcpy(&MOTOR_SPEED_RPM, validator, sizeof(int));
		cJSON_AddNumberToObject(container, "MOTOR_SPEED_RPM", MOTOR_SPEED_RPM);
		vPortFree(validator);
	}
	
	validator = nvsReadBlob("init", "MOTOR_STEPS", sizeof(long));
	if (validator) {
		memcpy(&MOTOR_POSITION_STEPS, validator, sizeof(long));
		cJSON_AddNumberToObject(container, "MOTOR_POSITION_STEPS", MOTOR_POSITION_STEPS);
		vPortFree(validator);
	} else {
		return ESP_FAIL;
	}
	
	validator = nvsReadBlob("init", "CURTAIN_PERC", sizeof(float));
	if (validator) {
		memcpy(&CURTAIN_PERCENTAGE, validator, sizeof(float));
		cJSON_AddNumberToObject(container, "CURTAIN_PERCENTAGE", CURTAIN_PERCENTAGE);
		vPortFree(validator);
	} else {
		return ESP_FAIL;
	}
	
	char *str = cJSON_PrintUnformatted(container);
	ESP_LOGI(TAG, "STATE RESTORED: %s", str);
	cJSON_free(str);
	cJSON_Delete(container);

	return ESP_OK;
}

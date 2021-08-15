#include <httpRequests.h>
#include <esp_log.h>
#include <cJSON.h>
#include <configSteppers.h>
#include <freertos/task.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>
#include <stdio.h>
#include <configWifi.h>

volatile bool HTTP_ERROR = false;
TaskHandle_t xHandleParseWebServer;
char HTTP_RESPONSE_DATA[2048];
cJSON *formSent = NULL;
cJSON *formReceived = NULL;
char *jsonStringBuffer = NULL;

esp_http_client_handle_t client;    // please note, this is a double pointer. do not set to null!
esp_http_client_config_t config;

esp_err_t initializeHttpClient() {
    char *TAG = "initializeHttpClient";
    esp_err_t err = ESP_FAIL;

    config.url = WEBHOOK_URL;
    ESP_LOGI(TAG, "setting POST url to: %s", config.url);
    config.event_handler = http_event_handler;
    config.method = HTTP_METHOD_POST;

    if (client == NULL) {
        ESP_LOGI(TAG, "initializing new http client");
        client = esp_http_client_init(&config);
        
        if (client) {
            err = ESP_OK;
        }

    } else {
        // we do not want any tasks to interrupt the freeing/re-init of the client object.
        // portENTER_CRITICAL(&mux);
        ESP_LOGD(TAG, "deinitializing http client");
        err = esp_http_client_close(client);
        ESP_LOGI(TAG, "initializing new http client");
        client = esp_http_client_init(&config);
        // portEXIT_CRITICAL(&mux);
    }

    return err;
}

esp_err_t httpPostData(char *c) {
    char *TAG = "httpPostData";
    esp_err_t err = ESP_FAIL;
    ESP_LOGI(TAG, "HTTP REQUEST -- %s", c);

    // ensure the http client is initialiazed. This is normally done upon boot.
    if (!client) {
        err = initializeHttpClient();

        // exit if failed to initialize the client.
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "Failed to initialize http client. Request Dropped!");
            return err;
        }
    }
    // set header data with content type
    err = esp_http_client_set_header(client, "Accept", "application/json");
    err = esp_http_client_set_header(client, "Content-Type", "application/json");

    // set field data
    err = esp_http_client_set_post_field(client, c, strlen(c));

    if (WIFI_CONNECTED) {
        err = esp_http_client_perform(client);
    }

    return err;
}

esp_err_t httpFetchServerData() {
    char *TAG = "httpFetchServerData";
    esp_err_t err = ESP_FAIL;   // initialized to null/fail 

    portENTER_CRITICAL(&mux);
    if (!formSent){
        formSent = cJSON_CreateObject();
    } else {
        cJSON_Delete(formSent);
        formSent = cJSON_CreateObject();
    }
    portEXIT_CRITICAL(&mux);
    cJSON_AddStringToObject(formSent, "READ_KEY", READ_KEY);
    cJSON_AddStringToObject(formSent, "DeviceID", LOCAL_DEVICE_ID);

    // parse the JSON object into string type to prepare for http client request
    portENTER_CRITICAL(&mux);
    if (jsonStringBuffer) {
        cJSON_free(jsonStringBuffer);
        jsonStringBuffer = NULL;
    }
    portEXIT_CRITICAL(&mux);
    jsonStringBuffer = cJSON_PrintUnformatted(formSent);
    err = httpPostData(jsonStringBuffer);
    // taskEXIT_CRITICAL(&mux);
    // long timeStart = esp_timer_get_time();
    // do {
    //     err = httpPostData(jsonStringBuffer);
    //     ESP_LOGI(TAG, "POST FAILED");
    //     ESP_LOGI(TAG, "RETRYING PENDING POST -- %s", jsonStringBuffer);
    //     vTaskDelay(pdMS_TO_TICKS(100));
    //     // 3 second retry timeout
    //     if (esp_timer_get_time() - timeStart > 3 * 1000 * 1000) {
    //         break;
    //     }
    // } while (err != ESP_OK);

    return err;
}

esp_err_t httpParseServerData() {
    char *TAG = "httpParseServerData";

    // taskENTER_CRITICAL(&mux);
    portENTER_CRITICAL(&mux);
    if (formReceived) {
        cJSON_Delete(formReceived);
        formReceived = NULL;
    }
    portEXIT_CRITICAL(&mux);
    // Parse Json data from string. Note that cJSON dynamically allocates memory that must be freed later.
    formReceived = cJSON_Parse(HTTP_RESPONSE_DATA);

    ESP_LOGI(TAG, "RESPONSE VALIDATION START");
    if (formReceived == cJSON_Invalid || formReceived == NULL) {
        ESP_LOGI(TAG, "FAILED: Form receieved is invalid JSON");
        HTTP_ERROR = true;
        portENTER_CRITICAL(&mux);
        if (formReceived) {
            cJSON_Delete(formReceived);
            formReceived = NULL;
        }
        portEXIT_CRITICAL(&mux);

        return ESP_FAIL;
    } else {
        portENTER_CRITICAL(&mux);
        if (jsonStringBuffer) {
            cJSON_free(jsonStringBuffer);
            jsonStringBuffer = NULL;
        }
        portEXIT_CRITICAL(&mux);
        jsonStringBuffer = cJSON_PrintUnformatted(formReceived);
        ESP_LOGI(TAG, "%s", jsonStringBuffer);
        HTTP_ERROR = false;
    }
    // taskEXIT_CRITICAL(&mux);

    char *str;
    esp_err_t err = ESP_OK;
    // update global macros if they match the parsed keys.
    if (formReceived) {
        
        // we do not care about device authorization if it is a cold boot, as we need to
        // restore the last known position the device was in.
        if (SYS_SYNC) {
            if (cJSON_HasObjectItem(formReceived, "USERNAME")) {
                str = cJSON_GetObjectItem(formReceived, "USERNAME")->valuestring;

                // note that strcmp returns '0' if true
                if (strcmp(str, USERNAME) == 0) {
                    ESP_LOGI(TAG, "JSON FAILED: 'USERNAME' of request is same as local device");
                    portENTER_CRITICAL(&mux);
                    // free the dynamically allocated memory. 
                    // Do not forget that the print functions also may allocate arrays. Free those as well!!!
                    cJSON_Delete(formReceived);
                    formReceived = NULL;
                    portEXIT_CRITICAL(&mux);
                    err = ESP_FAIL;
                }

            }
       
       }

        if (cJSON_HasObjectItem(formReceived, "MATERIAL_THICKNESS_MM")) {
            str = cJSON_GetObjectItem(formReceived, "MATERIAL_THICKNESS_MM")->valuestring;

            if (strlen(str) > 0) {
                float val = atof(str);
                MATERIAL_THICKNESS_MM = val;
            }
        }   

        if (cJSON_HasObjectItem(formReceived, "MOTOR_SPEED_RPM")) {
            str = cJSON_GetObjectItem(formReceived, "MOTOR_SPEED_RPM")->valuestring;

            if (strlen(str) > 0) {
                float val = atof(str);

                if (val >= 10 && val <= 400) {
                    MOTOR_SPEED_RPM = val;
                    setStepperRPM(StepperMotor_1, MOTOR_SPEED_RPM);
                }
            }
        }

        if (cJSON_HasObjectItem(formReceived, "ROD_DIAMETER_MM")) {
            str = cJSON_GetObjectItem(formReceived, "ROD_DIAMETER_MM")->valuestring;

            if (strlen(str) > 0) {
                float val = atof(str);
                if (val >= 0 && val <= 25.4 * 12) {     // limit max to 12" diam
                    ROD_DIAMETER_MM = val;
                }                    
            }
        }

        if (cJSON_HasObjectItem(formReceived, "CURTAIN_LENGTH_INCH")) {
            str = cJSON_GetObjectItem(formReceived, "CURTAIN_LENGTH_INCH")->valuestring;
       
            if (strlen(str) > 0) {
                float val = atof(str);
                if (val >= 0 && val <= 10 * 12) {     // limit max to 12" diam
                    CURTAIN_LENGTH_INCH = val;
                }                    
            }
        }

        // parse percentage last, as it is dependent on other parsed parameters
        if (cJSON_HasObjectItem(formReceived, "CURTAIN_PERCENTAGE")) {
            str = cJSON_GetObjectItem(formReceived, "CURTAIN_PERCENTAGE")->valuestring;

            if (strlen(str) > 0) {
                float val = atof(str);

                if (val >= 0 && val <= 100) {
                    CURTAIN_PERCENTAGE = val;

                    // as motor's position is uninitialized at startup, we need to restore
                    // the value by calculating the set percentage.
                    if (!SYS_SYNC) {
                        // TODO: make this account for the material thickness around the rod dynamically
                        int length_mm = CURTAIN_LENGTH_INCH * 25.4;
                        int circumference = 2 * M_PI * (ROD_DIAMETER_MM/2);
                        MOTOR_POSITION_STEPS = (CURTAIN_PERCENTAGE * .01) * calcStepsForRotation(StepperMotor_1, (length_mm / circumference) * 360);
                    }
                }
            }
        }

        portENTER_CRITICAL(&mux);
        // free the dynamically allocated memory. 
        // Do not forget that the print functions also may allocate arrays. Free those as well!!!
        cJSON_Delete(formReceived);
        formReceived = NULL;
        portEXIT_CRITICAL(&mux);
    }

    ESP_LOGI(TAG, "VALIDATION COMPLETE");
    return err;
}

esp_err_t httpValidateFormSubmission(char *form) {
    char *TAG = "httpValidateSubmission";
    ESP_LOGD(TAG, "TASK AVAILABLE HEAP: %i", xPortGetFreeHeapSize());
    ESP_LOGI(TAG, "VALIDATING FORM DATA");
    
    portENTER_CRITICAL(&mux);
    if (formReceived) {
        cJSON_Delete(formReceived);
        formReceived = NULL;
    }
    portEXIT_CRITICAL(&mux);

    // Parse Json data from string. Note that cJSON dynamically allocates memory that must be freed later.
    formReceived = cJSON_Parse(HTTP_RESPONSE_DATA);

    // begin data validation.
    if (formSent == cJSON_Invalid || formSent == NULL) {
        ESP_LOGI(TAG, "WARNING: input form data is invalid JSON");

        portENTER_CRITICAL(&mux);
        if (formSent) {
            cJSON_Delete(formSent);
            formSent = NULL;
        }
        portEXIT_CRITICAL(&mux);

        return ESP_FAIL;
    }
    if (formReceived == cJSON_Invalid || formReceived == NULL) {
        ESP_LOGI(TAG, "WARNING: response form data is invalid JSON");

        portENTER_CRITICAL(&mux);
        if (formReceived) {
            cJSON_Delete(formReceived);
            formReceived = NULL;
        }
        portEXIT_CRITICAL(&mux);

        return ESP_FAIL;
    }

    if (formSent) {
        portENTER_CRITICAL(&mux);
        if (jsonStringBuffer) {
            cJSON_free(jsonStringBuffer);
            jsonStringBuffer = NULL;
        }
        portEXIT_CRITICAL(&mux);
        jsonStringBuffer = cJSON_PrintUnformatted(formSent);
        ESP_LOGI(TAG, "SEND form: %s", jsonStringBuffer);
    }
    if (formReceived) {
        portENTER_CRITICAL(&mux);
        if (jsonStringBuffer) {
            cJSON_free(jsonStringBuffer);
            jsonStringBuffer = NULL;
        }
        portEXIT_CRITICAL(&mux);
        jsonStringBuffer = cJSON_PrintUnformatted(formReceived);
        ESP_LOGI(TAG, "RECEIVE form: %s", jsonStringBuffer);
    }

    if (cJSON_Compare(formSent, formReceived, 1)) {
        ESP_LOGI(TAG, "FORM VALIDATED TRUE");
        return ESP_OK;
    } else {
        ESP_LOGI(TAG, "FORM VALIDATED FALSE");
        return ESP_FAIL;
    }

    // if (json_data != cJSON_Invalid) {

    //     if (cJSON_HasObjectItem(json_data, "CURTAIN_PERCENTAGE")) {
    //         str = cJSON_GetObjectItem(json_data, "CURTAIN_PERCENTAGE")->valuestring;

    //         if (strlen(str) > 0) {
    //             float val = atof(str);
                
    //             if (val != CURTAIN_PERCENTAGE) {
    //                 ESP_LOGI(TAG, "ERROR: CONFIRMATION RESPONSE DOES NOT MATCH SEND DATA!");
    //                 err = ESP_FAIL;
    //             }
    //         }
    //     }
        
    // }

    // cJSON_Delete(json_data);
    // cJSON_Delete(form_data);

    return ESP_OK;
}

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    TaskHandle_t local_task = xTaskGetCurrentTaskHandle();
    vTaskPrioritySet(local_task, 22);

    char *TAG = "http_event_handler";
    ESP_LOGD(TAG, "TASK AVAILABLE HEAP: %i", xPortGetFreeHeapSize());
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            // ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            // ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            // printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                ESP_LOGI(TAG, "SERVER RESPONSE: %.*s", evt->data_len, (char*)evt->data);
                // update the global event data so the running task can parse it after it is signaled
                memcpy(HTTP_RESPONSE_DATA, evt->data, sizeof(char) * evt->data_len);
            }
            // xTaskCreate((TaskFunction_t) httpParseServerData, "httpParseServerData", 2048, evt->data, 3, &xHandleParseWebServer);
            break;
        case HTTP_EVENT_ON_FINISH:
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            ESP_LOGI(TAG, "Status = %d, content_length = %d", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
            break;
        case HTTP_EVENT_DISCONNECTED:
            // ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }

    return ESP_OK;
}



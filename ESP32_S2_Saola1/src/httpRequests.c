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
bool initialized = false;

esp_http_client_handle_t client;
esp_http_client_config_t config;

void vInitTaskParseWebserver() {
    ;
    // xTaskCreate(http_parse_server_data, "http_parse_server_data", 2048, NULL, 25, &xHandleParseWebServer);
    // assert(xHandleParseWebServer);
}

void temp_wrapper() {

    if (!initialized) {
        config.url = WEBHOOK_URL;
        config.event_handler = http_event_handle;
        config.method = HTTP_METHOD_POST;
        config.keep_alive_enable = true;
        client = esp_http_client_init(&config);


        initialized = true;
    }

}

esp_err_t http_request_server_data() {
    char *TAG = "http_request_server_data";
    temp_wrapper();

    // create buffer for HTTP request form
    // populate form/string with neccessary body information to fetch data from web server
    const static int bufferLength = 256;
    char postData[bufferLength];
    strcpy(postData, "READ_KEY=");
    strcat(postData, READ_KEY);
    strcat(postData, "&DeviceID=");
    strcat(postData, LOCAL_DEVICE_ID);
    ESP_LOGI(TAG, "Client POST Request Form - %s", postData);

    // // create config struct. attach handler for events here.
    // esp_http_client_config_t config = {
    //     .url = WEBHOOK_URL,
    //     .event_handler = http_event_handle,
    //     .method = HTTP_METHOD_POST,
    //     // .is_async = true,
    // };

    // initialize the client.
    // esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_post_field(client, postData, bufferLength);

    esp_err_t err = ESP_FAIL;   // initialized to null/fail 
    // attempt to connect to the host
    if (WIFI_CONNECTED) {
        err = esp_http_client_perform(client);
        // display contents and status code. 
        ESP_LOGI(TAG, "Status = %d, content_length = %d", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
        
        if (err == ESP_OK) {    
            ESP_LOGI(TAG, "Closing Http Client");
            ESP_ERROR_CHECK(esp_http_client_close(client));
        } else {
            esp_http_client_close(client);
            esp_http_client_init(&client);
        }
        
    }

    // ESP_LOGI(TAG, "Freeing client memory");
    // // call the cleanup method. essentially a destructor for the connected client.
    // ESP_ERROR_CHECK(esp_http_client_cleanup(client));

    // vPortFree(postData);
    // vPortFree(&client);
    // vPortFree(&config);
    
    return err;
}

esp_err_t http_parse_server_data() {

    char *TAG = "http_parse_server_data";
    char *str;
    esp_err_t err = ESP_FAIL;

    // configure the JSON library to use FREERTOS thread safe malloc / free methods
    cJSON_Hooks hooks;
    hooks.malloc_fn = pvPortMalloc;
    hooks.free_fn = vPortFree;
    cJSON_InitHooks(&hooks);

    // Parse Json data from string. Note that cJSON dynamically allocates memory that must be freed later.
    cJSON *json_data = cJSON_Parse(HTTP_RESPONSE_DATA);
    ESP_LOGI(TAG, "HTTP_DATA_RECEIVED:");
    // begin data validation.
    if (json_data == cJSON_Invalid) {
        ESP_LOGI(TAG, "WARNING: form is invalid JSON");
        HTTP_ERROR = true;
    } else {
        str = cJSON_Print(json_data);
        ESP_LOGI(TAG, "\n%s", str);
        cJSON_free(str);
        HTTP_ERROR = false;
        err = ESP_OK;
    }

    // update global macros if they match the parsed keys.
    if (json_data != cJSON_Invalid) {

        if (cJSON_HasObjectItem(json_data, "MATERIAL_THICKNESS_MM")) {

            // memcpy(str, cJSON_GetObjectItem(json_data, "MATERIAL_THICKNESS_MM")->valuestring, sizeof(char) * strlen(cJSON_GetObjectItem(json_data, "MATERIAL_THICKNESS_MM")->valuestring) + 1);
            str = cJSON_GetObjectItem(json_data, "MATERIAL_THICKNESS_MM")->valuestring;

            if (strlen(str) > 0) {
                float val = atof(str);
                MATERIAL_THICKNESS_MM = val;
            }
        }   

        if (cJSON_HasObjectItem(json_data, "MOTOR_SPEED_RPM")) {
            str = cJSON_GetObjectItem(json_data, "MOTOR_SPEED_RPM")->valuestring;
            // memcpy(str, cJSON_GetObjectItem(json_data, "MOTOR_SPEED_RPM")->valuestring, sizeof(char) * strlen(cJSON_GetObjectItem(json_data, "MOTOR_SPEED_RPM")->valuestring) + 1);

            if (strlen(str) > 0) {
                float val = atof(str);

                if (val >= 10 && val <= 400) {
                    MOTOR_SPEED_RPM = val;
                    setStepperRPM(StepperMotor_1, MOTOR_SPEED_RPM);
                }
            }
        }

        if (cJSON_HasObjectItem(json_data, "CURTAIN_PERCENTAGE")) {
            str = cJSON_GetObjectItem(json_data, "CURTAIN_PERCENTAGE")->valuestring;
            // memcpy(str, cJSON_GetObjectItem(json_data, "CURTAIN_PERCENTAGE")->valuestring, sizeof(char) * strlen(cJSON_GetObjectItem(json_data, "CURTAIN_PERCENTAGE")->valuestring) + 1);

            if (strlen(str) > 0) {
                float val = atof(str);

                if (val >= 0 && val <= 100) {
                    CURTAIN_PERCENTAGE = val;
                    // TODO: make this account for the material thickness around the rod dynamically
                    int length_mm = CURTAIN_LENGTH_INCH * 25.4;
                    int circumference = 2 * M_PI * (ROD_DIAMETER_MM/2);
                    MOTOR_POSITION_STEPS = (CURTAIN_PERCENTAGE * .01) * calcStepsForRotation(StepperMotor_1, (length_mm / circumference) * 360);
                }
            }
        }

        if (cJSON_HasObjectItem(json_data, "ROD_DIAMETER_MM")) {
            str = cJSON_GetObjectItem(json_data, "ROD_DIAMETER_MM")->valuestring;
            // memcpy(str, cJSON_GetObjectItem(json_data, "ROD_DIAMETER_MM")->valuestring, sizeof(char) * strlen(cJSON_GetObjectItem(json_data, "ROD_DIAMETER_MM")->valuestring) + 1);

            if (strlen(str) > 0) {
                float val = atof(str);
                if (val >= 0 && val <= 25.4 * 12) {     // limit max to 12" diam
                    ROD_DIAMETER_MM = val;
                }                    
            }
        }

        if (cJSON_HasObjectItem(json_data, "CURTAIN_LENGTH_INCH")) {
            str = cJSON_GetObjectItem(json_data, "CURTAIN_LENGTH_INCH")->valuestring;
            // memcpy(str, cJSON_GetObjectItem(json_data, "CURTAIN_LENGTH_INCH")->valuestring, sizeof(char) * strlen(cJSON_GetObjectItem(json_data, "CURTAIN_LENGTH_INCH")->valuestring) + 1);
            
            if (strlen(str) > 0) {
                float val = atof(str);
                if (val >= 0 && val <= 10 * 12) {     // limit max to 12" diam
                    CURTAIN_LENGTH_INCH = val;
                }                    
            }
        }

        // free the dynamically allocated memory. 
        // Do not forget that the print functions also may allocate arrays. Free those as well!!!
        cJSON_Delete(json_data);
    }

    return err;
}

esp_err_t http_event_handle(esp_http_client_event_t *evt)
{
    char *TAG = "http_event_handle";
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
                ESP_LOGI(TAG, "server response data: %.*s", evt->data_len, (char*)evt->data);
                // update the global event data so the running task can parse it after it is signaled
                memcpy(HTTP_RESPONSE_DATA, evt->data, sizeof(char) * evt->data_len);
            }
            // xTaskCreate((TaskFunction_t) http_parse_server_data, "http_parse_server_data", 2048, evt->data, 3, &xHandleParseWebServer);
            break;
        case HTTP_EVENT_ON_FINISH:
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            // ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }

    return ESP_OK;
}



#include <esp_http_client.h>
#include <globals.h>
#include <string.h>
#include <freertos/task.h>
#include <esp_sleep.h>
#include <globals.h>
#include <cJSON.h>
#include <cJSON_Utils.h>

extern volatile bool HTTP_ERROR;
extern esp_http_client_handle_t client;
extern esp_http_client_config_t config;
extern cJSON *formSent;
extern cJSON *formReceived;
extern char *jsonStringBuffer;

extern TaskHandle_t xHandleParseWebServer;

extern esp_err_t initializeHttpClient();

extern esp_err_t httpPostData(char *c);

extern esp_err_t http_event_handler(esp_http_client_event_t *evt);

extern esp_err_t httpFetchServerData();

extern esp_err_t httpParseServerData();

extern esp_err_t httpValidateFormSubmission(char *form);

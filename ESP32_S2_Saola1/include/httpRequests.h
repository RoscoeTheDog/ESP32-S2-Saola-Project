#include <esp_http_client.h>
#include <globals.h>
#include <string.h>
#include <freertos/task.h>
#include <esp_sleep.h>

extern volatile bool HTTP_ERROR;

extern TaskHandle_t xHandleParseWebServer;

extern esp_err_t http_event_handle(esp_http_client_event_t *evt);

extern esp_err_t http_request_server_data();

extern esp_err_t http_parse_server_data();

extern void xInitTaskParseWebServer();



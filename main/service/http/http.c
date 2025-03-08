#include "http.h"
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "app_config.h"
#include <sys/param.h>
#include "esp_tls.h"
#include "esp_crt_bundle.h"

static char* TAG = "HTTP_CLIENT";
static char response_buffer[HTTP_BUFF_SIZE];
esp_err_t _http_event_handler(esp_http_client_event_t *evt);
SemaphoreHandle_t request_mutex; // Mutex to protect request data.
static char * http_request_send(const char *url, 
                                const char *post_data, 
                                int* size); //send http requests
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*  *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
                *  However, event handler can also be used in case chunked encoding is used.
            */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                int copy_len = 0;
                if (evt->user_data) {
                    // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                    copy_len = MIN(evt->data_len, HTTP_BUFF_SIZE);
                    if (copy_len) {
                        memset(evt->user_data, 0, copy_len);
                        memcpy(evt->user_data, evt->data, copy_len);
                    }
                }
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}
                                
char* http_request_post(const char * Url, const char *post_data,int* size) {
    if(post_data == NULL) {
        ESP_LOGI(TAG, "Post data is NULL");
        return NULL;
    }
    if(Url == NULL) {
        ESP_LOGI(TAG, "URL is NULL");
        return NULL;
    }
    return http_request_send(Url, post_data, size);
}
char* http_request_get(const char * Url, int *size){
    if(Url == NULL) {
        ESP_LOGI(TAG, "URL is NULL");
        return NULL;
    }
    return http_request_send(Url, NULL, size);
}


void http_post_test() {
    int size = 0;
    ESP_LOGI(TAG, "HTTP Start POST TEST");
    http_lock();
    char * response = http_request_get(API_UPDATE_TIME, &size);
    ESP_LOGI(TAG, "Data response: %s", response);
    http_unlock();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
char * http_request_send(const char * url, const char *post_data, int* size)
{
    ESP_LOGI(TAG, "HTTP Request URL: %s", url);
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = response_buffer,        // Pass address of local buffer to get response
        .buffer_size = 1024,
        .crt_bundle_attach = esp_crt_bundle_attach
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status != HttpStatus_Ok) {
            ESP_LOGE(TAG, "Received incorrect http status %d", status);
            err = ESP_FAIL;
        }
        else {
            ESP_LOGI(TAG, "HTTP POST Status = %d", esp_http_client_get_status_code(client));
        }
    } else {
        ESP_LOGE(TAG, "ESP HTTP client perform failed: %d", err);
    }
    esp_http_client_cleanup(client);
    *(size) = (strlen(response_buffer) + 1);
    return response_buffer;
}

void http_init()
{
    request_mutex = xSemaphoreCreateMutex();
}
void http_lock()
{
    xSemaphoreTake(request_mutex, portMAX_DELAY);
}
void http_unlock()
{
    xSemaphoreGive(request_mutex);
}

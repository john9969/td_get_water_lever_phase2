#include "measuring_app.h"
#include "app_config.h"
#include "esp_log.h"
#include "esp_system.h"
#include "tiny-json.h"
#include "led.h"
#include "rtc.h"
#include "http.h"
static const char* TAG = "MEASURING_APP";

static bool measuring_app_set_state(void * arg, MEASURING_APP_STATE_t state);
static bool post_data_measuring(void* arg, void* data);
static bool sync_time(void* arg, void* data, int* size);

void measuring_app_init(void * arg){
    MeasuringApp * app = (MeasuringApp *)arg;
    app->state = MEASURING_APP_STATE_INIT;
    ESP_LOGI(TAG,"Measuring app init");
    xTaskCreate(measuring_app_process,"Measuring app process",1024*2,(void*)app,3,&app->measuring_task);
}

void measuring_app_deinit(void* arg){
    MeasuringApp * app = (MeasuringApp *)arg;
    vTaskDelete(app->measuring_task);
    app->measuring_task = NULL;
}

void measuring_app_process(void* arg){
    MeasuringApp * app = (MeasuringApp *)arg;
    while (1)
    {
        switch (app->state)
        {
        case MEASURING_APP_STATE_INIT:
            //sensor init
            //led_init(&led_control);                                                        
            ESP_LOGI(TAG,"Measuring app init");
            app->state = MEASURING_APP_STATE_MEASURING;
            break;
        case MEASURING_APP_STATE_MEASURING:
            ESP_LOGI("MEASURING_APP","Measuring app measuring");
            vTaskDelay(pdMS_TO_TICKS(100000));
            app->state = MEASURING_APP_STATE_POSTING;
            break;
        case MEASURING_APP_STATE_POSTING:
            ESP_LOGI(TAG,"Measuring app posting");
            app->state = MEASURING_APP_STATE_SET_TIME;
            break;
        case MEASURING_APP_STATE_SET_TIME:
            ESP_LOGI(TAG,"Measuring app set time");
            app->state = MEASURING_APP_STATE_DONE;
            break;
        case MEASURING_APP_STATE_DONE:
            ESP_LOGI(TAG,"Measuring app done");
            app->state = MEASURING_APP_STATE_WAITING;
            break;
        case MEASURING_APP_STATE_WAITING:
            ESP_LOGI(TAG,"Measuring app waiting");
            app->state = MEASURING_APP_STATE_ERROR;
            break;
        case MEASURING_APP_STATE_ERROR:
            ESP_LOGI(TAG,"Measuring app error");
            app->state = MEASURING_APP_STATE_INIT;
            break;
        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
bool measuring_app_set_state(void * arg, MEASURING_APP_STATE_t state) {
    MeasuringApp * app = (MeasuringApp* )arg;
    if(app->state == state) return false;
    switch (state)
    {
    case MEASURING_APP_STATE_INIT:
        set_led_state(&led_control,LED_BLINK_3000MS,-1);
        break;
    case MEASURING_APP_STATE_MEASURING:
        set_led_state(&led_control,LED_BLINK_3000MS,5);
        break;
    default:
        break;
    }
}
void uart_pattern_detected(void *arg, uint8_t *data, uint32_t len){
    ESP_LOGI(TAG,"Pattern detected");
    ESP_LOGI(TAG,"Length: %d",(int)len);
    for (int i = 0; i < len; i++)
    {
        ESP_LOGI(TAG,"Data: %c",data[i]);
    }

}
static bool post_data_measuring(void* arg, void* data){
    ESP_LOGI(TAG,"Post data");
    char response[HTTP_BUFF_SIZE] = {0};
    char *result = NULL;
    int size = 0;
    http_lock();
    result = http_request_post(API_POST_WATER,data,&size);
    if(result == NULL || size == 0){
        ESP_LOGI(TAG,"Post data failed");
        http_unlock();
        return false;
    }
    memcpy(response, result, size);
    
    http_unlock();
    ESP_LOGI(TAG,"Post data success");
    ESP_LOGI(TAG,"Response: %s",response);
    return true;

}
static bool sync_time(void* arg, void* data, int* size){
    ESP_LOGI(TAG,"Sync time");
    char response[HTTP_BUFF_SIZE] = {0};
    char *result = NULL;
    *size = 0;
    http_lock();
    result = http_request_get(API_UPDATE_TIME,size);
    if(result == NULL || *size == 0){
        ESP_LOGI(TAG,"Sync time failed");
        http_unlock();
        return false;
    }
    memcpy(response, result, *size);
    http_unlock();
    ESP_LOGI(TAG,"Sync time success");
    ESP_LOGI(TAG,"Response: %s",response);
    RTC_set_time_from_string(response);
    return true;
}



MeasuringApp measuring_app = {
    .error = {
        {
            .error_string = "No error",
            .error_code = ERROR_CODE_NONE
        }
    },
    .state = MEASURING_APP_STATE_INIT
};

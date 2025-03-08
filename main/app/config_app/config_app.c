#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "lazer_sensor.h"
#include "dcom.h"
#include "led.h"
#include "config_app.h"
#include "app_config.h"
static const char *TAG = "CONFIG_APP";
static void config_app_process(void* arg);

void config_app_init(void * arg){
    ConfigApp * p_app_config = (ConfigApp *)arg;
    p_app_config->state = STATE_INIT;
    ESP_LOGI("CONFIG_APP","Config app init");
    xTaskCreate(config_app_process,"Config app process",1024*2,(void*)p_app_config,3,&p_app_config->config_task);
}

void config_app_deinit(void * arg){
    ConfigApp * app = (ConfigApp *)arg;
    vTaskDelete(app->config_task);
    app->config_task = NULL;
}
void config_app_process(void *arg){
    ConfigApp * p_config_app = (ConfigApp *)arg;
    for(;;){
        switch(p_config_app->state){
            case STATE_INIT:
                set_led_state(&led_control,LED_BLINK_CONFIG,-1);
                p_config_app->state = STATE_ON_SENSOR;
                ESP_LOGI(TAG,"Config app init");
                break;
            case STATE_ON_SENSOR:
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                ESP_LOGI(TAG,"Config app on sensor");
                p_config_app->state = STATE_ON_DCOM;
                break;
            case STATE_ON_DCOM:
                ESP_LOGI(TAG,"Config app on dcom");
                p_config_app->state = STATE_WAITNG;
                break;
                case STATE_WAITNG:
                // get time and post info to  server 
                vTaskDelay  (5000 / portTICK_PERIOD_MS);
            default:
                break;
        }
    }
    
}
ConfigApp configApp = {
    .state = STATE_INIT,
    .config_task = NULL
};
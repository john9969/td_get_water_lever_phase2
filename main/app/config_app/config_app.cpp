#include "config_app.h"
static void config_app_process(void* arg);
void config_app_init(void * arg){
    ConfigApp * app = (ConfigApp *)arg;
    app->setupState = STATE_INIT;
    ESP_LOGI("CONFIG_APP","Config app init");
    xTaskCreate(config_app_process,"Config app process",1024*2,(void*)app,3,&app->config_task);
}

void config_app_deinit(void * arg){
    ConfigApp * app = (ConfigApp *)arg;
    vTaskDelete(app->config_task);
    app->config_task = NULL;
}
void config_app_process(void *arg){
    ConfigApp * app = (ConfigApp *)arg;
    for(;;){
        switch(app->setupState){
            case STATE_INIT:
                ESP_LOGI("CONFIG_APP","Config app init");
                break;
            case STATE_ON_SENSOR:
                ESP_LOGI("CONFIG_APP","Config app on sensor");
                break;
            case STATE_ON_DCOM:
                ESP_LOGI("CONFIG_APP","Config app on dcom");
                break;
            default:
                break;
        }
    }
    
}
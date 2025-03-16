#include "measuring_app.h"
#include "app_config.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "tiny-json.h"
#include "led.h"
#include "rtc.h"
#include "http.h"
#include "adc_hw.h"
#include "lazer_sensor.h"
#include "dcom.h"
#include "app.h"
#include "wifi_app.h"
static const char* TAG = "MEASURING_APP";

static bool measuring_app_set_state(void * arg, MEASURING_APP_STATE_t state);
static bool post_data_measuring(void* arg);
static bool sync_time();

void measuring_app_init(void * arg){
    MeasuringApp * app = (MeasuringApp *)arg;
    app->state = MEASURING_APP_STATE_INIT;
    ESP_LOGI(TAG,"Measuring app init");
    xTaskCreate(measuring_app_process,"Measuring app process",1024*8,(void*)app,3,&app->measuring_task);
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
        // char * time_str = RTC_get_time_string(&rtc_time);
        // ESP_LOGI(TAG,"Time: %s",time_str);
        // free(time_str);
        switch (app->state)
        {
        case MEASURING_APP_STATE_INIT:
            DCOM_ON                                               
            lazer_sensor_init(&lazer_sensor); 
            set_led_state(&led_control,LED_ON,-1);
            ESP_LOGI(TAG,"Measuring app init");
            measuring_app_set_state(app, MEASURING_APP_STATE_MEASURING);
            break;
        case MEASURING_APP_STATE_MEASURING:
            ESP_LOGI("MEASURING_APP","Measuring app measuring");
            app->time_stamp_get_water_level = RTC_get_time_string(&rtc_time);
            int timeout = 10;
            while(!lazer_sensor_on(&lazer_sensor) && (timeout-- > 0)){
                ESP_LOGI(TAG,"Lazer sensor on Fail");
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            ESP_LOGI(TAG,"Lazer sensor on OK");
            vTaskDelay(pdMS_TO_TICKS(100));
            for(int i =0 ; i < TIMES_GET_WATER_LEVEL; i++){
                uint32_t distance = lazer_sensor_get_distance(&lazer_sensor);
                app->water_level[i] = (int)distance;
                ESP_LOGI(TAG,"Water level %d: %d",i,app->water_level[i]);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            measuring_app_set_state(app, MEASURING_APP_STATE_POSTING);
            break;
        case MEASURING_APP_STATE_POSTING:
            ESP_LOGI(TAG,"Measuring app posting");
            xEventGroupWaitBits(event_group,WF_CONNECTED_BIT,pdFALSE,pdFALSE,portMAX_DELAY);
            if(!post_data_measuring(app)){
                measuring_app_set_state(app, MEASURING_APP_STATE_ERROR);
                break;
            }
            measuring_app_set_state(app, MEASURING_APP_STATE_SET_TIME);
            break;
        case MEASURING_APP_STATE_SET_TIME:
            ESP_LOGI(TAG,"Measuring app set time");
            if(!sync_time()){
                measuring_app_set_state(app, MEASURING_APP_STATE_ERROR);
                break;
            }
            
            measuring_app_set_state(app, MEASURING_APP_STATE_DONE);
            break;
        case MEASURING_APP_STATE_DONE:
        {
            ESP_LOGI(TAG,"Measuring app done");
            DCOM_OFF
            wifi_app_deinit();
            free(app->time_stamp_get_water_level);
            app->time_stamp_get_water_level = NULL;
            esp_deep_sleep_start();
            break;
        }
        case MEASURING_APP_STATE_ERROR:
            ESP_LOGI(TAG,"Measuring app error");
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
        set_led_state(&led_control,LED_ON,-1);
        break;
    case MEASURING_APP_STATE_MEASURING:
        set_led_state(&led_control,LED_BLINK_3000MS,-1);
        break;
    case MEASURING_APP_STATE_ERROR:
    case MEASURING_APP_STATE_DONE:
        set_led_state(&led_control,LED_OFF,-1);
        break;

    default:
        break;
    }
    app->state = state;
    return true;
}
void uart_pattern_detected(void *arg, uint8_t *data, uint32_t len){
    ESP_LOGI(TAG,"Pattern detected");
    ESP_LOGI(TAG,"Length: %d",(int)len);
    for (int i = 0; i < len; i++)
    {
        ESP_LOGI(TAG,"Data: %c",data[i]);
    }

}
/**
 * @brief Post data to server
 * data format:
 * {
 *  "water_lever_0" : 100,
 *  "water_lever_1" : 100,
 *  "water_lever_2" : 100,
 *  "date_time" : "hour:min:sec day/month/year",;
 *  "serial_number" : "TD-016",
 * "type" : "water_lever",
 * "vol"    : 100
 * }
*/
bool post_data_measuring(void* arg ){
    if (arg == NULL)
    {
        ESP_LOGI(TAG,"Arg is NULL");
        return false;
    }
    MeasuringApp * app = (MeasuringApp *)arg;
    char str[200];
    snprintf(str,sizeof(str),
            "{\n"
            "\t\"water_lever_0\": %d,\n"
            "\t\"water_lever_1\": %d,\n"
            "\t\"water_lever_2\": %d,\n"
            "\t\"date_time\":   \"%s\",\n"
            "\t\"serial_number\": \"%s\",\n"
            "\t\"type\": \"water_lever\",\n"
            "\t\"vol\": %d\n"
            "}\n",
            app->water_level[0],
            app->water_level[1],
            app->water_level[2],
            app->time_stamp_get_water_level,
            SERIAL_NUMBER,
            adc_read_vol());
    ESP_LOGI(TAG,"Data post to server: %s",str);
    
    char response[HTTP_BUFF_SIZE] = {0};
    char *result = NULL;
    int size = 0;
    http_lock();
    result = http_request_post(API_POST_WATER,str,&size);
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
static bool sync_time(){
    ESP_LOGI(TAG,"Sync time");
    char response[HTTP_BUFF_SIZE] = {0};
    char *result = NULL;
    int size = 0;
    http_lock();
    result = http_request_get(API_UPDATE_TIME,&size);
    if(result == NULL || size == 0){
        ESP_LOGI(TAG,"Sync time failed");
        http_unlock();
        return false;
    }
    memcpy(response, result, size);
    http_unlock();
    //time struct: 10:34:20_16:03:2025
    struct tm _time_remote;
    sscanf(response,"%d:%d:%d_%d:%d:%d",&_time_remote.tm_hour,&_time_remote.tm_min,&_time_remote.tm_sec,&_time_remote.tm_mday,&_time_remote.tm_mon,&_time_remote.tm_year);
    if(rtc_time.minute == RTC_get_minute(&rtc_time)){
        ESP_LOGI(TAG, "No need update time");
        return true;
    } 
    int timeout =0;
    ESP_LOGI(TAG,"Sync time success");
    ESP_LOGI(TAG,"Response: %s",response);
    RTC_set_time_from_string(response);
    timeout =0;
    printf("RTC: Waiting for wake up pin set to HIGH");
    while (timeout++ < 10){ //timeout 10s
        printf(".");
        if(gpio_read(&button_gpio) == GPIO_PIN_RESET){
            if((RTC_reset_alarm(NULL) != ESP_OK)){
                ESP_LOGI(TAG,"Reset alarm Error");
            }
            else {
                ESP_LOGI(TAG, "Reset alarm done");
                timeout =0;
                while ((gpio_read(&button_gpio) == GPIO_PIN_RESET) && (timeout++ <10))
                {
                    ESP_LOGI(TAG, "Waiting for INT to HIGH");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
                break;
            }
        } 
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    return true;
}

MeasuringApp measuring_app = {
    .error = {
        {
            .error_string = "No error",
            .error_code = ERROR_CODE_NONE
        }
    },
    .time_stamp_get_water_level = NULL,
    .measuring_task = NULL,
    .water_level = {0,0,0},
    .state = MEASURING_APP_STATE_INIT
};

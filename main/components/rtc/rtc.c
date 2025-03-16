#include "rtc.h"
#include "esp_log.h"
#include "gpio_hw.h"
#include <string.h>
#include "config_app.h"
static const char* TAG = "RTC"; 
RTC_Alarm rtc_alarm;
RTC_DateTime rtc_time;

#if ENABLE_TEST_ALARM
static void RTC_task();
#endif

void RTC_init(void* arx){   
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    // p_rtc->rtc_task = NULL;
    pcf8563_reset_alarm(&pcf8563_dev); //clear alarm and set RTC to normal mode.
    RTC_set_alarm(ALARM_HOUR,ALARM_MINUTE);
#if ENABLE_TEST_ALARM
    //GPIO btn_pin = {.pin = BUTTON_GPIO_PIN, .mode = GPIO_INPUT,.pull_en = PULL_UP_EN};
    //GPIO wake_up_pin = {.pin = WAKEUP_PIN, .mode = GPIO_INPUT,.pull_en = PULL_UP_EN};
    //gpio_init(&btn_pin);
    //gpio_init(&wake_up_pin);
    ESP_LOGI(TAG,"RTC task created");
    xTaskCreate(RTC_task,"RTC task",1024*2,NULL,4,&p_rtc->rtc_task);

    // if(xTaskCreate(RTC_task, "RTC task", 512, NULL, 1, &p_rtc->rtc_task) != pdPASS) {
    //     ESP_LOGE(TAG, "Failed to create RTC task");
    // }
#endif // DEBUG
}
void RTC_deinit(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    vTaskDelete(p_rtc->rtc_task);
}

#if ENABLE_TEST_ALARM
static uint8_t time_test = 0;
static int time_increase = 0;
void RTC_task()
{
    ESP_LOGI(TAG,"RTC begin");
    //RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    while (1)
    {
        
        // if(gpio_get_level(BUTTON_GPIO_PIN) == 0){
        //     char result[20];
        //     memset(result,0,20);
        //     sprintf(result,"%d:9:50_01:03:2025",++time_test);
        //     RTC_set_time_from_string(result);
        //     ESP_LOGI(TAG,"has set time");
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        // }
        // if(gpio_get_level(WAKEUP_PIN) == 0){
        //     ESP_LOGI(TAG,"Wakeup pin triggered");
        //     vTaskDelay(pdMS_TO_TICKS(100));
        //     RTC_set_alarm(PCF8563_ALARM_DONT_CARE,10);
        // }
        // if(time_increase++ < 100){
        //     vTaskDelay(pdMS_TO_TICKS(10));
        //     continue;
        // }
        ESP_LOGI(TAG,"RTC task running");
        vTaskDelay(1000 / portTICK_PERIOD_MS); // delay 5s from led off
        // struct tm time;
        // esp_err_t err = pcf8563_get_time(&pcf8563_dev, &time);
        // if(err != ESP_OK){
        //     ESP_LOGE(TAG,"Error reading time from RTC");
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        //     continue;
        // }
        // rtc_time.year = time.tm_year;
        // rtc_time.month = time.tm_mon;
        // rtc_time.day = time.tm_mday;
        // rtc_time.hour = time.tm_hour;
        // rtc_time.minute = time.tm_min;
        // rtc_time.second = time.tm_sec;
        // ESP_LOGI(TAG,"Time: %d-%d-%d %d:%d:%d", rtc_time.year,
        //                                         rtc_time.month,
        //                                         rtc_time.day, 
        //                                         rtc_time.hour, 
        //                                         rtc_time.minute, 
        //                                         rtc_time.second);
        // vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif

int RTC_get_minute(void * arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    RTC_get_time(p_rtc);
    return p_rtc->minute;
}

char * RTC_get_time_string(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    if(RTC_get_time(p_rtc) != ESP_OK){
        return "";
    }
    char * timeStr = (char*)malloc(20);
    sprintf(timeStr,"%d-%d-%d %d:%d:%d", p_rtc->year, p_rtc->month, p_rtc->day, p_rtc->hour, p_rtc->minute, p_rtc->second);
    return timeStr;
}
esp_err_t RTC_get_time(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    struct tm time;
    esp_err_t err = pcf8563_get_time(&pcf8563_dev, &time);
    if(err != ESP_OK){
        ESP_LOGE(TAG,"Error reading time from RTC");
        return err;
    }
    p_rtc->year = time.tm_year;
    p_rtc->month = time.tm_mon;
    p_rtc->day = time.tm_mday;
    p_rtc->hour = time.tm_hour;
    p_rtc->minute = time.tm_min;
    p_rtc->second = time.tm_sec;
    return ESP_OK;
}
/**
 * @brief Set time for RTC from string
 * @param timeStr: string format "HH:MM:SS_DD:MM:YYYY"
 * @note: time string pattern: 10:56:09_01:03:2025
 */
static const int RTC_ELEMENTS_NUM = 6;
void RTC_set_time_from_string(const char* timeStr){
    struct tm time;
    int count = sscanf(timeStr,"%d:%d:%d_%d:%d:%d", &time.tm_hour, &time.tm_min, &time.tm_sec, &time.tm_mday, &time.tm_mon, &time.tm_year);
    if (count != RTC_ELEMENTS_NUM)
    {
        ESP_LOGE(TAG,"Error parsing time string");
        ESP_LOGE(TAG, "Time string: %s", timeStr);
        return;
    }
    if( pcf8563_set_time(&pcf8563_dev, &time) != ESP_OK){
        ESP_LOGE(TAG,"Error setting time");
    }
}

void RTC_set_alarm(uint8_t alarm_hour,uint8_t alarm_minute){
    struct tm time;
    time.tm_hour =  alarm_hour;
    time.tm_min =   alarm_minute;
    time.tm_mday = PCF8563_ALARM_DONT_CARE;
    time.tm_wday = PCF8563_ALARM_DONT_CARE;
#if ENABLE_TEST_ALARM
    pcf8563_reset_alarm(&pcf8563_dev);
#endif
    if(ESP_OK == pcf8563_set_alarm(&pcf8563_dev, &time)){
        ESP_LOGI(TAG,"Set alarm success");
    }
}

esp_err_t RTC_reset_alarm(void* arg){
    (void)arg;
    return pcf8563_reset_alarm(&pcf8563_dev);
}

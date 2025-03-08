#include "rtc.h"
#include "esp_log.h"
#include "gpio_hw.h"
#include <string.h>

static const char* TAG = "RTC"; 
RTC_Alarm rtc_alarm;
RTC_DateTime rtc_time;

#ifdef ENABLE_TEST_ALARM
static void RTC_task(void * arx);
#endif

void RTC_init(void* arx){   
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    p_rtc->rtc_task = NULL;
    RTC_set_alarm(PCF8563_ALARM_DONT_CARE,10);
#ifdef ENABLE_TEST_ALARM
    GPIO btn_pin = {.pin = BUTTON_GPIO_PIN, .mode = GPIO_INPUT,.pull_en = PULL_UP_EN};
    GPIO wake_up_pin = {.pin = WAKEUP_PIN, .mode = GPIO_INPUT,.pull_en = PULL_UP_EN};
    gpio_init(&btn_pin);
    gpio_init(&wake_up_pin);
    xTaskCreate(RTC_task,"RTC task",1024*2,(void*)p_rtc,3,&p_rtc->rtc_task);
#endif // DEBUG
}
void RTC_deinit(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    vTaskDelete(p_rtc->rtc_task);
}

#ifdef ENABLE_TEST_ALARM
static uint8_t time_test = 0;
static int time_increase = 0;
void RTC_task(void* arx)
{
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    while (1)
    {
        if(gpio_get_level(BUTTON_GPIO_PIN) == 0){
            char result[20];
            memset(result,0,20);
            sprintf(result,"%d:9:50_01:03:2025",++time_test);
            RTC_set_time_from_string(result);
            ESP_LOGI(TAG,"has set time");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        if(gpio_get_level(WAKEUP_PIN) == 0){
            ESP_LOGI(TAG,"Wakeup pin triggered");
            vTaskDelay(pdMS_TO_TICKS(100));
            RTC_set_alarm(PCF8563_ALARM_DONT_CARE,10);
        }
        if(time_increase++ < 100){
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }
        struct tm time;
        esp_err_t err = pcf8563_get_time(&pcf8563_dev, &time);
        if(err != ESP_OK){
            ESP_LOGE(TAG,"Error reading time from RTC");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        p_rtc->year = time.tm_year;
        p_rtc->month = time.tm_mon;
        p_rtc->day = time.tm_mday;
        p_rtc->hour = time.tm_hour;
        p_rtc->minute = time.tm_min;
        p_rtc->second = time.tm_sec;
        ESP_LOGI(TAG,"Time: %d-%d-%d %d:%d:%d", p_rtc->year, p_rtc->month, p_rtc->day, p_rtc->hour, p_rtc->minute, p_rtc->second);
        time_increase =0;
    }
}
#endif

char * RTC_get_time_string(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    char * timeStr = (char*)malloc(20);
    sprintf(timeStr,"%d-%d-%d %d:%d:%d", p_rtc->year, p_rtc->month, p_rtc->day, p_rtc->hour, p_rtc->minute, p_rtc->second);
    return timeStr;
}
RTC_DateTime RTC_get_time(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    return *p_rtc;
}
/**
 * @brief Set time for RTC from string
 * @param timeStr: string format "HH:MM:SS_DD:MM:YYYY"
 * @note: time string pattern: 10:56:09_01:03:2025
 */
static const int RTC_ELEMENTS_NUM = 6;
void RTC_set_time_from_string(const char* timeStr){
    struct tm time;
    int count = sscanf(timeStr,"%d:%d;%d_%d:%d:%d", &time.tm_hour, &time.tm_min, &time.tm_sec, &time.tm_mday, &time.tm_mon, &time.tm_year);
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
    pcf8563_reset_alarm(&pcf8563_dev);
    if(ESP_OK == pcf8563_set_alarm(&pcf8563_dev, &time)){
        ESP_LOGI(TAG,"Set alarm success");
    }
}
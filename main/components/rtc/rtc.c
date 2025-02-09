#include "rtc.h"
#include "esp_log.h"
static const char* TAG = "RTC"; 
RTC_Alarm rtc_alarm;
RTC_DateTime rtc_time;
static void rtc_task(void * arx);
void rtc_init(void* arx){   
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    p_rtc->rtc_task = NULL;

    xTaskCreate(rtc_task,"RTC task",1024*2,(void*)p_rtc,3,&p_rtc->rtc_task);

}
void rtc_deinit(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    vTaskDelete(p_rtc->rtc_task);
}
void rtc_task(void* arx)
{
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    while (1)
    {
        // Assuming you have a function to read time from the PCF library
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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

char * rtc_get_time_string(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    char * timeStr = (char*)malloc(20);
    sprintf(timeStr,"%d-%d-%d %d:%d:%d", p_rtc->year, p_rtc->month, p_rtc->day, p_rtc->hour, p_rtc->minute, p_rtc->second);
    return timeStr;
}
RTC_DateTime rtc_get_time(void* arx){
    RTC_DateTime * p_rtc = (RTC_DateTime*) arx;
    return *p_rtc;
}
void rtc_set_time_from_string(const char* timeStr){
    struct tm time;
    sscanf(timeStr,"%d-%d-%d %d:%d:%d", &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min, &time.tm_sec);
    time.tm_year -= 1900;
    time.tm_mon -= 1;
    pcf8563_set_time(&pcf8563_dev, &time);
}
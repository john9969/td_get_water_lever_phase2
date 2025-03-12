#ifndef RTC_H
#define RTC_H

#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h" 
#include "hw_pcf8563.h"    // RTC library

#define ENABLE_TEST_ALARM 1

// Structure to hold time and date information
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    TaskHandle_t rtc_task;
} RTC_DateTime;

// Structure to hold alarm information
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

} RTC_Alarm;
void RTC_init(void* arx);
void RTC_deinit(void* arx); 
void RTC_set_time_from_string(const char* timeStr);
char * RTC_get_time_string(void* arx);
RTC_DateTime RTC_get_time(void* arx);
void RTC_set_alarm(uint8_t alarm_hour,uint8_t alarm_minute);
extern RTC_Alarm rtc_alarm;
extern RTC_DateTime rtc_time;

#endif // RTC_H
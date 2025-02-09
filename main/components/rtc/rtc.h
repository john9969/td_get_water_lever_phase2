#ifndef RTC_H
#define RTC_H

#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h" 
#include "hw_pcf8563.h"    // RTC library
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
void rtc_init(void* arx);
void rtc_deinit(void* arx); 
void rtc_set_time_from_string(const char* timeStr);
char * rtc_get_time_string(void* arx);
RTC_DateTime rtc_get_time(void* arx);

extern RTC_Alarm rtc_alarm;
extern RTC_DateTime rtc_time;


#endif // RTC_H
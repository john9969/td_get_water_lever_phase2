#ifndef MEASURING_APP_H
#define MEASURING_APP_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "app_config.h"
#define TIMES_GET_WATER_LEVEL 3 // Number of times to get water level
typedef enum MEASURING_APP_STATE_t MEASURING_APP_STATE_t;
typedef enum ERROR_CODE_t ERROR_CODE_t;
typedef struct MeasuringApp MeasuringApp;
enum ERROR_CODE_t
{
    ERROR_CODE_NONE,
    ERROR_CODE_WIFI, 
    ERROR_CODE_SERVER,
    ERROR_CODE_SENSOR
};

typedef struct ERROR_t
{
    const char * error_string;
    ERROR_CODE_t error_code;
} ERROR_t;

enum MEASURING_APP_STATE_t
{
    MEASURING_APP_STATE_INIT,
    MEASURING_APP_STATE_MEASURING,
    MEASURING_APP_STATE_POSTING,
    MEASURING_APP_STATE_SET_TIME,
    MEASURING_APP_STATE_DONE,
    MEASURING_APP_STATE_WAITING,
    MEASURING_APP_STATE_ERROR
};
struct MeasuringApp
{
    ERROR_t error[MAX_NUM_ERROR];
    MEASURING_APP_STATE_t state;
    int water_level[TIMES_GET_WATER_LEVEL];
    char* time_stamp_get_water_level;
    TaskHandle_t measuring_task;
};

void measuring_app_init(void * arg);
void measuring_app_process(void* arg);
void measuring_app_deinit(void* arg);

extern MeasuringApp measuring_app;
#endif // MEASURING_APP_H
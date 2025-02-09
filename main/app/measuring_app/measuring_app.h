#ifndef MEASURING_APP_H
#define MEASURING_APP_H
#include "app_config.h"
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
};

void measuring_app_init(void * arg);
void measuring_app_process(void* arg);


extern MeasuringApp measuring_app;
#endif // MEASURING_APP_H
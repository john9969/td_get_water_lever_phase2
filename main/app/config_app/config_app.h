#ifndef CONFIG_APP_H
#define CONFIG_APP_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "app_config.h"
/**
 * @brief The configuration application state.
 * @note ENABLE server config
 *
 *
typdef enum {
    STATE_INIT =0,
    STATE_ON_SERVER,
    STATE_ON_SENSOR,
    STATE_ON_DCOM
} SETUP_STATE;
 * */


typedef enum {
    STATE_INIT =0,
    STATE_ON_SENSOR,
    STATE_ON_DCOM,
    STATE_WAITNG
} SETUP_STATE;
typedef struct ConfigApp ConfigApp;
struct ConfigApp{
    SETUP_STATE state;
    TaskHandle_t config_task;
};

void config_app_init(void * arg);
void config_app_deinit(void * arg);
extern ConfigApp configApp;

#endif // CONFIG_APP_H
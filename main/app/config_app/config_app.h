#ifndef CONFIG_APP_H
#define CONFIG_APP_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
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
    STATE_ON_DCOM
} SETUP_STATE;

typedef struct {
    SETUP_STATE setupState;
    vTaskHandle_t config_task;
} ConfigApp;

void config_app_init(void * arg);
void config_app_deinit(void * arg);
extern ConfigApp configApp;

#endif // CONFIG_APP_H
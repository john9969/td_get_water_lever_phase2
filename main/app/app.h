/*
 * app.h
 *
 *  Created on: 2024/11/27
 *      Author: tuanndm
 */

#ifndef APP_APP_H_
#define APP_APP_H_
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef enum APP_STATE_t APP_STATE_t;
enum APP_STATE_t
{
    APP_STATE_INIT,
    APP_STATE_RUN_SCRIPT_MEASURING,
    APP_STATE_RUN_CONFIG,
#if ENABLE_OTA    
    APP_STATE_OTA,
#endif
    APP_STATE_DONE,
    APP_STATE_ERROR
};

typedef struct Main_App Main_App;
struct Main_App{
    APP_STATE_t state;
};
void app_init(void);
#ifdef ENABLE_OTA
char* app_get_version();
#endif // ENABLE_OTA

//void app_process(void* arg);
char* device_reset_detected(int event);
void device_restart();
extern Main_App main_app;
extern EventGroupHandle_t event_group;

#endif /* MAIN_APP__H */

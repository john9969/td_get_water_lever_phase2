/*
 * app.h
 *
 *  Created on: 2024/11/27
 *      Author: tuanndm
 */

#ifndef APP_APP_H_
#define APP_APP_H_
typedef enum APP_STATE_t APP_STATE_t;
enum APP_STATE_t
{
    APP_STATE_INIT,
    APP_STATE_RUN_SCRIPT_MEASURING,
    APP_STATE_RUN_SCRIPT_CONFIG,
#if ENABLE_OTA    
    APP_STATE_OTA,
#endif
    APP_STATE_DONE,
    APP_STATE_ERROR
};
void app_init(void);
#ifdef ENABLE_OTA
char* app_get_version();
#endif // ENABLE_OTA

char* device_reset_detected(int event);
void device_restart();
#endif /* MAIN_APP_APP_CONFIG_H_ */

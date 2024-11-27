/*
 * app.h
 *
 *  Created on: Dec 15, 2022
 *      Author: MinhNhan
 */




#ifndef APP_APP_H_
#define APP_APP_H_

void app_init(void);
char* app_get_version();
char* device_reset_detected(int event);
void device_restart();
#endif /* MAIN_APP_APP_CONFIG_H_ */

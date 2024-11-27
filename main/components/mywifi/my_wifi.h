/*
 * wifi.h
 *
 *  Created on: Dec 15, 2022
 *      Author: Admin
 */

#ifndef COMPONENTS_WIFI_MY_WIFI_H_
#define COMPONENTS_WIFI_MY_WIFI_H_

#include "stdbool.h"

bool wifi_set_infor(char *ssid,char *pass);
void wifi_clear_info();

#endif /* COMPONENTS_WIFI_MY_WIFI_H_ */

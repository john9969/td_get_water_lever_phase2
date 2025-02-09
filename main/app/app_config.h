/*
 * app_config.h
 *
 *  Created on: 2024//11/27
 *      Author: tuanndm
 */

#ifndef MAIN_APP_APP_CONFIG_H_
#define MAIN_APP_APP_CONFIG_H_



#define MAX_BATTERY_CABINET_NUM										MAX_CABINET
#define STATION_CHARGER_NUM											(2)
#define SERIAL_NUMBER "TD-016"

#define API_UPDATE_TIME "https://"
#define API_POST_WATER "https://"
#if ENABLE_OTA
    #define API_OTA "https://"
#endif
#define MQTT_ENABLE 1
#define MQTT_TIME_PUBLIC 5

#define APP_STATE_MACHINE_TICK_INTERVAL_mS							(20UL)
#define APP_CO_PROCESS_TICK_INTERVAL_mS								(1UL)

#define APP_CO_SLAVE_START_NODE_ID									(5)

#define ENABLE_OTA_SV												(0)
#define ENABLE_MQTT_SV 												(0)
#define ENABLE_SNTP_SV 												(0)
#define ENABLE_HTTP_SERVER_SV 										(HTTP_SERVER_EN)
#define ENABLE_CLI													(0)
#define ENABLE_OTA                                                  (0)

#if ENABLE_OTA
    #define APP_VERSION 											(2.5)
#endif

#define WF_CONNECTED_BIT BIT0

#define USING_WIFI 0
#define USING_DEFAULT_WIFI_INFO 1
#define USING_CONFIG_MODE 0

#define MINIMUM_VOL_INPUT											(9.0)
#define CONNECT_BIT BIT1
#define MAX_NUM_ERROR                                                (10)   // Maximum number of errors that can be stored  
#endif /* MAIN_APP_APP_CONFIG_H_ */

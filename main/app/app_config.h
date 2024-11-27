/*
 * app_config.h
 *
 *  Created on: Nov 23, 2022
 *      Author: quangnd
 */

#ifndef MAIN_APP_APP_CONFIG_H_
#define MAIN_APP_APP_CONFIG_H_

#include "services.h"

#define APP_VERSION 												(2.5)

#define MAX_BATTERY_CABINET_NUM										MAX_CABINET
#define STATION_CHARGER_NUM											(2)
#define MBS_SERIAL_NUMBER "101MB1N000001"
#define CONFIG_BROKER_URL "mqtt://hub.selex.vn"//"mqtt://mqtt.eclipseprojects.io"
#define MQTT_ENABLE 1
#define MQTT_TIME_PUBLIC 5

#define APP_STATE_MACHINE_TICK_INTERVAL_mS							(20UL)
#define APP_CO_PROCESS_TICK_INTERVAL_mS								(1UL)

#define APP_CO_SLAVE_START_NODE_ID									(5)

#define ENABLE_OTA_SV												(0)
#define ENABLE_MQTT_SV 												(1)
#define ENABLE_SNTP_SV 												(1)
#define ENABLE_HTTP_SERVER_SV 										(HTTP_SERVER_EN)
#define ENABLE_CLI													(1)
#define BP_INACTIVE_TIME_THRESHOLD_mS								(100UL)
#define BP_INACTIVE_TIME_RETURN_mS									(1000UL)


#define BP_SOC_DELIVERY 											(90.0f)
#define BYPASS_WRITE_VEHICLE_SN_ENABLE 								(1)

#define USING_WIFI 0
#define USING_LTE 1


#define CONNECT_BIT  BIT0
#define DISCONNECT_BIT  BIT1
#define WF_CONNECTED_BIT  BIT2
#define WF_DISCONNECTED_BIT BIT3
#define ESPTOUCH_DONE_BIT BIT4
#define MQTT_EVENT_DISCONNECT BIT5

#endif /* MAIN_APP_APP_CONFIG_H_ */

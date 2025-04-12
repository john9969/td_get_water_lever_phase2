/*
 * app_config.h
 *
 *  Created on: 2024//11/27
 *      Author: tuanndm
 */

#ifndef MAIN_APP_APP_CONFIG_H_
#define MAIN_APP_APP_CONFIG_H_



#define SERIAL_NUMBER "TD_MW_0011"

#define API_UPDATE_TIME "http://donuoctrieuduong.xyz/dev_test/get_time.php"
#define API_POST_WATER "http://donuoctrieuduong.xyz/dev_test/water_lever.php"
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
    #define APP_VERSION 											(1.0)
#endif

#define WF_CONNECTED_BIT BIT0
#define UART_RESPONSE_BIT BIT1
#define USING_WIFI 1
#define USING_DEFAULT_WIFI_INFO 1
#define USING_CONFIG_MODE 0
#define HTTP_BUFF_SIZE (512)
#define MINIMUM_VOL_INPUT											(9.0)
#define CONNECT_BIT BIT1
#define BUTTON_STATE_BIT BIT2
#define MAX_NUM_ERROR                                                (10)   // Maximum number of errors that can be stored  
#define ALARM_HOUR (0x80)
#define ALARM_MINUTE (0)
#endif /* MAIN_APP_APP_CONFIG_H_ */

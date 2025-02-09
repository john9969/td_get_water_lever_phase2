/*
 * wifi_app.h
 *
 *  Created on: Feb 13, 2023
 *      Author: Admin
 *
 *  This header file contains the declarations for the WiFi application.
 *  The WiFi application includes functions for initializing and deinitializing
 *  the WiFi, as well as handling LTE states and network operations.
 *
 *  The WiFi application provides the following functionalities:
 *  - WiFi initialization and deinitialization
 *  - LTE state management and network operations
 *  - WiFi status retrieval
 *
 *  Constants:
 *  - MODE_AP_STA: Mode for Access Point and Station
 *  - WIFI_SSID_DEFAULT: Default WiFi SSID
 *  - WIFI_PASS_DEFAULT: Default WiFi password
 *  - WIFI_SETTING_SSID: Key for storing WiFi SSID in settings
 *  - WIFI_SETTING_PWD: Key for storing WiFi password in settings
 *  - WIFI_SSID: Default WiFi SSID
 *  - WIFI_PASS: Default WiFi password
 *
 *  Enumerations:
 *  - LTE_ST_LIVE: Enumeration for LTE states
 *      - LTE_ST_NETWORK: Network state
 *      - LTE_ST_COMMAND: Command state
 *      - LTE_ST_RUN: Run state
 *      - LTE_ST_RUN_COMMAND_MODE: Run command mode state
 *
 *  Functions:
 *  - void wifi_app_init(void): Initialize the WiFi application
 *  - void wifi_app_deinit(void): Deinitialize the WiFi application
 *  - void lte_app_init(): Initialize the LTE application
 *  - void lte_start_network(void): Start the LTE network
 *  - void lte_network_poll(void): Poll the LTE network
 *  - void lte_set_state(LTE_ST_LIVE st): Set the LTE state
 *  - LTE_ST_LIVE lte_get_st(): Get the current LTE state
 *  - void wifi_initialise(void): Initialize the WiFi hardware
 *  - wifi_event_t get_wifi_status(void): Get the current WiFi status
 */

#ifndef MAIN_APP_WIFI_APP_WIFI_APP_H_
#define MAIN_APP_WIFI_APP_WIFI_APP_H_


void wifi_app_init(void);
void wifi_app_deinit(void);



#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_smartconfig.h"
#include "esp_wpa2.h"
#include "esp_system.h"
#include "esp_netif.h"

#define MODE_AP_STA 0
#define WIFI_SSID_DEFAULT "SSIDDEFAULT"
#define WIFI_PASS_DEFAULT "PASSDEFAULT"
#define WIFI_SETTING_SSID "wifi_ssid"
#define WIFI_SETTING_PWD "wifi_pwd"
#define WIFI_SSID "SSIDDEFAULT"
#define WIFI_PASS "PASSDEFAULT"

typedef enum{
	LTE_ST_NETWORK = 10,
	LTE_ST_COMMAND = 20,
	LTE_ST_RUN = 30,
	LTE_ST_RUN_COMMAND_MODE = 40
}LTE_ST_LIVE;

void lte_app_init();
void lte_start_network(void);
void lte_network_poll(void);
void lte_set_state(LTE_ST_LIVE st);
LTE_ST_LIVE lte_get_st();

//void wifi_hw_scan(void);
void wifi_initialise(void);
wifi_event_t get_wifi_status(void);


#endif /* MAIN_APP_WIFI_APP_WIFI_APP_H_ */

/*
 * wifi.c
 *
 *  Created on: Dec 15, 2022
 *      Author: Admin
 */

/*
 * wifi_hw.c
 *
 *  Created on: Sep 22, 2022
 *      Author: Admin
 */
#include "my_wifi.h"
#include "string.h"
#include "hw_flash.h"
#include "gpio_hw.h"
#include "drv_timer.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "app_config.h"
#include "lte_app.h"
#include "wifi_app.h"
#include "stdbool.h"

static const char *TAG = "WIFI";

bool wifi_set_infor(char *ssid,char *pass){
	esp_err_t err = ESP_OK;
	err = flash_write_str(WIFI_SETTING_SSID, (char*) ssid);
	if (err != ESP_OK) {
		ESP_LOGI(TAG,"Write ssid false -err -%d\r\n", err);
		return false;
	}
	err = flash_write_str(WIFI_SETTING_PWD, (char*) pass);
	if (err != ESP_OK) {
		ESP_LOGI(TAG,"Write password false -err -%d\r\n", err);
		return false;
	}
	return true;
}
void wifi_clear_info(){
	flash_erase_key(WIFI_SETTING_SSID);
	flash_erase_key(WIFI_SETTING_PWD);
}

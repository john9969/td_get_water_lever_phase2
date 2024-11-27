/*
 * app.c
 *
 *  Created on: Dec 15, 2022
 *      Author: Admin
 */



#include <mbs/mbs.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <mqtt/mqtt.h>
#include "app.h"
#include "mqtt_data.h"
#include "app_config.h"
#include "cli_task.h"
#include "sntp_sv.h"
#include "mbs_app/mbs_app.h"
#include "ota_task.h"
//#include "http_server.h"
#include "storage.h"
#include "wifi_app/wifi_app.h"
#include "freertos/semphr.h"
#include "ota_bms.h"
#include "lte_app.h"
#include "esp_system.h"
#include "esp_log.h"
SemaphoreHandle_t internet_mutex;
extern EventGroupHandle_t event_group;

static const char *TAG = "App";

void app_init(void){

	esp_reset_reason_t rs_reason = esp_reset_reason();
	ESP_LOGI(TAG,"Device reset by %s",device_reset_detected(rs_reason));

	if(rs_reason == ESP_RST_PANIC){
		esp_restart();
	}

	event_group = xEventGroupCreate();
#if USING_LTE
	lte_app_init();
	lte_set_state(LTE_ST_NETWORK);
#endif

#if USING_WIFI
	wifi_app_init();
#endif
	internet_mutex = xSemaphoreCreateMutex();
	cli_task_init();
#if ENABLE_SNTP_SV
	sntp_sv_init();
#endif
#if ENABLE_HTTP_SERVER_SV
	http_server_init();
#endif

#if ENABLE_OTA_SV
	ota_task_init();
#endif
	mbs_app_init();

	storage_init();
	bms_ota_init();
}

#define MAX_LENGTH_VER 50
static char version[MAX_LENGTH_VER];

char* app_get_version(){
	memset(version,0,MAX_LENGTH_VER);
	sprintf(version,"ver%04d",(int)(APP_VERSION*10));
	return version;
}
char reset_reason[125];
char* device_reset_detected(int event){
	memset(reset_reason,0,125);
	switch (event) {
		case ESP_RST_UNKNOWN:
			sprintf(reset_reason,"ESP_RST_UNKNOWN");
			break;
		case ESP_RST_POWERON:
			sprintf(reset_reason,"ESP_RST_POWERON");
			break;
		case ESP_RST_EXT:
			sprintf(reset_reason,"ESP_RST_EXT");
			break;
		case ESP_RST_SW:
			sprintf(reset_reason,"ESP_RST_SW");
			break;
		case ESP_RST_PANIC:
			sprintf(reset_reason,"ESP_RST_PANIC");
			break;
		case ESP_RST_INT_WDT:
			sprintf(reset_reason,"ESP_RST_INT_WDT");
			break;
		case ESP_RST_TASK_WDT:
			sprintf(reset_reason,"ESP_RST_TASK_WDT");
			break;
		case ESP_RST_WDT:
			sprintf(reset_reason,"ESP_RST_WDT");
			break;
		case ESP_RST_DEEPSLEEP:
			sprintf(reset_reason,"ESP_RST_DEEPSLEEP");
			break;
		case ESP_RST_BROWNOUT:
			sprintf(reset_reason,"ESP_RST_BROWNOUT");
			break;
		case ESP_RST_SDIO:
			sprintf(reset_reason,"ESP_RST_SDIO");
			break;
		default:
			break;
	}
	return reset_reason;
}
void device_restart(){
	esp_restart();
}

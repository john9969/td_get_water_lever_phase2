/*
 * app.c
 *
 *  Created on: Dec 15, 2022
 *      Author: Admin
 */



#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "app.h"
#include "app_config.h"
#include "storage.h"
#include "wifi_app/wifi_app.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "measuring_app.h"
#include "config_app.h"
#include "led.h"
#include "button.h"

static bool has_config = false;
void button_callback(Button *p_button);

SemaphoreHandle_t internet_mutex;
extern EventGroupHandle_t event_group;

EventGroupHandle_t event_group;

static const char *TAG = "App";
void button_callback(void * arg);
void app_pprocess(void){
	//MeasuringApp * app = (MeasuringApp *)arg;
	//app->state = MEASURING_APP_STATE_INIT;
	ESP_LOGI(TAG,"App process");
}

void app_init(void){

	esp_reset_reason_t rs_reason = esp_reset_reason();
	ESP_LOGI(TAG,"Device reset by %s",device_reset_detected(rs_reason));

	if(rs_reason == ESP_RST_PANIC){
		esp_restart();
	}

	event_group = xEventGroupCreate();


#if USING_WIFI
	wifi_app_init();
#endif
	internet_mutex = xSemaphoreCreateMutex();
#if ENABLE_HTTP_SERVER_SV
	http_server_init();
#endif

#if ENABLE_OTA_SV
	ota_task_init();
#endif

	storage_init();
#if TEST_LED 
	test_led(&led_control);
#endif
}

#define MAX_LENGTH_VER 50
static char version[MAX_LENGTH_VER];
#if ENABLE_OTA
char* app_get_version(){
	memset(version,0,MAX_LENGTH_VER);
	sprintf(version,"ver%04d",(int)(APP_VERSION*10));
	return version;
}
#endif
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


void button_callback(void * arg){
	Button *p_button = (Button*)arg;
	if(p_button->mode == BT_PRESS){
		measuring_app_deinit(&measuring_app);
		config_app_inti(&configApp);
	}
}

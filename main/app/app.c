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
#include "esp_sleep.h"
#include "app.h"
#include "app_config.h"
#include "storage.h"
#include "wifi_app/wifi_app.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "measuring_app.h"
#include "dcom.h"
#include "app_config.h"
#include "led.h"
#include "button.h"
#include "lazer_sensor.h"
#include "rtc.h"
#include "config_app.h"
#include "http.h"
static bool has_config = false;

SemaphoreHandle_t internet_mutex;
extern EventGroupHandle_t event_group;\
static void app_state_change(void *arg,APP_STATE_t state);
static APP_STATE_t get_app_state(void *arg);
static void app_sleep();
static void app_wakeup_init();
static void test_http_request();
EventGroupHandle_t event_group;
static const char *TAG = "App";
#if 0
void app_process(void* arg){
	MeasuringApp * app = (MeasuringApp *)arg;
	for (;;)
	{
		ESP_LOGI(TAG,"APP_STATE_INIT");
		dcom_set_state(&dcom,STATE_ON);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
		measuring_app_init(&app);
		lazer_sensor_on(&lazer_sensor);

		//set State measuring
		EventBits_t bits = xEventGroupWaitBits(event_group,
			BUTTON_STATE_BIT,    // The bit to wait for
			pdTRUE,         // Clear the bit on exit
			pdFALSE,        // Wait for any bit
			portMAX_DELAY); // Wait forever
		measuring_app_deinit(&measuring_app);
		config_app_init(&configApp);

		if(!has_config)
		{
			ESP_LOGI(TAG,"APP_STATE_INIT: No config");
			app_state_change( &main_app, APP_STATE_RUN_CONFIG);
		}
		else
		{
			ESP_LOGI(TAG,"APP_STATE_INIT: Has config");
			app_state_change(&main_app, APP_STATE_RUN_SCRIPT_MEASURING);
		}
	}
	
}
#endif
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
#else 
	led_init(&led_control);
#endif

#if ENABLE_TEST_SENSOR
	lazer_sensor_init(&lazer_sensor);
#endif
#if ENABLE_TEST_ALARM
	RTC_init(&rtc_time);
#endif
	button_init(&wake_button_process);
	app_wakeup_init();
	measuring_app_init(&measuring_app);
	http_init();
	ESP_LOGI(TAG,"test_http_init");
	xTaskCreate(test_http_request,"test_http_request",1024*4,NULL,3,NULL);
	//xTaskCreate(http_post_test,"http_post_test",1024*2,(void*)&event_group,3,NULL);
	// http_post_test(&event_group);
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

static int button_old_state = (int)BT_RELEASE;
void button_wake_callback(void * arg){
	Button *p_button = (Button*)arg;	
	if((int)p_button->mode == button_old_state){
		return;
	}	
	
	/**
	 * switch state from Config to Measuring
	 * But Current Button state is holding
	 */
	if(button_get_state(p_button) == BT_HOLD && main_app.state == APP_STATE_RUN_CONFIG) {
		
		//Button hold in config mode->Off led
		set_led_state(&led_control,LED_OFF,-1);
		vTaskDelay(5000 / portTICK_PERIOD_MS); // delay 5s from led off
		app_state_change(&main_app, APP_STATE_INIT);
	}
	/**
	 * switch state from Config to MEASURING_APP_STATE_POSTING
	 * Current State is release, start switch state
	 */
	else if(button_get_state(p_button) == BT_PRESS && main_app.state == APP_STATE_RUN_CONFIG){
		
	}
	else if(button_get_state(p_button) == BT_HOLD && main_app.state != APP_STATE_RUN_CONFIG) {
		//Button hold in measuring mode -> set state to config
		app_state_change(&main_app ,APP_STATE_RUN_CONFIG);
	}
	else if(button_get_state(p_button) == BT_PRESS && main_app.state != APP_STATE_RUN_CONFIG){
		//Do nothing.
		
	}
	button_old_state = (int)p_button->mode;
	//xEventGroupSetBits(event_group, BUTTON_STATE_BIT);
}
void app_wakeup_init(){
	esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 0);
}
void app_sleep(){

}

void app_state_change(void *arg,APP_STATE_t state){
	Main_App * app = (Main_App *)arg;
	ESP_LOGI(TAG,"App_state:%d, new state:%d",app->state,state);
	if(app->state == state) return;
	if(app->state == APP_STATE_RUN_CONFIG && state == APP_STATE_INIT){
		device_restart();
	}
	if(app->state != APP_STATE_RUN_CONFIG && state == APP_STATE_RUN_CONFIG){
		measuring_app_deinit(&measuring_app);
		config_app_init(&configApp);
	}
	app->state = state;
	
}
APP_STATE_t get_app_state(void *arg){
	Main_App * app = (Main_App *)arg;
	return app->state;
}
Main_App main_app = {
	.state = APP_STATE_INIT,
};
void test_http_request(){
	//http_post_test();
    while (1)
	{
		vTaskDelay(5000 / portTICK_PERIOD_MS);
		ESP_LOGI(TAG,"HTTP Start POST TEST");
		http_post_test();
	}

}
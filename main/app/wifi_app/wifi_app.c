/*
 * wifi_app.c
 *
 *  Created on: Feb 13, 2023
 *      Author: Admin
 */


#include "wifi_app.h"
#include <my_wifi.h>

#include "esp_wifi_types.h"
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

extern EventGroupHandle_t event_group;
static bool s_wifi_enable = true;

QueueHandle_t wifi_queue_sta_handle = NULL;

typedef enum WIFI_MODE_t WIFI_MODE;

enum WIFI_MODE_t{
	WF_CONNECTED_AP = 1,
	WF_DISCONNECT_AP = 0,
	WF_SMART_CONFIG = 2,
};

#define WIFI_LOG printf
//#define COM_PORT UART_PORT_DEF
char DEFAULT_SSID[] = "Minh Tuan";
char DEFAULT_PWD[] = "j12345678";

static const char* TAG = "WIFI";

#if CONFIG_EXAMPLE_WIFI_ALL_CHANNEL_SCAN
#define DEFAULT_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#elif CONFIG_EXAMPLE_WIFI_FAST_SCAN
#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN
#else
#define DEFAULT_SCAN_METHOD WIFI_FAST_SCAN
#endif /*CONFIG_EXAMPLE_SCAN_METHOD*/

#if CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SIGNAL
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#elif CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SECURITY
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SECURITY
#else
#define DEFAULT_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#endif /*CONFIG_EXAMPLE_SORT_METHOD*/

#define DEFAULT_RSSI -127
#define DEFAULT_AUTHMODE WIFI_AUTH_OPEN


#if MODE_AP_STA
#define AP_SSID "selex_bss"
#define AP_PASS "smart_electric"
#define AP_CHANNEL 10
#endif
static wifi_event_t wifi_event = WIFI_EVENT_STA_DISCONNECTED;
static void dcom_state_control(void *arg);	
//static void wifi_set_info(uint8_t *ssid, uint8_t *pass);
//static const char *TAG = "WIFI";

void found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

void found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg){
    printf( "DNS found: %i.%i.%i.%i\n",
        ip4_addr1(&ipaddr->u_addr.ip4),
        ip4_addr2(&ipaddr->u_addr.ip4),
        ip4_addr3(&ipaddr->u_addr.ip4),
        ip4_addr4(&ipaddr->u_addr.ip4) );
}

uint16_t num_connect = 0;
uint16_t num_retry =0;
#define  MAX_NUM_CONNECT 10
#define MAX_NUM_RETRY 10
static void event_handler(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data);
static void event_handler_smartconfig(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data);

static void event_handler(void *arg, esp_event_base_t event_base,
							int32_t event_id, void *event_data) 
{
	WIFI_MODE wifi_sta_mod;
	ESP_LOGI(TAG,"event_handle, event_base: %s, event_id: %d\r\n", (char*)event_base, (int)event_id);
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		ESP_LOGI(TAG,"Wifi connecting...\r\n");
		wifi_sta_mod = WF_DISCONNECT_AP;
		xQueueSend(wifi_queue_sta_handle,&wifi_sta_mod,(TickType_t)portMAX_DELAY);
		//esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		wifi_event = WIFI_EVENT_STA_DISCONNECTED;
		num_connect++;
		if(num_connect == MAX_NUM_CONNECT){
			ESP_LOGI(TAG,"Wifi connect timeout, reset wifi...\r\n");
			esp_wifi_stop();
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			ESP_LOGI(TAG,"Wifi reconnect...\r\n");
			esp_wifi_start();
			num_connect = 0;
			num_retry++;
		}
		if(num_retry == MAX_NUM_RETRY){
			ESP_LOGI(TAG,"Wifi connect fail, reset wifi...\r\n");
			esp_wifi_disconnect();
			vTaskDelay(100 / portTICK_PERIOD_MS);
			esp_wifi_stop();
			vTaskDelay(100 / portTICK_PERIOD_MS);
			esp_restart();
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
		wifi_sta_mod = WF_DISCONNECT_AP;
		xQueueSend(wifi_queue_sta_handle,&wifi_sta_mod,(TickType_t)portMAX_DELAY);
		ESP_LOGI(TAG,"Wifi disconnect...%d\r\n", num_connect);
		
		esp_wifi_connect();
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		vTaskDelay(100 / portTICK_PERIOD_MS); // delay 5s from led off
		//dns_gethostbyname("https://www.google.com.vn",NULL, found_callback, NULL);
		wifi_event = WIFI_EVENT_STA_CONNECTED;
		wifi_sta_mod = WF_CONNECTED_AP;
		xQueueSend(wifi_queue_sta_handle,&wifi_sta_mod,(TickType_t)portMAX_DELAY);
		ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI(TAG,"got ip:" IPSTR "\r\n", IP2STR(&event->ip_info.ip));
		num_connect = 0;
		xEventGroupSetBits(event_group, CONNECT_BIT);
		xEventGroupSetBits(event_group,WF_CONNECTED_BIT);
	}
}



static void smartconfig_example_task(void *parm);
static void wifi_status_interface(void *arg);

void wifi_initialise(void) {
	ESP_LOGI(TAG, "Creating wifi_queue_sta_handle");
	
	wifi_queue_sta_handle = xQueueCreate(10, sizeof(WIFI_MODE));
	ESP_LOGI(TAG, "Creating wifi_status_interface task");
	xTaskCreate(wifi_status_interface, "wifi status", 1024, NULL, 6, NULL);
	ESP_ERROR_CHECK(esp_netif_init());

    // Initialize the event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

	ESP_LOGI(TAG, "Creating default wifi STA netif");
	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	if (sta_netif == NULL) {
		ESP_LOGE(TAG, "Failed to create default wifi STA netif");
		return;
	}
	ESP_LOGI(TAG, "Default wifi STA netif created successfully");
	ESP_LOGI(TAG, "Initializing wifi with default config");
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_LOGI(TAG, "Setting wifi storage to RAM");
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_LOGI(TAG, "Setting wifi mode to NULL");
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
	ESP_LOGI(TAG, "Reading wifi information");
	char ssid[30];
	char password[30];
	memset(ssid, 0, sizeof(ssid));
	memset(password, 0, sizeof(password));
#if USING_DEFAULT_WIFI_INFO
	ESP_LOGI(TAG, "Using default wifi info");
	int len_ssid = sizeof(DEFAULT_SSID);
	int len_pass = sizeof(DEFAULT_PWD);
	memcpy(ssid, DEFAULT_SSID, len_ssid);
	memcpy(password, DEFAULT_PWD, len_pass);
#else
	ESP_LOGI(TAG, "Reading wifi info from flash");
	int len_ssid = flash_read_str(WIFI_SETTING_SSID, ssid);
	int len_pass = flash_read_str(WIFI_SETTING_PWD, password);
#endif
	if (len_ssid > 0 && len_pass > 0) {
		ESP_LOGI(TAG, "SSID: %s", ssid);
		ESP_LOGI(TAG, "Password: %s", password);

		wifi_config_t wifi_config = {
			.sta = {
				
				.scan_method = DEFAULT_SCAN_METHOD,
				.sort_method = DEFAULT_SORT_METHOD,
				.threshold.rssi = DEFAULT_RSSI,
				.threshold.authmode = DEFAULT_AUTHMODE,
			},
		};
		ESP_LOGI(TAG, "Setting STA config");
		memset(wifi_config.sta.ssid, 0, sizeof(wifi_config.sta.ssid));
		memset(wifi_config.sta.password, 0, sizeof(wifi_config.sta.password));
		memcpy(wifi_config.sta.ssid, ssid, len_ssid);
		memcpy(wifi_config.sta.password, password, len_pass);
		ESP_LOGI(TAG, "Setting mode to STA");
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
		ESP_LOGI(TAG, "Registering event handlers");
		ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
				&event_handler, NULL, NULL));
		ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
				&event_handler, NULL, NULL));

		ESP_LOGI(TAG, "Starting wifi");
		ESP_ERROR_CHECK(esp_wifi_start());
		ESP_LOGI(TAG, "Connecting to wifi");
		ESP_ERROR_CHECK(esp_wifi_connect());
	} else {
		ESP_LOGI(TAG, "Wifi info not found, starting smartconfig");
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
				&event_handler_smartconfig, NULL));
		ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
				&event_handler_smartconfig, NULL));
		ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID,
				&event_handler_smartconfig, NULL));
		WIFI_MODE wifi_sta_mod = WF_SMART_CONFIG;
		event_group = xEventGroupCreate();
		xQueueSend(wifi_queue_sta_handle, &wifi_sta_mod, (TickType_t)portMAX_DELAY);
		ESP_ERROR_CHECK(esp_wifi_start());
	}
	s_wifi_enable = false;
}

TaskHandle_t smart_config_handle;

static void event_handler_smartconfig(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data) {
	WIFI_MODE wifi_sta_mod;
	//event_handler(arg, event_base, event_id, event_data);
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		xTaskCreate(smartconfig_example_task, "smartconfig_example_task\r\n",
				4096 * 2, NULL, 3, &smart_config_handle);
	} else if (event_base == WIFI_EVENT
			&& event_id == WIFI_EVENT_STA_DISCONNECTED) {
		wifi_event = WIFI_EVENT_STA_DISCONNECTED;
		esp_wifi_connect();
			xEventGroupClearBits(event_group, WF_CONNECTED_BIT);
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		wifi_sta_mod = WF_CONNECTED_AP;
		xQueueSend(wifi_queue_sta_handle,&wifi_sta_mod,(TickType_t)portMAX_DELAY);
		ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI(TAG,"got ip:" IPSTR "\r\n", IP2STR(&event->ip_info.ip));
		xEventGroupSetBits(event_group, CONNECT_BIT);
		if(s_wifi_enable == true){
			xEventGroupSetBits(event_group, WF_CONNECTED_BIT);
		}
	} else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
		ESP_LOGI(TAG,"Scan done\r\n");
	} else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
		ESP_LOGI(TAG,"Found channel\r\n");
	} else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
		ESP_LOGI(TAG,"Got SSID and password\r\n");

		smartconfig_event_got_ssid_pswd_t *evt =
				(smartconfig_event_got_ssid_pswd_t*) event_data;
		wifi_config_t wifi_config;
		uint8_t ssid[33] = { 0 };
		uint8_t password[65] = { 0 };
		uint8_t rvd_data[33] = { 0 };

		bzero(&wifi_config, sizeof(wifi_config_t));
		memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
		memcpy(wifi_config.sta.password, evt->password,
				sizeof(wifi_config.sta.password));
		wifi_config.sta.bssid_set = evt->bssid_set;
		if (wifi_config.sta.bssid_set == true) {
			memcpy(wifi_config.sta.bssid, evt->bssid,
					sizeof(wifi_config.sta.bssid));
		}

		memcpy(ssid, evt->ssid, sizeof(evt->ssid));
		memcpy(password, evt->password, sizeof(evt->password));
		ESP_LOGI(TAG,"SSID:%s \r\n", ssid);
		ESP_LOGI(TAG,"PASSWORD:%s \r\n", password);
		esp_err_t err = ESP_OK;
		err = flash_write_str(WIFI_SETTING_SSID, (char*) ssid);
		if (err != ESP_OK) {
			ESP_LOGI(TAG,"Write ssid false -err -%d\r\n", err);
		}
		err = flash_write_str(WIFI_SETTING_PWD, (char*) password);
		if (err != ESP_OK) {
			ESP_LOGI(TAG,"Write password false -err -%d\r\n", err);
		}
		ESP_LOGI(TAG,"Save wifi info\r\n");
		if (evt->type == SC_TYPE_ESPTOUCH_V2) {
			ESP_ERROR_CHECK(
					esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
			ESP_LOGI(TAG,"RVD_DATA:");
			for (int i = 0; i < 33; i++) {
				printf("%02x ", rvd_data[i]);
			}
			printf("\r\n");
		}

		ESP_ERROR_CHECK(esp_wifi_disconnect());
		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
		esp_wifi_connect();
	} else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
		//xEventGroupSetBits(event_group, ESPTOUCH_DONE_BIT);
	}
}


static void smartconfig_example_task(void *parm) {

	EventBits_t uxBits;
	ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
	smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

	while (1) {
		uxBits = xEventGroupWaitBits(event_group,
				WF_CONNECTED_BIT, false,false, portMAX_DELAY);
		if (uxBits & WF_CONNECTED_BIT) {
			ESP_LOGI(TAG,"WiFi Connected to ap\r\n");
			wifi_event = WIFI_EVENT_STA_CONNECTED;
		}
	
	}
	//configend: vTaskDelete(smart_config_handle);
	return;
}

wifi_event_t get_wifi_status(void) {
	wifi_event_t result = wifi_event;
	return result;
}



#define TIME_WIFI_ISACTIVE_mS 1
uint32_t time_isactive = 0;
static void wifi_status_interface(void *arg) {
	WIFI_MODE wf_mode = WF_CONNECTED_AP;
	WIFI_MODE wf_state = WF_DISCONNECT_AP;
	while (1) {
		if(xQueueReceive(wifi_queue_sta_handle, &wf_mode,(TickType_t) 1) == pdPASS){
			if(wf_state != wf_mode){
				wf_state = wf_mode;
				time_isactive = 0;
			}
		}
		switch (wf_state) {
			case WF_CONNECTED_AP:
				//gpio_pin_write(&led_wifi, GPIO_PIN_SET);
//				if(time_isactive > 60000){
//					esp_err_t err = dns_gethostbyname("google.com",NULL, found_callback, NULL);
//					if(err != ESP_OK){
//						wf_state = WF_DISCONNECT_AP;
//					}
//					time_isactive = 0;
//				}
				break;
			case WF_DISCONNECT_AP:
				if((time_isactive > 0) && (time_isactive <1000)){
					//gpio_pin_write(&led_wifi, GPIO_PIN_SET);
				}
				else if(time_isactive >=1000 && time_isactive <2000){
					// gpio_pin_write(&led_wifi, GPIO_PIN_RESET);
				}
				else if(time_isactive > 2000){
					time_isactive = 0;
				}
				break;
			case WF_SMART_CONFIG:
				if((time_isactive > 0) && (time_isactive <100)){
					// gpio_pin_write(&led_wifi, GPIO_PIN_SET);
				}
				else if((time_isactive >=100) && (time_isactive <200)){
					// gpio_pin_write(&led_wifi, GPIO_PIN_RESET);
				}
				else if(time_isactive > 200){
					time_isactive = 0;
				}
				break;
			default:
				break;
		}
		time_isactive += TIME_WIFI_ISACTIVE_mS;
		vTaskDelay(pdMS_TO_TICKS(TIME_WIFI_ISACTIVE_mS));
	}
}

void wifi_app_init(void){
	wifi_initialise();
}
void wifi_app_deinit(void){
	esp_wifi_disconnect();
	esp_wifi_stop();
	esp_wifi_deinit();
}

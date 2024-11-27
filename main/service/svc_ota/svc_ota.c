/* OTA example

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "cJSON.h"
#include "driver/gpio.h"

#include "common.h"
#include "svc_ota.h"
#include "mqtt.h"
#include "mbs_app/mbs_app.h"

#define OTA_DBG_ENA	//add to pre-compiler

#ifdef OTA_DBG_ENA
static const char *TAG = "OTA_DBG";
#define OTA_DBG				ESP_LOGI
#define OTA_PRINT			printf
#else
	#define OTA_DBG(...)
	#define OTA_PRINT(...)
#endif

// server certificates
//extern const char server_cert_pem_start[] asm("_binary_certs_pem_start");
//extern const char server_cert_pem_end[] asm("_binary_certs_pem_end");
/*const char *server_cert_pem_start =
 "-----BEGIN CERTIFICATE-----\n"
 "MIIEFzCCAv+gAwIBAgIQB/LzXIeod6967+lHmTUlvTANBgkqhkiG9w0BAQwFADBh\n"
 "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
 "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
 "QTAeFw0yMTA0MTQwMDAwMDBaFw0zMTA0MTMyMzU5NTlaMFYxCzAJBgNVBAYTAlVT\n"
 "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMDAuBgNVBAMTJ0RpZ2lDZXJ0IFRMUyBI\n"
 "eWJyaWQgRUNDIFNIQTM4NCAyMDIwIENBMTB2MBAGByqGSM49AgEGBSuBBAAiA2IA\n"
 "BMEbxppbmNmkKaDp1AS12+umsmxVwP/tmMZJLwYnUcu/cMEFesOxnYeJuq20ExfJ\n"
 "qLSDyLiQ0cx0NTY8g3KwtdD3ImnI8YDEe0CPz2iHJlw5ifFNkU3aiYvkA8ND5b8v\n"
 "c6OCAYIwggF+MBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFAq8CCkXjKU5\n"
 "bXoOzjPHLrPt+8N6MB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA4G\n"
 "A1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwdgYI\n"
 "KwYBBQUHAQEEajBoMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5j\n"
 "b20wQAYIKwYBBQUHMAKGNGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdp\n"
 "Q2VydEdsb2JhbFJvb3RDQS5jcnQwQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2Ny\n"
 "bDMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNybDA9BgNVHSAE\n"
 "NjA0MAsGCWCGSAGG/WwCATAHBgVngQwBATAIBgZngQwBAgEwCAYGZ4EMAQICMAgG\n"
 "BmeBDAECAzANBgkqhkiG9w0BAQwFAAOCAQEAR1mBf9QbH7Bx9phdGLqYR5iwfnYr\n"
 "6v8ai6wms0KNMeZK6BnQ79oU59cUkqGS8qcuLa/7Hfb7U7CKP/zYFgrpsC62pQsY\n"
 "kDUmotr2qLcy/JUjS8ZFucTP5Hzu5sn4kL1y45nDHQsFfGqXbbKrAjbYwrwsAZI/\n"
 "BKOLdRHHuSm8EdCGupK8JvllyDfNJvaGEwwEqonleLHBTnm8dqMLUeTF0J5q/hos\n"
 "Vq4GNiejcxwIfZMy0MJEGdqN9A57HSgDKwmKdsp33Id6rHtSJlWncg+d0ohP/rEh\n"
 "xRqhqjn1VtvChMQ1H3Dau0bwhr9kAMQ+959GG50jBbl9s08PqUU643QwmA==\n"
 "-----END CERTIFICATE-----\n";*/
//"-----BEGIN CERTIFICATE-----\n"
//"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
//"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
//"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
//"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
//"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
//"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
//"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
//"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
//"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
//"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
//"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
//"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
//"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
//"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
//"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
//"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
//"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
//"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
//"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
//"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
//"+OkuE6N36B9K\n"
//"-----END CERTIFICATE-----\n";
// receive buffer

#define RCV_BUFF_SIZE 200
static char rcv_buffer[RCV_BUFF_SIZE];

//=======================prototype==============================//

//static void blink_task(void *pvParameter);

//======================end prototype =========================//

// esp_http_client event handler
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {

	switch (evt->event_id) {
	case HTTP_EVENT_ERROR:
		break;
	case HTTP_EVENT_ON_CONNECTED:
		break;
	case HTTP_EVENT_HEADER_SENT:
		break;
	case HTTP_EVENT_ON_HEADER:
		break;
	case HTTP_EVENT_ON_DATA:
		break;
	case HTTP_EVENT_ON_FINISH:
		break;
	case HTTP_EVENT_DISCONNECTED:
		break;
	default:
		break;
	}
	return ESP_OK;
}

static esp_err_t _http_client_init_cb(esp_http_client_handle_t http_client) {
	esp_err_t err = ESP_OK;
	/* Uncomment to add custom headers to HTTP request */
	// err = esp_http_client_set_header(http_client, "Custom-Header", "Value");
	return err;
}
static void ota_update_task(void *arg);
// Check update task
// downloads every 30sec the json file with the latest firmware
static void ota_check_update_task(void *pvParameter) {

	ota_config_t *ota_cfg = (ota_config_t*) pvParameter;
	int content_length = 0;
	while (1) {

		OTA_DBG(TAG, "Looking for a new firmware...\n");

		// configure the esp_http_client
		esp_http_client_config_t config = { .url = ota_cfg->url_src,
				.event_handler = _http_event_handler, .timeout_ms = 5000};
		esp_http_client_handle_t client = esp_http_client_init(&config);

		// downloading the json file
		//esp_err_t err = esp_http_client_perform(client);
		esp_err_t err = esp_http_client_open(client, 0);
		if (err == ESP_OK) {

			content_length = esp_http_client_fetch_headers(client);

			// parse the json file
			if (content_length < 0) {
				ESP_LOGE(TAG, "HTTP client fetch headers failed");
			}
			else {
				memset(rcv_buffer,0,RCV_BUFF_SIZE);
				esp_http_client_read_response(client, rcv_buffer,RCV_BUFF_SIZE);
				OTA_DBG(TAG, "data = %s\r\n", rcv_buffer);
				cJSON *json = cJSON_Parse(rcv_buffer);
				if (json == NULL)
					OTA_DBG(TAG, "downloaded file is not a valid json, aborting...\n");
				else {
					cJSON *version = cJSON_GetObjectItemCaseSensitive(json,
							"version");
					cJSON *file = cJSON_GetObjectItemCaseSensitive(json,
							"file");
					// check the version
					if (!cJSON_IsNumber(version))
						OTA_DBG(TAG, "unable to read new version, aborting...\n");
					else {
						uint16_t new_version = (uint16_t) (version->valuedouble
								* 10);
						if (new_version
								> (uint16_t) (ota_cfg->fw_version * 10)) {
							OTA_DBG(TAG, "current firmware version (%.1f) is lower than the available one (%.1f), upgrading...\n",
									ota_cfg->fw_version, (float)new_version/10.0);
							if (cJSON_IsString(file)
									&& (file->valuestring != NULL)) {
								OTA_PRINT(
										"downloading and installing new firmware (%s)...\n",
										cJSON_GetStringValue(file));

								esp_http_client_config_t ota_client_config = {
										.url = file->valuestring, .timeout_ms =
												20000, .keep_alive_enable =
										true, };

								esp_https_ota_config_t https_ota_cfg = {
										.http_config = &ota_client_config,
										.http_client_init_cb =
												_http_client_init_cb };

								esp_err_t ret = esp_https_ota(&https_ota_cfg);
								if (ret == ESP_OK) {
									OTA_PRINT("OTA OK, restarting...\n");
									esp_restart();
								} else {
									OTA_DBG(TAG, "OTA failed...(%s)\n",esp_err_to_name(ret));
								}
							} else
								OTA_DBG(TAG, "unable to read the new file name, aborting...\n");
						} else
							OTA_DBG(TAG, "current firmware version (%.1f) is greater or equal to the available one (%.1f), nothing to do...\n", ota_cfg->fw_version, (float)new_version/10.0);
					}
				}
				cJSON_Delete(json);
			}
		} else
			OTA_DBG(TAG, "unable to download the json file, aborting...\n");

		// cleanup
		esp_http_client_cleanup(client);

		OTA_PRINT("\n");
		vTaskDelay(pdMS_TO_TICKS(1000*60*60));
	}
}

void svc_ota_start(ota_config_t *ota_cfg) {

	OTA_PRINT("HTTPS OTA, firmware %.1f\n\n", ota_cfg->fw_version);

	// start the check update task
	xTaskCreate(&ota_check_update_task, "check_update_task", 1024*4,
			(void* const ) ota_cfg, 2 , NULL);
}

void svc_ota_init(ota_config_t *const me, char *url, float fw_version) {
	strcpy(me->url_src, url);
	me->fw_version = fw_version;
}

char* msg_id = NULL;
char* version = NULL;

void svc_ota_update(const char *url,char* id,char *ver){
	ota_config_t mqtt_ota_config;

	memset(mqtt_ota_config.url_src,0,LEN_URL);
	strcpy(mqtt_ota_config.url_src,url);
	msg_id = id;
	version = ver;

	ESP_LOGI(TAG,"url %s",mqtt_ota_config.url_src);
	ESP_LOGI(TAG,"msg_id %s",msg_id);
	ESP_LOGI(TAG,"version %s",version);

	xTaskCreate(ota_update_task,"ota_update",1024*4,(void*)&mqtt_ota_config,configMAX_PRIORITIES,NULL);
}


static void ota_update_task(void *arg){
	ota_config_t *p_ota_config = (ota_config_t*)arg;
	char* url = malloc(LEN_URL+1);

	memset(url,0,LEN_URL+1);
	strcpy(url,p_ota_config->url_src);

	ESP_LOGI(TAG,"url %s",p_ota_config->url_src);
	if(msg_id != NULL)
		ESP_LOGI(TAG,"msg_id %s",msg_id);
	if(version != NULL)
		ESP_LOGI(TAG,"version %s",version);

	esp_http_client_config_t ota_update_client_config = {
			.url = url, .timeout_ms =
					20000, .keep_alive_enable =
			true, };

	OTA_PRINT(
			"downloading and installing new firmware (%s)...\n",
			url);

	esp_https_ota_config_t https_ota_update_cfg = {
			.http_config = &ota_update_client_config,
			.http_client_init_cb =
					NULL};
	esp_err_t ret = esp_https_ota(&https_ota_update_cfg);

	cJSON *json = cJSON_CreateObject();
	cJSON *data = cJSON_CreateObject();
	if(msg_id != NULL)
		cJSON_AddStringToObject(json, "msg_id", msg_id);
	cJSON_AddStringToObject(json, "type", "UPGRADING_STATUS");
	if(version != NULL)
		cJSON_AddStringToObject(data, "version", version);

	if (ret == ESP_OK) {
		cJSON_AddBoolToObject(data, "status",true);
		cJSON_AddNumberToObject(data, "error",0);
		cJSON_AddItemToObject(json, "data",data);
		mqtt_public(selex_mbs.pub_upgrading_status,cJSON_Print(json), strlen(cJSON_Print(json)), 1, 0);
		vTaskDelay(pdMS_TO_TICKS(5000));
		cJSON_Delete(json);
		OTA_PRINT("OTA OK, restarting...\n");
		esp_restart();
	} else {
		cJSON_AddBoolToObject(data, "status",false);
		cJSON_AddNumberToObject(data, "error",1);
		cJSON_AddItemToObject(json, "data",data);
		mqtt_public(selex_mbs.pub_upgrading_status,cJSON_Print(json), strlen(cJSON_Print(json)), 1, 0);
		vTaskDelay(pdMS_TO_TICKS(5000));
		cJSON_Delete(json);
		OTA_DBG(TAG, "OTA failed...(%s)\n",esp_err_to_name(ret));
	}

	mbs_set_state(&selex_mbs,MBS_ST_NORMAL);

	free(url);
	vTaskDelete(NULL);
}

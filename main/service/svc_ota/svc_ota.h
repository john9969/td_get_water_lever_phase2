/*
 * svc-ota.h
 *
 *  Created on: Dec 15, 2021
 *      Author: Admin
 */

#ifndef SERVICES_SVC_OTA_SVC_OTA_H_
#define SERVICES_SVC_OTA_SVC_OTA_H_

#define LEN_URL				(512)
#define LEN_MSG_ID			(64)
#define LEN_VER				(64)
#define LEN_STA_SSID		(32)
#define LEN_STA_PWD			(64)

typedef struct
{
	char ssid[LEN_STA_SSID + 1];
	char pass[LEN_STA_PWD + 1];
}network_t;

typedef struct
{
	char url_src[LEN_URL + 1];
	float fw_version;	//%.1f
	network_t wifi_sta;
	char *msg_id;
	char *version;
}ota_config_t;

void svc_ota_start(ota_config_t* ota_cfg);
void svc_ota_update(const char* url,char* id,char *ver);
void svc_ota_init(ota_config_t* const me, char* url, float fw_version);



#endif /* SERVICES_SVC_OTA_SVC_OTA_H_ */

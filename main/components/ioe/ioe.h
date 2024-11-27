/*
 * svc_ledhmi.h
 *
 *  Created on: Dec 24, 2022
 *      Author: Admin
 */

#ifndef MAIN_SERVICE_SVC_LED_HMI_SVC_LEDHMI_H_
#define MAIN_SERVICE_SVC_LED_HMI_SVC_LEDHMI_H_

#include "stdint.h"
#include <stddef.h>
#include <ioe_hw.h>
#include <74hc595.h>
#include "stdbool.h"

typedef enum{
	HMI_ST_OFF = 0,
	HMI_ST_EMPTY = 1,
	HMI_ST_ASSIGNING = 2,
	HMI_ST_LOST_CONNECT = 3,
	HMI_ST_STANDBY = 4,
	HMI_ST_CHARGING = 5,
	HMI_ST_FULL = 6,
	HMI_ST_DELIVERY = 7
}HMI_LED_Status;

typedef struct IOExtern_t Led_Hmi;

struct IOExtern_t{
#if V1_0
	IOExtern_Hw *p_io;
	uint8_t addr;
#else
	_74HC595 *p_io;
	uint8_t bit1;
	uint8_t bit2;
	uint16_t data;
	uint32_t time_poll_ms;
	int led;
	bool led_state;
#endif
	HMI_LED_Status status;
	void (*led_set)(Led_Hmi *p_led,HMI_LED_Status _status);
};

void led_hmi_init(Led_Hmi *p_led);
void led_hmi_begin(Led_Hmi *p_led);
static inline void led_hmi_set_status(Led_Hmi *p_led,HMI_LED_Status _status){
	if(p_led->led_set != NULL)
		p_led->led_set(p_led,_status);
}

#endif /* MAIN_SERVICE_SVC_LED_HMI_SVC_LEDHMI_H_ */

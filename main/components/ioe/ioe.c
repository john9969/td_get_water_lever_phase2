/*
 * svc_ledhmi.c
 *
 *  Created on: Dec 24, 2022
 *      Author: Admin
 */

#include "huart.h"
#include "ioe.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define COLOR_BLACK 0
#define COLOR_RED 1
#define COLOR_BLUE 2
#define COLOR_YELLOW 3

static const char *TAG = "LEDIO";

SemaphoreHandle_t led_mutex;

static void p_led_hmi_set(Led_Hmi *p_led,HMI_LED_Status _status);
static void led_hmi_control(void* arg);
static void led_hmi_write(Led_Hmi *p_led,int color);
static uint16_t led_set_bit_low(uint16_t* data,uint8_t bit);
static uint16_t led_set_bit_high(uint16_t* data,uint8_t bit);
static void led_set_blink(Led_Hmi *p_led,int color,uint32_t time_driff_ms);

void led_hmi_begin(Led_Hmi *p_led){
	p_led->p_io = &led_io;
	p_led->status = HMI_ST_EMPTY;
	p_led->led_set = p_led_hmi_set;
	p_led->led_set(p_led,p_led->status);
	xTaskCreate(led_hmi_control,"led control",1024,(void*)p_led,5,NULL);
}

void led_hmi_init(Led_Hmi *p_led){
	led_mutex = xSemaphoreCreateMutex();
	if(led_mutex == NULL){
		ESP_LOGE(TAG,"Error creat mutex");
	}
}

static void p_led_hmi_set(Led_Hmi *p_led,HMI_LED_Status _status){

	p_led->status = _status;
	p_led->led_state = true;
	p_led->time_poll_ms = 0;

}

#define TIME_LED_Process_Ms 1
#define NUM_LED 8
static void led_hmi_control(void* arg){
	Led_Hmi *p_led = (Led_Hmi*) arg;
	while(1){
		switch (p_led->status) {
			case HMI_ST_OFF:
				led_hmi_write(p_led,COLOR_BLACK);
				break;
			case HMI_ST_EMPTY:
				led_hmi_write(p_led,COLOR_RED);
				break;
			case HMI_ST_ASSIGNING:
				led_set_blink(p_led,COLOR_BLUE,100);
				break;
			case HMI_ST_LOST_CONNECT:
				led_set_blink(p_led,COLOR_RED,100);
				break;
			case HMI_ST_STANDBY:
				led_hmi_write(p_led,COLOR_BLUE);
				break;
			case HMI_ST_CHARGING:
				led_set_blink(p_led,COLOR_BLUE,500);
				break;
			case HMI_ST_FULL:
				led_hmi_write(p_led,COLOR_YELLOW);
				break;
			case HMI_ST_DELIVERY:
				led_set_blink(p_led,COLOR_YELLOW,500);
				break;
			default:
				break;
		}
		vTaskDelay(pdMS_TO_TICKS(TIME_LED_Process_Ms));
	}
}
static void led_hmi_write(Led_Hmi *p_led,int color){
	xSemaphoreTake(led_mutex,portMAX_DELAY);
	switch (color) {
		case COLOR_BLACK:
			p_led->p_io->data16 = led_set_bit_low((uint16_t*)&p_led->p_io->data16,p_led->bit1);
			p_led->p_io->data16  = led_set_bit_low((uint16_t*)&p_led->p_io->data16,p_led->bit2);
			hc595_send16(p_led->p_io,p_led->p_io->data16);
			break;
		case COLOR_RED:
			p_led->p_io->data16  = led_set_bit_high((uint16_t*)&p_led->p_io->data16,p_led->bit1);
			p_led->p_io->data16  = led_set_bit_low((uint16_t*)&p_led->p_io->data16,p_led->bit2);
			//ESP_LOGI(TAG,"Led %d Write: %04XH" ,p_led->led,p_led->p_io->data16);
			hc595_send16(p_led->p_io,p_led->p_io->data16);
			break;
		case COLOR_BLUE:
			p_led->p_io->data16  = led_set_bit_low((uint16_t*)&p_led->p_io->data16,p_led->bit1);
			p_led->p_io->data16  = led_set_bit_high((uint16_t*)&p_led->p_io->data16,p_led->bit2);
			hc595_send16(p_led->p_io,p_led->p_io->data16);
			break;
		case COLOR_YELLOW:
			p_led->p_io->data16  = led_set_bit_high((uint16_t*)&p_led->p_io->data16,p_led->bit1);
			p_led->p_io->data16 = led_set_bit_high((uint16_t*)&p_led->p_io->data16,p_led->bit2);
			hc595_send16(p_led->p_io,p_led->p_io->data16);
			break;
		default:
			break;
	}
	xSemaphoreGive(led_mutex);
}
static uint16_t led_set_bit_low(uint16_t* data,uint8_t bit){
	return (uint16_t)(data[0] & (~(0x01 << bit))); // (n & (~(1 << (k - 1))))
}
static uint16_t led_set_bit_high(uint16_t* data,uint8_t bit){
	return (uint16_t)(data[0] | (0x01 << bit)); //(n | (1 << (k - 1)))
}
static void led_set_blink(Led_Hmi *p_led,int color,uint32_t time_driff_ms){
	p_led->time_poll_ms += TIME_LED_Process_Ms;
	if(p_led->time_poll_ms > (time_driff_ms)){
		p_led->time_poll_ms = 0;
		p_led->led_state = !p_led->led_state;
		if(p_led->led_state == true){
			led_hmi_write(p_led,COLOR_BLACK);
		}
		else{
			led_hmi_write(p_led,color);
		}
	}
}

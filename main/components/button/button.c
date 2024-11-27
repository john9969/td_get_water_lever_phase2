/*
 * button.c
 *
 *  Created on: Dec 26, 2022
 *      Author: Admin
 */


#include "button.h"
#include "esp_log.h"
#include "my_wifi.h"
//static const char *TAG = "BUTTON";

#define BUTTON_TIME_DELAY 10
#define TIME_RESET_WIFI_INFO 5000
static void button_task(void* arg);
#define MAX_TIME_DEBONE 1000
#define MAX_TIME_HOLD 10
void button_init(Button *p_button,MBS_GPIO *gpio){
	//p_button->gpio = gpio;
	p_button->time = 0;
//	p_button->time_debone = 0;
//	p_button->mode = BT_RELEASE;
	p_button->button_task = NULL;
	xTaskCreate(button_task,"Button task",1024*2,(void*)p_button,3,&p_button->button_task);
}

void button_deinit(Button *p_button){
	vTaskDelete(p_button->button_task);
}

BUTTON_MODE button_get_state(Button *p_button){
	return p_button->mode;
}

static void button_task(void* arg){
	Button *p_button = (Button*)arg;
	GPIO_PIN_LEVEL pin_level = GPIO_PIN_SET;
	for(;;){
		pin_level = gpio_get_level(0);

		if(pin_level == GPIO_PIN_RESET){
			p_button->time += BUTTON_TIME_DELAY;
		}
		else{
			p_button->time = 0;
		}
		if(p_button->time >= TIME_RESET_WIFI_INFO){
			wifi_clear_info();
			esp_restart();
		}
		vTaskDelay(pdMS_TO_TICKS(BUTTON_TIME_DELAY));
	}

}


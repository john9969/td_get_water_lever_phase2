/*
 * button.c
 *
 *  Created on: Dec 26, 2022
 *      Author: Admin
 */


#include "button.h"
#include "esp_log.h"
#include "my_wifi.h"
static const char *TAG = "BUTTON";

#define BUTTON_TIME_DELAY 10
#define TIME_GET_INTO_CONFIG_MODE 3000
static void button_task(void* arg);
#define MAX_TIME_DEBONE 1000
#define MAX_TIME_HOLD 10
void button_init(Button *p_button){
	p_button->callback = button_wake_callback;
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
		pin_level = gpio_get_level(WAKEUP_PIN);
		// if(pin_level == GPIO_PIN_SET) {
		// 	ESP_LOGI(TAG, "BUTTON State: HIGH" );
		// }
		// else {
		// 	ESP_LOGI(TAG, "BUTTON State: LOW" );
		// }
		if(pin_level == GPIO_PIN_RESET){
			p_button->time += BUTTON_TIME_DELAY;
		}
		else{
			/**
			 * Check button hold
			 * if button hold more then 5s and release 
			 * active call back
			 */
			if(p_button->time >= TIME_GET_INTO_CONFIG_MODE){
				p_button->mode = BT_PRESS;
				p_button->callback(p_button);
			}
			p_button->time = 0;
		}
		/**
		 * Check button hold
		 * if button hold and not release, active call back 
		 * change led status only, logic in app_main
		 */
		if(p_button->time >= TIME_GET_INTO_CONFIG_MODE){
			p_button->mode = BT_HOLD;
			p_button->callback(p_button);
			
		}
		vTaskDelay(pdMS_TO_TICKS(BUTTON_TIME_DELAY));
	}

}
Button wake_button_process = {
		.gpio = &button_gpio,
		.mode = BT_RELEASE,
		.time = 0,
		.callback = button_wake_callback,
		.button_task = NULL
};

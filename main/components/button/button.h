/*
 * button.h
 *
 *  Created on: Dec 26, 2022
 *      Author: Admin
 */

#ifndef MAIN_COMPONENTS_BUTTON_BUTTON_H_
#define MAIN_COMPONENTS_BUTTON_BUTTON_H_

#include "gpio_hw.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"



typedef enum{
	BT_RELEASE = 0,
	BT_PRESS = 1,
	BT_HOLD
}BUTTON_MODE;

typedef struct Button_t Button;

struct Button_t{
	MBS_GPIO *gpio;
	volatile unsigned int time;
	volatile unsigned int time_debone;
	BUTTON_MODE mode;
	TaskHandle_t button_task;
};

void button_init(Button *p_button,MBS_GPIO *gpio);
void button_deinit(Button *p_button);
BUTTON_MODE button_get_state(Button *p_button);


#endif /* MAIN_COMPONENTS_BUTTON_BUTTON_H_ */

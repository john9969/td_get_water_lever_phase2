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
typedef void (*Button_Callback)(void *);
struct Button_t{
	GPIO *gpio;
	BUTTON_MODE mode;
	volatile unsigned int time;
	Button_Callback callback;
	TaskHandle_t button_task;
};

void button_init(Button *p_button,GPIO *gpio);
void button_deinit(Button *p_button);
BUTTON_MODE button_get_state(Button *p_button);

extern void button_callback(void *arg);
#endif /* MAIN_COMPONENTS_BUTTON_BUTTON_H_ */

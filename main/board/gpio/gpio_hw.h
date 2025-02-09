/*
 * gpio_hw.h
 *
 *  Created on: Dec 16, 2022
 *      Author: Admin
 */

#ifndef MAIN_BOARD_GPIO_GPIO_HW_H_
#define MAIN_BOARD_GPIO_GPIO_HW_H_

#include "driver/gpio.h"

/*
 * using by v1.0
#define Node_1_GPIO 23
#define Node_2_GPIO 19
#define Node_3_GPIO 18
#define Node_4_GPIO 5
#define Node_5_GPIO 33
#define Node_6_GPIO 25
#define Node_7_GPIO 26
#define Node_8_GPIO 27
*/


/* Using by v2.0 */
#define BUTTON_GPIO_PIN 	(33)
#define LED_SIGNAL_PIN 		(25)
#define WIFI_POWERON_PIN 	(26)
#define WAKEUP_PIN 			(36)

typedef enum{
	GPIO_PIN_RESET = 0,
	GPIO_PIN_SET 
}GPIO_PIN_LEVEL;

typedef enum{
	GPIO_OUTPUT = 0,
	GPIO_INPUT,
}GPIO_MODE_t;

typedef enum{
	PULL_UP_DOWN_NO = 0,
	PULL_UP_EN,
	PULL_DOWN_EN
}GPIO_Pull_Up_Down_En_t;

typedef struct GPIO_t GPIO;

struct GPIO_t{
	int pin;
	GPIO_MODE_t mode;
	GPIO_Pull_Up_Down_En_t pull_en;
	GPIO_PIN_LEVEL level;
};

void gpio_init(GPIO *gpio);
GPIO_PIN_LEVEL gpio_read(GPIO *gpio);
void gpio_pin_write(GPIO *gpio, GPIO_PIN_LEVEL level);
void gpio_pin_toggle(GPIO *gpio);


extern GPIO led_signal;
extern GPIO button_gpio;
extern GPIO wifi_poweron_gpio;

#endif /* MAIN_BOARD_GPIO_GPIO_HW_H_ */

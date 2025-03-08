/*
 * gpio_hw.c
 *
 *  Created on: Dec 16, 2022
 *      Author: Admin
 */



#include "gpio_hw.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

void gpio_init(GPIO *gpio){
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    if(gpio->mode == GPIO_OUTPUT){
    	io_conf.mode = GPIO_MODE_OUTPUT;
    }
    else{
    	io_conf.mode = GPIO_MODE_INPUT;
    }
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL<<gpio->pin);
    //disable pull-down mode
    if(gpio->pull_en == PULL_UP_EN){
    	io_conf.pull_up_en = 1;
    }
    else if(gpio->pull_en == PULL_DOWN_EN){
    	io_conf.pull_down_en = 1;
    }
    else{
    	io_conf.pull_down_en = 0;
    	io_conf.pull_up_en = 0;
    }
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}
GPIO_PIN_LEVEL gpio_read(GPIO *gpio){
	gpio->level = gpio_get_level(gpio->pin);
	return gpio->level;
}
void gpio_pin_write(GPIO *gpio, GPIO_PIN_LEVEL level){
	gpio->level = level;
	gpio_set_level(gpio->pin,gpio->level);
}
void gpio_pin_toggle(GPIO *gpio){
	gpio->level == GPIO_PIN_SET ? gpio_pin_write(gpio,GPIO_PIN_RESET):gpio_pin_write(gpio,GPIO_PIN_SET);
}

GPIO led_signal = { .pin = LED_SIGNAL_PIN, .mode = GPIO_OUTPUT, .pull_en = PULL_UP_DOWN_NO };
GPIO button_gpio = { .pin = WAKEUP_PIN, .mode = GPIO_INPUT};
GPIO wifi_poweron_gpio = { .pin = WIFI_POWERON_PIN, .mode = GPIO_OUTPUT, .pull_en = PULL_UP_EN };
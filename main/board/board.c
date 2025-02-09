/*
 * board.c
 *
 *  Created on: Dec 14, 2022
 *      Author: tuanndm
 */

#include "board.h"
#include "esp_random.h"
#include "stdio.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_event.h"

char auther_str[50] = {NULL};


void board_init(void){
	esp_err_t err =  pcf8563_init_desc(&pcf8563_dev, I2C_NUM_0, SDA_GPIO_NUM, SCL_GPIO_NUM);
	if (err != ESP_OK) {
		// log_e("Could not initialize PCF8563 device descriptor");
	}
	flash_init();
	drv_timer_init();
	int size = flash_read_str("auther", auther_str);
	if(size == -1){
		uint32_t auther = esp_random();
		printf("Auther : 0x%08X\n",(unsigned int)auther);
		sprintf(auther_str,"%08X",(unsigned int)auther);
		flash_write_str("auther",auther_str);
	}
	gpio_init(&led_signal);
	gpio_init(&button_gpio);
	gpio_init(&wifi_poweron_gpio);
}

void board_deinit(void){
}



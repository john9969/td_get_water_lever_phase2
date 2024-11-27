/*
 * board.c
 *
 *  Created on: Dec 14, 2022
 *      Author: MinhNhan
 */

#include "board.h"
#include "esp_random.h"
#include "stdio.h"
#include "74hc595.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_event.h"
char auther_str[50] = {0};

void board_init(void){
	flash_init();
	drv_timer_init();
	//HAL_UART_Init(&huart1);
	ioe_hw_init(&ioe_hw);
	gpio_init(&button_gpio);
	gpio_init(&gpio_warning);
	gpio_init(&led_wifi);
	gpio_init(&can_stb);
	gpio_init(&lte_poweron_gpio);
	gpio_init(&lte_rs_gpio);

/*
 * Using V1.0
 *
	for(uint16_t i = 0;i<8;i++)
		gpio_init(&gpio_nodeid[i]);
*/
	hc595_init(&node_id_io,NODE_ID_DATA,NODE_ID_SCK,NODE_ID_LATCH,1);
	hc595_init(&led_io,LED_DATA,LED_SCK,LED_LATCH,2);
	int size = flash_read_str("auther", auther_str);
	if(size == -1){
		uint32_t auther = esp_random();
		printf("Auther : 0x%08X\n",(unsigned int)auther);
		sprintf(auther_str,"%08X",(unsigned int)auther);
		flash_write_str("auther",auther_str);
	}
}
void board_deinit(void){
	can_hardware_deinit();
}



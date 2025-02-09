/*
 * board.h
 *
 *  Created on: 2024/11/27
 *      Author: tuanndm
 */

#ifndef MAIN_BOARD_BOARD_H_
#define MAIN_BOARD_BOARD_H_

#include "drv_timer.h"
#include "uart_hw/huart.h"
#include "gpio/gpio_hw.h"
#include "esp_system.h" 
#include "esp_log.h" 
#include "driver/rtc_io.h"
#include "driver/periph_ctrl.h"
#include "driver/rtc_cntl.h"
#include "hw_flash.h"
#include "driver/i2c.h"
#include "hw_pcf8563.h"
void board_init(void);
void board_deinit(void);

extern char auther_str[50];

#endif /* MAIN_BOARD_BOARD_H_ */

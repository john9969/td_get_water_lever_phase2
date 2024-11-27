/*
 * buzzer.h
 *
 *  Created on: Dec 16, 2022
 *      Author: Admin
 */

#ifndef MAIN_COMPONENTS_BUZZER_BUZZER_H_
#define MAIN_COMPONENTS_BUZZER_BUZZER_H_

#include "gpio_hw.h"
#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define BUZZER_DEFAULT_ACTIVE_TIME_mS							(5000UL)

typedef enum{
	WARNING_NONE = 0,
	WARNING_100_HZ,
	WARNING_1_HZ,
	WARNING_INSERT_BP,
	WARNING_LOSCONNECT,
	WARNING_RETURN_NEW_BP,
	WARNING_RETURN_OLD_BP
}WARNING_MODE;

typedef struct Buzzer_t Buzzer;

struct Buzzer_t{
	MBS_GPIO *buzzer_drv;
	TaskHandle_t warning_task;
	QueueHandle_t warning_queue;
	WARNING_MODE mode;
	uint32_t active_counter_ms;
	uint32_t active_time_ms;
};



void buzzer_init(Buzzer *p_buzzer);
void buzzer_setmode(Buzzer *p_buzzer, WARNING_MODE mode);
void buzzer_deinit(Buzzer *p_buzzer);

extern Buzzer bz_warning;

#endif /* MAIN_COMPONENTS_BUZZER_BUZZER_H_ */

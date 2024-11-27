/*
 * buzzer.c
 *
 *  Created on: Dec 16, 2022
 *      Author: Admin
 */

#include "buzzer.h"
#include "drv_timer.h"
#include "stdio.h"

static void buzzer_task(void *arg);
static void warning_set_mode(Buzzer *p_buzzer,WARNING_MODE mode);

#define WARNING_TASK_PRORITY 4

static char name_mode[20] = { 0 };

char* print_mode(WARNING_MODE mode) {
	switch (mode) {
	case WARNING_NONE:
		memset(name_mode, 0, 20);
		strcpy(name_mode, "WARNING_NONE");
		break;
	case WARNING_100_HZ:
		memset(name_mode, 0, 20);
		strcpy(name_mode, "WARNING_100_HZ");
		break;
	default:
		memset(name_mode, 0, 20);
		strcpy(name_mode, "WARNING_1_HZ");
		break;
	}
	return name_mode;
}

void buzzer_init(Buzzer *p_buzzer) {
	p_buzzer->active_counter_ms=0;
	p_buzzer->active_time_ms=BUZZER_DEFAULT_ACTIVE_TIME_mS;
	p_buzzer->mode = WARNING_NONE;
	if (p_buzzer->buzzer_drv == NULL) {
		p_buzzer->buzzer_drv = &gpio_warning;
	}
	p_buzzer->warning_queue = xQueueCreate(10, sizeof(WARNING_MODE));
	xTaskCreate(buzzer_task, "warning", 1024, (void*) p_buzzer,
			WARNING_TASK_PRORITY, &p_buzzer->warning_task);
}
void buzzer_setmode(Buzzer *p_buzzer, WARNING_MODE mode) {
	if (xQueueSend(p_buzzer->warning_queue,&mode,portMAX_DELAY) == pdPASS) {
		//printf("Set mode %s success\r\n", print_mode(mode));
	} else {
		//printf("Set mode %s false\r\n", print_mode(mode));
	}
}
void buzzer_deinit(Buzzer *p_buzzer) {
	vTaskDelete(p_buzzer->warning_task);
	vQueueDelete(p_buzzer->warning_queue);
}

static void buzzer_task(void *arg) {

	Buzzer *p_buzzer = (void*) arg;
	for (;;) {
		WARNING_MODE mode = WARNING_NONE;
		if (xQueueReceive(p_buzzer->warning_queue, &mode,
				(TickType_t) 1) == pdPASS) {
			p_buzzer->mode = mode;
		}
		switch (p_buzzer->mode) {
		case WARNING_NONE:
			gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_RESET);
			vTaskDelay(pdMS_TO_TICKS(1));
			break;
		case WARNING_100_HZ:
			gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_RESET);
			vTaskDelay(pdMS_TO_TICKS(100));
			gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_SET);
			vTaskDelay(pdMS_TO_TICKS(100));
			break;
		case WARNING_1_HZ:
			gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_RESET);
			vTaskDelay(pdMS_TO_TICKS(1000));
			gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_SET);
			vTaskDelay(pdMS_TO_TICKS(1000));
			break;
		case WARNING_INSERT_BP:
			gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_SET);
			vTaskDelay(pdMS_TO_TICKS(500));
			gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_RESET);
			warning_set_mode(p_buzzer,WARNING_NONE);
			break;
		case WARNING_LOSCONNECT:
			gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_SET);
			vTaskDelay(pdMS_TO_TICKS(10000));
			warning_set_mode(p_buzzer,WARNING_NONE);
			break;
		case WARNING_RETURN_NEW_BP:
			for (uint16_t i = 0; i < 2; i++) {
				gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_SET);
				vTaskDelay(pdMS_TO_TICKS(500));
				gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_RESET);
				vTaskDelay(pdMS_TO_TICKS(500));
			}
			warning_set_mode(p_buzzer,WARNING_NONE);
			break;
		case WARNING_RETURN_OLD_BP:
			for (uint16_t i = 0; i < 5; i++) {
				gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_SET);
				vTaskDelay(pdMS_TO_TICKS(500));
				gpio_pin_write(p_buzzer->buzzer_drv, GPIO_PIN_RESET);
				vTaskDelay(pdMS_TO_TICKS(500));
			}
			warning_set_mode(p_buzzer,WARNING_NONE);
			break;
		default:
			break;
		}
		vTaskDelay(1);
	}
}
static void warning_set_mode(Buzzer *p_buzzer,WARNING_MODE mode){
	switch(mode){
	case WARNING_NONE:

		break;
	case WARNING_100_HZ:

		break;
	case WARNING_1_HZ:

		break;
	case WARNING_INSERT_BP:

		break;
	case WARNING_LOSCONNECT:

		break;
	case WARNING_RETURN_NEW_BP:
		break;
	case WARNING_RETURN_OLD_BP:

		break;
	default:
		break;
	}
	p_buzzer->mode = mode;
}
Buzzer bz_warning = { .buzzer_drv = &gpio_warning, .mode = WARNING_NONE, };

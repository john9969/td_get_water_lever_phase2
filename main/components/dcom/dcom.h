/**
 * @file dcom.h
 * @brief This file controls the state of Dcom, allowing it to be turned on and off.
 * 
 * Note: To utilize this file effectively, you only need to set the state of the Dcom variable.
 * The file contains a comprehensive process that checks the state of the Dcom variable and 
 * processes it accordingly, ensuring that the state transitions are handled smoothly and 
 * efficiently. This abstraction allows for a clean and maintainable approach to managing 
 * the Dcom state without requiring direct manipulation of the underlying mechanisms.
 */
#ifndef DCOM_H
#define DCOM_H
#include "gpio_hw.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#define DCOM_ON dcom_set_state(&dcom, STATE_ON);
#define DCOM_OFF dcom_set_state(&dcom, STATE_OFF);

typedef enum STATE{
    STATE_OFF =0,
    STATE_ON
} STATE;

typedef struct {
    GPIO *dcom_gpio;
    volatile STATE state;
    TaskHandle_t dcom_task;
} Dcom;
void dcom_deinit(Dcom *p_dcom);
void dcom_init(Dcom* p_dcom);
void dcom_set_state(Dcom* dcom,STATE state);
extern Dcom dcom;
#endif // DCOM_H

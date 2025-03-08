#ifndef LAZER_SENSOR_H
#define LAZER_SENSOR_H


#include <stdint.h>
#include "huart.h"

#define ENABLE_TEST_SENSOR 0
#define MAX_TIME_OUT_SENSOR_RESPONSE (1000UL)
#define LAZER_BAUDRATE		(19200)
#define MAX_DISTANCE        (60000UL)

#define SENSOR_TX_PIN		(18)
#define SENSOR_RX_PIN 		(19)
#define COMMAND_SENSOR_NUM    (3)

typedef uint32_t (*lazer_sensor_process)(void*);
typedef enum {
    LAZER_SENSOR_COMMAND_ON = 0,
    LAZER_SENSOR_COMMAND_OFF,
    LAZER_SENSOR_COMMAND_GET_DISTANCE
} LAZER_SENSOR_COMMAND;

typedef struct Command{
    LAZER_SENSOR_COMMAND command;
    const uint8_t data[9];
} Command;


typedef struct LazerSensor{
    UART_Typedef* huart;
    lazer_sensor_process process;
}LazerSensor;
void lazer_sensor_init(void * arg);
void lazer_sensor_deinit(void * arg);
bool lazer_sensor_on(void * arg);
bool lazer_sensor_off(void * arg);
uint32_t lazer_sensor_get_distance(void * arg);

extern LazerSensor lazer_sensor;
extern Command command_list[COMMAND_SENSOR_NUM];

#endif
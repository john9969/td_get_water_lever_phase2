#ifndef LAZER_SENSOR_H
#define LAZER_SENSOR_H


#include <stdint.h>
#include "huart.h"


#define SENSOR_TX_PIN		(18)
#define SENSOR_RX_PIN 		(19)
#define COMMAND_SENSOR_NUM    (3)
typedef struct LazerSensor{
    UART_Typedef* huart;
}LazerSensor;
typedef enum {
    LAZER_SENSOR_COMMAND_ON = 0,
    LAZER_SENSOR_COMMAND_OFF,
    LAZER_SENSOR_COMMAND_GET_DISTANCE
} LAZER_SENSOR_COMMAND;
typedef struct Command{
    LAZER_SENSOR_COMMAND command;
    uint8_t data[20];
} Command;

Command command_list[COMMAND_SENSOR_NUM] = {
    {LAZER_SENSOR_COMMAND_ON,{0x55,0x01}},
    {LAZER_SENSOR_COMMAND_OFF,{0x55,0x02}},
    {LAZER_SENSOR_COMMAND_GET_DISTANCE,{0x55,0x04}}
};

void lazer_sensor_init(void * arg);
void lazer_sensor_deinit(void * arg);
void lazer_sensor_on(void * arg);
uint32_t lazer_sensor_get_distance(void * arg);

extern LazerSensor lazer_sensor;


#endif
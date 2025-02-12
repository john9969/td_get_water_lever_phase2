#include "lazer_sensor.h"

void lazer_sensor_init(void * arg){
    LazerSensor * sensor = (LazerSensor*)arg;
    sensor->huart = &huart1;
    sensor->huart->pin_rx = SENSOR_RX_PIN;
    sensor->huart->pin_tx = SENSOR_TX_PIN;
    HAL_UART_Init(sensor->huart);
}
LazerSensor lazer_sensor = {
    .huart = &huart1,
};

void lazer_sensor_deinit(void * arg){
    (void)arg;    
}
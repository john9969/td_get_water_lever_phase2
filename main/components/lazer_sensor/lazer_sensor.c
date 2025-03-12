#include "lazer_sensor.h"

#include "esp_log.h"
#include "app_config.h"

static uint32_t pri_lazer_sensor_process_on_off(void* arg);
static uint32_t pri_lazer_sensor_process_get_distance(void* arg);

#define MAX_DISTANCE            (60000)
#if ENABLE_TEST_SENSOR
static void test_sensor(void * arg);
#endif
static uint32_t _distance;
static uint8_t _data_come_array[20];
void lazer_sensor_init(void * arg){
  LazerSensor * sensor = (LazerSensor*)arg;
  sensor->huart = &huart1;
  sensor->huart->pin_rx = SENSOR_RX_PIN;
  sensor->huart->pin_tx = SENSOR_TX_PIN;
  sensor->huart->baurate = LAZER_BAUDRATE;
  HAL_UART_Init(sensor->huart);
  vTaskDelay(pdMS_TO_TICKS(5000));
#if ENABLE_TEST_SENSOR
  test_sensor(sensor);
#endif
}
LazerSensor lazer_sensor = {
    .huart = &huart1,
};

void lazer_sensor_deinit(void * arg){
    (void)arg;    
}
static bool has_done_response = false;
bool lazer_sensor_on(void * arg){
  LazerSensor * sensor = (LazerSensor*)arg;
  for(int i=0; i < COMMAND_SENSOR_NUM; i++){
    if(command_list[i].command == LAZER_SENSOR_COMMAND_ON){
      HAL_UART_Transmit(sensor->huart,command_list[i].data,sizeof(command_list[i].data));
      sensor->process = pri_lazer_sensor_process_on_off;
      has_done_response = false;
    }
  }
  return sensor->process(sensor);
}
bool lazer_sensor_off(void * arg){
  LazerSensor * sensor = (LazerSensor*)arg;
  for(int i=0; i < COMMAND_SENSOR_NUM; i++){
    if(command_list[i].command == LAZER_SENSOR_COMMAND_OFF){
      HAL_UART_Transmit(sensor->huart,command_list[i].data,sizeof(command_list[i].data));
      sensor->process = pri_lazer_sensor_process_on_off;
      has_done_response = false;
    }
  }
  return sensor->process(sensor);
}
uint32_t lazer_sensor_get_distance(void* arg){
  LazerSensor * sensor = (LazerSensor*)arg;
  for(int i=0; i < COMMAND_SENSOR_NUM; i++){
    if(command_list[i].command == LAZER_SENSOR_COMMAND_GET_DISTANCE){
      HAL_UART_Transmit(sensor->huart,command_list[i].data,sizeof(command_list[i].data));
      sensor->process = pri_lazer_sensor_process_get_distance;
      has_done_response = false;
      _distance =0;
    }
  }
  return sensor->process(sensor);
}
#define TIME_DELAY_READ_SENSOR (100UL)
uint32_t pri_lazer_sensor_process_on_off(void* arg)
{
  LazerSensor * sensor = (LazerSensor*)arg;

  uint32_t timeout = 0;
  while (!has_done_response && (timeout < MAX_TIME_OUT_SENSOR_RESPONSE)){
    // ESP_LOGI("LAZER_SENSOR","Data available: %d",HAL_UART_Available(sensor->huart)); 
       timeout += TIME_DELAY_READ_SENSOR;
    vTaskDelay(pdMS_TO_TICKS(TIME_DELAY_READ_SENSOR));
  }
  return has_done_response;
}
uint32_t pri_lazer_sensor_process_get_distance(void* arg)
{
  LazerSensor * sensor = (LazerSensor*)arg;

  uint32_t timeout = 0;
  while (!has_done_response && (timeout < MAX_TIME_OUT_SENSOR_RESPONSE)){
    timeout += TIME_DELAY_READ_SENSOR;
    vTaskDelay(pdMS_TO_TICKS(TIME_DELAY_READ_SENSOR));
  }
  if(!has_done_response) {
    return 0;
  }
  for(int i = 6; i < 10; i++){
    ESP_LOGI("LAZER_SENSOR","Distance: %lu mm",_distance);
    if(_data_come_array[i]!= '\0')
    _distance = (_distance<<8|_data_come_array[i]);
  }
  if(_distance > MAX_DISTANCE){
    return MAX_DISTANCE;
  }
  return _distance;
}

void uart_has_data_come(void * arg, uint8_t *data, uint32_t len){
    LazerSensor * sensor = (LazerSensor*)arg;
    ESP_LOGI("UART","Data come");
    for(int i = 0; i < len; i++){
        ESP_LOGI("UART","Data: 0x%x",data[i]);
    }
    if(data[0] == 0xAA){
      memset(_data_come_array,0,sizeof(_data_come_array));
      memcpy(_data_come_array,data,len);
      ESP_LOGI("UART","Data come OK");
      has_done_response = true;
    }
    else {
        ESP_LOGI("UART","Data come FAIL");
        memset(_data_come_array,0,sizeof(_data_come_array));
        memset(data,0,len);
    }
}

#if ENABLE_TEST_SENSOR
static void test_sensor(void * arg){
    LazerSensor * sensor = (LazerSensor*)arg;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI("TEST_SENSOR","Test Sensor ON");

        if(lazer_sensor_on(sensor)){
            ESP_LOGI("TEST_SENSOR","Test Sensor ON OK");
        }
        else {
            ESP_LOGI("TEST_SENSOR","Test Sensor ON FAIL");
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI("TEST_SENSOR","Test Sensor OFF");
        ESP_LOGI("TEST_SENSOR","Distance: %lu",lazer_sensor_get_distance(sensor));
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI("TEST_SENSOR","Distance: %lu",lazer_sensor_get_distance(sensor));
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI("TEST_SENSOR","Distance: %lu",lazer_sensor_get_distance(sensor));
        vTaskDelay(pdMS_TO_TICKS(5000));
        if(lazer_sensor_off(sensor)){
            ESP_LOGI("TEST_SENSOR","Test Sensor OFF OK");
        }
        else {
            ESP_LOGI("TEST_SENSOR","Test Sensor OFF FAIL");
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    ESP_LOGI("TEST_SENSOR","FINISH");
    
}
#endif

Command command_list[COMMAND_SENSOR_NUM] = {
  {LAZER_SENSOR_COMMAND_ON,           {0xAA,  0x00, 0x01, 0xBE,   0x00,   0x01,   0x00,   0x01,   0xC1}},
  {LAZER_SENSOR_COMMAND_OFF,          {0xAA,  0x00, 0x01, 0xBE,   0x00,   0x01,   0x00,   0x00,   0xC0}},
  {LAZER_SENSOR_COMMAND_GET_DISTANCE, {0xAA,  0x00, 0x00, 0x20,   0x00,   0x01,   0x00,   0x00,   0x21}}
};
#ifndef HW_PCF8563_H_
#define HW_PCF8563_H_

#include <time.h>
#include <stdbool.h>
#include "i2cdev.h"

#define PCF8563_ADDR 0x51 //!< I2C address
//#define PCF8563_ADDR 0xA2 //!< I2C address

#define PCF8563_ADDR_STATUS1 0x00   //!< Status register
#define PCF8563_ADDR_STATUS2 0x01   //!< Status register
#define PCF8563_ADDR_TIME    0x02   //!< Time register
#define PCF8563_ADDR_ALARM   0x09   //!< Alarm register
#define PCF8563_ADDR_CONTROL 0x0d   //!< Control/status register         
#define PCF8563_ADDR_TIMER   0x0e   //!< Timer register
#define PCF8563_ALARM_DONT_CARE 0x8f    //!< Don't care value for alarm time

#define SDA_GPIO_NUM 21   //!< GPIO number for SDA
#define SCL_GPIO_NUM 22   //!< GPIO number for SCL

uint8_t bcd2dec(uint8_t val);
uint8_t dec2bcd(uint8_t val);
esp_err_t pcf8563_init_desc(i2c_dev_t *dev, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);
esp_err_t pcf8563_reset(i2c_dev_t *dev);
esp_err_t pcf8563_set_time(i2c_dev_t *dev, struct tm *time);
esp_err_t pcf8563_get_time(i2c_dev_t *dev, struct tm *time);
esp_err_t pcf8563_set_alarm(i2c_dev_t *dev, struct tm *time);
extern i2c_dev_t pcf8563_dev;
extern time_t current_time; 
#endif /* HW_PCF8563_H_ */

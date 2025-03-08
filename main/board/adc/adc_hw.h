#ifndef ADC_HW_H
#define ADC_HW_H

#include <stdio.h>
#include "driver/adc.h"
// #include "esp_adc_cal.h"
#include "esp_log.h"
#define ADC_PIN         ADC1_CHANNEL_6  // GPIO34
#define ADC_WIDTH       ADC_WIDTH_BIT_12
#define ADC_ATTEN       ADC_ATTEN_DB_11
#define OFFSET          1  // Offset from bits to mili voltage
// Function to initialize the ADC hardware
void adc_init(void);
// Function to read a value from the ADC
int adc_read_value();
// Function to read a voltage from the ADC
int adc_read_vol();
#endif // ADC_HW_H
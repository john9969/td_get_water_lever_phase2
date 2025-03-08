#include "adc_hw.h"
static const char * TAG = "ADC";
int adc_read_value() {
    int raw_value = adc1_get_raw(ADC_PIN);
    ESP_LOGI(TAG, "ADC Raw: %d", raw_value);
    return raw_value;
}
int adc_read_vol(){
    return adc_read_value()*OFFSET;
}
void adc_init(void){
    // Configure the ADC pin as an input
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_PIN, ADC_ATTEN);
}
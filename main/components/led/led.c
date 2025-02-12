#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "gpio_hw.h"
#include "base.h"
static const char TAG[] = "LED_CONTROL";
static const char TAG_TEST[] = "LED_TEST";
static int old_error_times = 0;
void led_control_task(void *arg) {
    led_control_t * p_led = (led_control_t*) arg;
    while (1) {
        if (p_led->times > 0 && p_led->state != LED_BLINK_ERR)    
        {
            p_led->times--;
        }
        else if (p_led->times == 0 && p_led->state != LED_BLINK_ERR) 
        {
            vTaskDelay(pdMS_TO_TICKS(1000));   
            continue;
        }

        switch (p_led->state){
            case LED_OFF:
                gpio_pin_write(&led_signal, GPIO_PIN_SET);      //LED OFF
                if(old_error_times >0){
                    old_error_times =0;
                }                
                if(p_led->times > 0){
                    p_led->times = 0;
                }   
                ESP_LOGI(TAG, "LED OFF");
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
            case LED_ON:
                gpio_pin_write(&led_signal, GPIO_PIN_RESET);      //LED ON
                ESP_LOGI(TAG, "LED ON");
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
            case LED_BLINK_ERR:
                ESP_LOGI(TAG, "LED BLINK Error: %s", (char*)&error_list.errors[error_list.count-1]); //print lastest error code
                for(int i = 0; i < p_led->times; i++) {
                    gpio_pin_write(&led_signal, GPIO_PIN_RESET);      //LED ON
                    vTaskDelay(pdMS_TO_TICKS(200));    
                    gpio_pin_write(&led_signal, GPIO_PIN_SET);      //LED OFF
                    vTaskDelay(pdMS_TO_TICKS(200));    
                }
                vTaskDelay(pdMS_TO_TICKS(2500));
                break;
            case LED_BLINK_1000MS:
                gpio_pin_write(&led_signal, GPIO_PIN_RESET);      //LED ON
                vTaskDelay(pdMS_TO_TICKS(500));
                gpio_pin_write(&led_signal, GPIO_PIN_SET);      //LED OFF
                vTaskDelay(pdMS_TO_TICKS(500));
                ESP_LOGI(TAG, "LED BLINK 1000MS");
                break;
            case LED_BLINK_3000MS:
                gpio_pin_write(&led_signal, GPIO_PIN_RESET);      //LED ON
                vTaskDelay(pdMS_TO_TICKS(1500));
                gpio_pin_write(&led_signal, GPIO_PIN_SET);      //LED OFF
                vTaskDelay(pdMS_TO_TICKS(1500));
                ESP_LOGI(TAG, "LED BLINK 3000MS");
                break;   
            default:    
                break;
        }
    }
}
void led_deinit(void * arx) {
    led_control_t * p_led = (led_control_t*) arx;
    vTaskDelete(p_led->led_task);
    p_led->led_task = NULL;
} 

bool set_led_state(void * arx, led_state_t state, int times) {
    led_control_t* p_led_control = (led_control_t*) arx;
    if((p_led_control->state == state) && (state != LED_BLINK_ERR)) {
        ESP_LOGI(TAG, "LED state is already %d", state);
        return false;
    }
    p_led_control->state = state;
    p_led_control->times = times;
    if(p_led_control->state == LED_BLINK_ERR){
        old_error_times = (int) times;
    }
    return true;
}
void signal_new_error(void * arg) {
    ErrorList * error = (ErrorList*) arg;
    ErrorCode error_code =  error->errors[error->count-1].code;
    if(error_code == ERROR_NONE) {
        set_led_state(&led_control, LED_OFF, 0);
        return;
    }
    set_led_state(&led_control, LED_BLINK_ERR, (int) error_code);
}

void led_init(void * arx) {
    led_control_t * p_led = (led_control_t*) arx;    
    xTaskCreate(led_control_task, "LED Control Task", 1024, p_led, 1, &(p_led->led_task));
}
#if TEST_LED
void test_led(void * arx) {
    led_control_t * p_led = (led_control_t*) arx;
    ESP_LOGI(TAG_TEST, "Test LED START");
    ESP_LOGI(TAG_TEST, "Initializing LED");
    led_init(p_led);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // ESP_LOGI(TAG_TEST, "Setting LED to ON");
    // set_led_state(p_led, LED_ON, -1);
    // vTaskDelay(pdMS_TO_TICKS(5000));

    // ESP_LOGI(TAG_TEST, "Setting LED to OFF");
    // if(set_led_state(p_led, LED_OFF, -1)){
    //     ESP_LOGI(TAG_TEST, "LED OFF Success");
    // }
    // vTaskDelay(pdMS_TO_TICKS(5000));

    // ESP_LOGI(TAG_TEST, "Setting LED to BLINK 1000MS 5 times");
    // set_led_state(p_led, LED_BLINK_1000MS, 5);
    // vTaskDelay(pdMS_TO_TICKS(10000));

    // ESP_LOGI(TAG_TEST, "Setting LED to BLINK 3000MS 4 times");
    // set_led_state(p_led, LED_BLINK_3000MS, 4);
    // vTaskDelay(pdMS_TO_TICKS(20000));

    // ESP_LOGI(TAG_TEST, "Setting LED to BLINK ERR Num 5");
    // set_led_state(p_led, LED_BLINK_ERR, 5);
    // vTaskDelay(pdMS_TO_TICKS(20000));
    
    // ESP_LOGI(TAG_TEST, "Setting LED to BLINK ERR Num 2");
    // set_led_state(p_led, LED_BLINK_ERR, 2);
    // vTaskDelay(pdMS_TO_TICKS(20000));
    
    // ESP_LOGI(TAG_TEST, "Setting LED to off led blink err");
    // set_led_state(p_led, LED_BLINK_ERR, 0);
    // vTaskDelay(pdMS_TO_TICKS(10000));
    

    ESP_LOGI(TAG_TEST, "Deinitializing LED");
    led_deinit(p_led);
}
#endif
led_control_t led_control = { 
    .led_gpio = &led_signal, 
    .state = LED_OFF, 
    .times = 0,
    .led_task = NULL
};
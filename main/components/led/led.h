#ifndef LED_H
#define LED_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_hw.h"
#define TEST_LED 0


typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINK_ERR,
    LED_BLINK_CONFIG,
    LED_BLINK_1000MS,
    LED_BLINK_3000MS
} led_state_t;

typedef struct {
    GPIO* led_gpio;
    led_state_t state;
    int times;
    TaskHandle_t led_task;
} led_control_t;


/**
 * @brief Initializes the LED.
 * Note:
 */
void led_init(void * arx);
/**
 * @brief deinti the LED.
 * Note:
 */
void led_deinit(void * arx);
/**
 * @brief Controls the LED state.
 * 
 * @param led_control The LED control identifier.
 * @param state The state of the LED (e.g., ON or OFF).
 * @param times The number of times the LED should blink. 
 * Note: If times is -1, the LED will keep its current state until it changes.
 * if error orcus, times is number of error code in runing modle 500ms
 */
bool set_led_state(void * arx, led_state_t state, int times);
#if TEST_LED
/**
 * @brief Test the LED.
 * 
 * @param arx The LED control identifier.
 */

void test_led(void * arx);
#endif // TEST_LED

extern led_control_t led_control;
#endif // LED_Hled_control
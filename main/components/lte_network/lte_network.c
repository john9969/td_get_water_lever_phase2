/*
 * lte_network.c
 *
 *  Created on: Apr 17, 2023
 *      Author: Admin
 */



#include <string.h>
#include "esp_netif.h"
#include "esp_modem_api.h"
#include "esp_log.h"
#include "esp_modem_c_api_types.h"
#include "esp_modem_config.h"
#include "esp_modem_dce_config.h"
#include "gpio_hw.h"

static esp_modem_dce_t *dce = NULL;

#define LTE_TX_PIN 17
#define LTE_RX_PIN 16
#define UART_RX_BUFF_SIZE 4096*2
#define UART_TX_BUFF_SIZE 4096*2
#define UART_QUEUE_SIZE 20
#define UART_EVENT_STACK_SIZE 4096*2
#define CONFIG_MODEM_PPP_APN "v-internet"

//static const char *TAG = "pppos";


esp_err_t modem_init_network(esp_netif_t *netif)
{
	(void) netif;
    return ESP_OK;
}

void modem_deinit_network(void)
{
    if (dce) {
        esp_modem_destroy(dce);
        dce = NULL;
    }
}

bool modem_start_network()
{
    return esp_modem_set_mode(dce, ESP_MODEM_MODE_DATA) == ESP_OK;
}

bool modem_stop_network()
{
    return esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND);
}

bool modem_check_sync()
{
    return esp_modem_sync(dce) == ESP_OK;
}

void modem_reset()
{
	gpio_pin_write(&lte_rs_gpio,GPIO_PIN_SET);
	vTaskDelay(pdMS_TO_TICKS(300));
	gpio_pin_write(&lte_rs_gpio,GPIO_PIN_RESET);
}

bool modem_check_signal()
{
    int rssi, ber;
    if (esp_modem_get_signal_quality(dce, &rssi, &ber) == ESP_OK) {
        return rssi != 99 && rssi > 5;
    }
    return false;
}
int modem_get_signal(){
    int rssi, ber;
    if (esp_modem_get_signal_quality(dce, &rssi, &ber) == ESP_OK) {
        return rssi;
    }
    return -99;
}
void modem_power_on(){
	// power on lte
	gpio_pin_write(&lte_poweron_gpio,GPIO_PIN_SET);
	vTaskDelay(pdMS_TO_TICKS(500));
	gpio_pin_write(&lte_poweron_gpio,GPIO_PIN_RESET);
}
void modem_power_off(){
	// power on lte
	gpio_pin_write(&lte_poweron_gpio,GPIO_PIN_SET);
	vTaskDelay(pdMS_TO_TICKS(1000));
	gpio_pin_write(&lte_poweron_gpio,GPIO_PIN_RESET);
}

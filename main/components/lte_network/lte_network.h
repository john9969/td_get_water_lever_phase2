/*
 * lte_network.h
 *
 *  Created on: Apr 17, 2023
 *      Author: Admin
 */

#ifndef MAIN_COMPONENTS_LTE_NETWORK_LTE_NETWORK_H_
#define MAIN_COMPONENTS_LTE_NETWORK_LTE_NETWORK_H_


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_netif.h"
#include "esp_modem_api.h"

/**
 * @brief Initialize a singleton covering the PPP network provided by the connected modem device
 *
 * @param netif Already created network interface in PPP mode
 *
 * @return ESP_OK on success
 */
esp_err_t modem_init_network(esp_netif_t *netif);

/**
 * @brief Destroys the single network DCE
 */
void modem_deinit_network();

/**
 * @brief Starts the PPP network
 */
bool modem_start_network();

/**
 * @brief Stops the PPP network
 */
bool modem_stop_network();

bool modem_check_sync();

void modem_reset();

bool modem_check_signal();

int modem_get_signal();
void modem_power_off();
void modem_power_on();

#ifdef __cplusplus
}
#endif


#endif /* MAIN_COMPONENTS_LTE_NETWORK_LTE_NETWORK_H_ */

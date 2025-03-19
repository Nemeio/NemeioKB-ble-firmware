#ifndef APP_BLE_NUS_H
#define APP_BLE_NUS_H

#include "ble_nus.h"

void app_ble_nus_init(void);
ret_code_t app_ble_nus_send_data(const uint8_t *p_data, size_t length, size_t *ret_length);

#endif // APP_BLE_NUS_H
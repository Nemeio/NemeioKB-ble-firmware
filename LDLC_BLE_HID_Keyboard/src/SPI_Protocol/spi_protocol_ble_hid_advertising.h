#ifndef SPI_PROTOCOL_BLE_HID_ADVERTISING_H
#define SPI_PROTOCOL_BLE_HID_ADVERTISING_H

#include "sdk_common.h"

ret_code_t set_advertising_start(uint32_t data_size);

ret_code_t set_advertising_stop(uint32_t data_size);

ret_code_t set_advertising_data_receive_start(uint32_t data_size);

ret_code_t set_advertising_data_receive(uint32_t data_index, uint8_t data);

ret_code_t configure_advertising_data_receive(uint32_t data_index, uint8_t data);
ret_code_t parse_enable_whitelist(uint8_t data_length, uint8_t *data);
ret_code_t parse_enable_directed(uint8_t data_length, uint8_t *data);

#endif //SPI_PROTOCOL_BLE_HID_ADVERTISING_H
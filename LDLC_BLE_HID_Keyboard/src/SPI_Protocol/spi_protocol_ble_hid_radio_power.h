#ifndef SPI_PROTOCOL_BLE_HID_RADIO_POWER_H
#define SPI_PROTOCOL_BLE_HID_RADIO_POWER_H

#include "sdk_common.h"

ret_code_t set_radio_power_start(uint32_t data_size);

ret_code_t set_radio_power_stop(uint32_t data_size);

ret_code_t set_radio_power_data_receive(uint32_t data_index, uint8_t data);

#endif //SPI_PROTOCOL_BLE_HID_RADIO_POWER_H
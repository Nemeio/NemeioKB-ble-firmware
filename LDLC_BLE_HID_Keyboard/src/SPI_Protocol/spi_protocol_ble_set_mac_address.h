#ifndef SPI_PROTOCOL_BLE_SET_MAC_ADDRESS_H
#define SPI_PROTOCOL_BLE_SET_MAC_ADDRESS_H

#include "sdk_common.h"

ret_code_t set_mac_address_start(size_t data_size);
ret_code_t set_mac_address_receive(uint32_t data_index, uint8_t data);
ret_code_t set_mac_address_stop(size_t data_size);

#endif
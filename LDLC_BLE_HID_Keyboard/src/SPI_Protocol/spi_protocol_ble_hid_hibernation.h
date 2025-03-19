#ifndef SPI_PROTOCOL_BLE_HID_HIBERNATION_H
#define SPI_PROTOCOL_BLE_HID_HIBERNATION_H

#include "sdk_common.h"

ret_code_t set_hibernation_start(size_t data_size);

ret_code_t set_hibernation_stop(size_t data_size);

ret_code_t set_hibernation(size_t data_size);

ret_code_t set_hibernation_action(size_t data_size);

#endif //SPI_PROTOCOL_BLE_HID_HIBERNATION_H

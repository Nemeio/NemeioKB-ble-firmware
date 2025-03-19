#ifndef SPI_PROTOCOL_SET_DEVICE_NAME_H
#define SPI_PROTOCOL_SET_DEVICE_NAME_H

#include "sdk_common.h"

ret_code_t set_device_name_start(size_t data_size);
ret_code_t set_device_name_receive(uint32_t data_index, uint8_t data);
ret_code_t set_device_name_stop(size_t data_size);

#endif //SPI_PROTOCOL_SET_DEVICE_NAME_H

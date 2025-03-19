#ifndef SPI_PROTOCOL_SET_DEVICE_PROPERTIES_H
#define SPI_PROTOCOL_SET_DEVICE_PROPERTIES_H

#include "sdk_common.h"

ret_code_t set_device_properties_protocol_version_start(size_t size);
ret_code_t set_device_properties_protocol_version_receive(uint32_t data_index, uint8_t data);
ret_code_t set_device_properties_protocol_version_stop(size_t size);

#endif //SPI_PROTOCOL_SET_DEVICE_PROPERTIES_H

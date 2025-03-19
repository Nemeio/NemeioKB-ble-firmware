#ifndef SPI_PROTOCOL_GET_DEVICE_LIST_H
#define SPI_PROTOCOL_GET_DEVICE_LIST_H

#include "sdk_common.h"

ret_code_t get_device_count_stop(size_t data_size);

ret_code_t get_device_info_device_id_start(size_t data_size);
ret_code_t get_device_info_device_id_receive(uint32_t data_index, uint8_t data);
ret_code_t get_device_info_stop(size_t data_size);

ret_code_t delete_device_device_id_start(size_t data_size);
ret_code_t delete_device_device_id_receive(uint32_t data_index, uint8_t data);
ret_code_t delete_device_stop(size_t data_size);

#endif //SPI_PROTOCOL_GET_DEVICE_LIST_H

#ifndef SPI_PROTOCOL_NUS_DATA_H
#define SPI_PROTOCOL_NUS_DATA_H

#include "sdk_common.h"

ret_code_t spi_protocol_serial_data_start(size_t data_size);
ret_code_t spi_protocol_serial_data_receive(uint32_t data_index, uint8_t data);
ret_code_t spi_protocol_serial_data_stop(size_t data_size);

ret_code_t spi_protocol_serial_data_send(const uint8_t *p_buffer, size_t data_length);

#endif // SPI_PROTOCOL_NUS_DATA_H

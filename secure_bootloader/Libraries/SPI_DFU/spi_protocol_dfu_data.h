#ifndef SPI_PROTOCOL_DFU_DATA_H
#define SPI_PROTOCOL_DFU_DATA_H

#include "sdk_common.h"

ret_code_t spi_protocol_dfu_data_receive(uint32_t data_index, uint8_t data);

ret_code_t spi_protocol_dfu_data_send(uint8_t * p_buffer, uint32_t data_length);

#endif // SPI_PROTOCOL_DFU_DATA_H
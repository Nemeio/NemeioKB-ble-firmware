#ifndef SPI_PROTOCOL_DFU_COMMIT_VERSION_H
#define SPI_PROTOCOL_DFU_COMMIT_VERSION_H

#include "sdk_common.h"

ret_code_t spi_protocol_dfu_commit_version_start(size_t data_size);
ret_code_t spi_protocol_dfu_commit_version_receive(uint32_t data_index, uint8_t data);
ret_code_t spi_protocol_dfu_commit_version_stop(size_t data_size);

#endif
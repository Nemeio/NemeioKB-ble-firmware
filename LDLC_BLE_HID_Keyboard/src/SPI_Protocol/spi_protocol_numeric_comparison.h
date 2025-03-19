#ifndef SPI_PROTOCOL_NUMERIC_COMPARISON_H
#define SPI_PROTOCOL_NUMERIC_COMPARISON_H

#include "sdk_common.h"
#include "ble_gap.h"

ret_code_t spi_protocol_numeric_comparison_reply_receive(uint32_t data_index, uint8_t data);
ret_code_t spi_protocol_numeric_comparison_reply_stop(size_t data_size);
ret_code_t spi_protocol_numeric_comparison_send_passkey(const uint8_t passkey[BLE_GAP_PASSKEY_LEN],
							size_t length);

#endif // SPI_PROTOCOL_PASSKEY_H

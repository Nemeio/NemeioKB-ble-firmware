#ifndef SPI_PROTOCOL_TX_H
#define SPI_PROTOCOL_TX_H

#include "sdk_common.h"

typedef void (*spi_protocol_tx_data_sent_evt_handler_t)();

ret_code_t spi_protocol_tx_set_data_to_send(const uint8_t *p_buffer, uint32_t data_length,
					    uint8_t command_id);
void spi_protocol_tx_data_sent(uint8_t tx_amount);
void spi_protocol_tx_init();
uint8_t *spi_protocol_tx_get_tx_buff();

#endif // SPI_PROTOCOL_TX_H
#include "spi_protocol_dfu_data.h"
#include "spi_protocol_tx.h"
#include "dfu_transport_spi.h"
#include "spi_protocol.h"

ret_code_t spi_protocol_dfu_data_receive(uint32_t data_index, uint8_t data) {
  return spi_dfu_byte_received(data);
}

ret_code_t spi_protocol_dfu_data_send(uint8_t * p_buffer, uint32_t data_length) {
  return spi_protocol_tx_set_data_to_send(p_buffer, data_length, DFU_DATA);
}
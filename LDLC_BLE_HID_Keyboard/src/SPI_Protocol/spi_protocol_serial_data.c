#include "spi_protocol_serial_data.h"
#include "spi_protocol_tx.h"
#include "spi_protocol.h"
#include "app_ble_nus.h"

#define MAX_APP_COMM_FRAME_SIZE 1024

static uint8_t m_app_comm_frame_buffer[MAX_APP_COMM_FRAME_SIZE] = { 0 };

ret_code_t spi_protocol_serial_data_start(size_t data_size)
{
	return NRF_SUCCESS;
}

ret_code_t spi_protocol_serial_data_receive(uint32_t data_index, uint8_t data)
{
	ret_code_t err_code = NRF_SUCCESS;

	if (data_index < MAX_APP_COMM_FRAME_SIZE) {
		m_app_comm_frame_buffer[data_index] = data;
	} else {
		err_code = NRF_ERROR_NO_MEM;
	}

	return err_code;
}

ret_code_t spi_protocol_serial_data_stop(size_t data_size)
{
	return app_ble_nus_send_data(m_app_comm_frame_buffer, data_size, &data_size);
}

ret_code_t spi_protocol_serial_data_send(const uint8_t *p_buffer, size_t data_length)
{
	return spi_protocol_tx_set_data_to_send(p_buffer, data_length, SERIAL_DATA);
}

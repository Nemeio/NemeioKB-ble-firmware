#include "spi_protocol_numeric_comparison.h"
#include "spi_protocol.h"
#include "spi_protocol_tx.h"
#include "app_lesc.h"

static bool m_b_accept = false;

ret_code_t spi_protocol_numeric_comparison_reply_receive(uint32_t data_index, uint8_t data)
{
	if (data_index > sizeof(m_b_accept)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_b_accept = data;

	return NRF_SUCCESS;
}

ret_code_t spi_protocol_numeric_comparison_reply_stop(size_t data_size)
{
	app_lesc_end_num_comp(m_b_accept);
	return NRF_SUCCESS;
}

ret_code_t spi_protocol_numeric_comparison_send_passkey(const uint8_t passkey[BLE_GAP_PASSKEY_LEN],
							size_t length)
{
	ret_code_t ret = NRF_SUCCESS;

	if (length != BLE_GAP_PASSKEY_LEN) {
		ret = NRF_ERROR_INVALID_PARAM;
	} else {
		ret = spi_protocol_tx_set_data_to_send(passkey, length, NUMERIC_COMPARISON);
	}

	return ret;
}

#include "spi_protocol_radio_test.h"

#include "radio_test_modes.h"

static enum TestMode_t m_testMode = TEST_MODE_MAX;

ret_code_t set_radio_test_mode_receive(uint32_t data_index, uint8_t data)
{
	if (data_index != 0) {
		return NRF_ERROR_INVALID_LENGTH;
	}

	if (data >= TEST_MODE_MAX) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_testMode = (enum TestMode_t)data;

	return NRF_SUCCESS;
}

ret_code_t set_radio_test_stop(size_t data_size)
{
	radio_test_modes_set_mode(m_testMode);
}
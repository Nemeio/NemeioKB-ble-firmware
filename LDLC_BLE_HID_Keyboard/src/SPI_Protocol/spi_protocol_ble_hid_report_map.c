#include "spi_protocol_ble_hid_report_map.h"
#include "app_ble_hid.h"

#include "ble.h"

static uint8_t m_report_id_max = 0;
static uint16_t m_curr_report_length = 0;

ret_code_t update_hid_report_desc_start(size_t data_size)
{
	m_report_id_max = 0;
	m_curr_report_length = 0;

	return app_ble_hid_report_map_init();
}

ret_code_t update_hid_report_desc_stop(size_t data_size)
{
	return app_ble_hid_report_map_send();
}

ret_code_t update_hid_report_desc_receive_start(size_t data_size)
{
	return app_ble_hid_report_map_data_init(data_size);
}

ret_code_t update_hid_report_desc_receive(uint32_t data_index, uint8_t data)
{
	return app_ble_hid_report_map_write(data_index, data);
}

ret_code_t update_hid_report_desc_get_report_count(uint32_t data_index, uint8_t data)
{
	if (data_index > sizeof(uint8_t)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_report_id_max = data;

	return app_ble_hid_set_input_report_id_max(m_report_id_max);
}

ret_code_t update_hid_report_desc_get_report_lengths(uint32_t data_index, uint8_t data)
{
	ret_code_t ret = NRF_SUCCESS;

	uint32_t report_id = data_index / sizeof(m_curr_report_length) + 1;

	if (report_id > m_report_id_max) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_curr_report_length |= (data << (sizeof(m_curr_report_length) - 1 -
					  (data_index % sizeof(m_curr_report_length))));

	if ((data_index % sizeof(m_curr_report_length)) == sizeof(m_curr_report_length) - 1) {
		ret = app_ble_hid_set_input_report_length(report_id, m_curr_report_length);

		// Reset the report length to parse the next one
		m_curr_report_length = 0;
	}

	return ret;
}

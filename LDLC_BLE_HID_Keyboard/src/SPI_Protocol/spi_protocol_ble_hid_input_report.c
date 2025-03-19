#include "spi_protocol_ble_hid_input_report.h"
#include "app_ble_hid.h"
#include "ble_management.h"

ret_code_t send_hid_input_report_start(size_t data_size)
{
	return app_ble_hid_input_report_init();
}

ret_code_t send_hid_input_report_stop(size_t data_size)
{
	return app_ble_hid_input_report_send();
}

ret_code_t send_hid_input_report_data_receive_start(size_t data_size)
{
	return app_ble_hid_input_report_data_init(data_size);
}

ret_code_t send_hid_input_report_data_receive(uint32_t data_index, uint8_t data)
{
	return app_ble_hid_input_report_write(data_index, data);
}

ret_code_t send_hid_input_report_id_receive(uint32_t data_index, uint8_t data)
{
	if (data_index > sizeof(uint8_t)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	return app_ble_hid_input_report_set_id(data);
}

#include "spi_protocol_ble_set_device_name.h"
#include "device_list.h"
#include "nrf_log.h"

static uint8_t deviceName[DEVICE_NAME_MAX_LENGTH];

ret_code_t set_device_name_start(size_t data_size)
{
	if (data_size > sizeof(deviceName)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	memset(deviceName, 0, sizeof(deviceName));

	return NRF_SUCCESS;
}

ret_code_t set_device_name_receive(uint32_t data_index, uint8_t data)
{
	if (data_index > sizeof(deviceName)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	deviceName[data_index] = data;

	return NRF_SUCCESS;
}

ret_code_t set_device_name_stop(size_t data_size)
{
	if (data_size > sizeof(deviceName)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	ble_gap_conn_sec_mode_t sec_mode;
	ret_code_t ret = NRF_ERROR_INTERNAL;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
	NRF_LOG_INFO("set edvicen name\r\n");

	ret = sd_ble_gap_device_name_set(&sec_mode, deviceName, data_size);

	return ret;
}
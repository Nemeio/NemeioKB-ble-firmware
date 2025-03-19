#include <string.h>
#include "spi_protocol_ble_set_device_properties.h"
#include "ble_device_properties.h"

static uint8_t buffer[25];

ret_code_t set_device_properties_protocol_version_start(size_t size)
{
	if (size >= sizeof(buffer) - sizeof('\0')) {
		return NRF_ERROR_DATA_SIZE;
	}

	memset(buffer, 0, sizeof(buffer));
	return NRF_SUCCESS;
}

ret_code_t set_device_properties_protocol_version_receive(uint32_t data_index, uint8_t data)
{
	if (data_index >= sizeof(buffer) - sizeof('\0')) {
		return NRF_ERROR_DATA_SIZE;
	}

	buffer[data_index] = data;

	return NRF_SUCCESS;
}

ret_code_t set_device_properties_protocol_version_stop(size_t size)
{
	UNUSED_PARAMETER(size);
	if (strnlen(buffer, sizeof(buffer)) >= sizeof(buffer)) {
		return NRF_ERROR_DATA_SIZE;
	}

	set_ble_software_revision(buffer);

	return NRF_SUCCESS;
}

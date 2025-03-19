#include "spi_protocol_ble_hid_radio_power.h"
#include "ble_management.h"

ret_code_t set_radio_power_data_receive(uint32_t data_index, uint8_t data)
{
	ret_code_t ret = NRF_SUCCESS;
	if (data) {
		radio_power_on();
	} else {
		ret = radio_power_off();
	}
	return ret;
}

#include "spi_protocol_ble_hid_hibernation.h"
#include "ble_management.h"

ret_code_t set_hibernation_start(size_t data_size)
{
	return NRF_SUCCESS;
}

ret_code_t set_hibernation_stop(size_t data_size)
{
	return NRF_SUCCESS;
}

ret_code_t set_hibernation_action(size_t data_size)
{
	sleep_mode_enter();
	return NRF_SUCCESS;
}

ret_code_t set_hibernation(size_t data_size)
{
	sleep_mode_enter();
	return NRF_SUCCESS;
}

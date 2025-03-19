#include "spi_protocol_dfu_cmd.h"
#include "nrf_power.h"
#include "app_error.h"
#include "nrf_bootloader_info.h"

void enter_bootloader_dfu()
{
#ifndef NEMEIO_BOOTLOADER
	sd_power_gpregret_set(0, BOOTLOADER_DFU_START);

	NVIC_SystemReset();

	APP_ERROR_CHECK_BOOL(false);
#endif
}

ret_code_t spi_protocol_start_dfu_stop(size_t data_size)
{
#ifndef NEMEIO_BOOTLOADER
	enter_bootloader_dfu();
#endif

	return NRF_SUCCESS;
}

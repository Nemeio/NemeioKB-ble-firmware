#include "nrf_dfu_commit_version.h"
#include "app_error.h"
#include "application_defs.h"
#include "nrf.h"
#include <string.h>
#include "nrf_log.h"

static firmware_version_t commitVersion;

static void commit_version()
{
	sd_power_gpregret_set(0, BOOTLOADER_DFU_COMMIT_VERSION);
	NVIC_SystemReset();
}

ret_code_t spi_protocol_dfu_commit_version_start(size_t data_size)
{
	memset(&commitVersion, 0, sizeof(commitVersion));
	return NRF_SUCCESS;
}

ret_code_t spi_protocol_dfu_commit_version_receive(uint32_t data_index, uint8_t data)
{
	if (data_index >= sizeof(commitVersion)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	((uint8_t *)&commitVersion)[data_index] = data;
	return NRF_SUCCESS;
}

ret_code_t spi_protocol_dfu_commit_version_stop(size_t data_size)
{
	if (data_size != sizeof(commitVersion)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	if (commitVersion.major == APP_MAJOR_NUMBER && commitVersion.minor == APP_BUILD_NUMBER) {
		/* TODO: Add a way to ensure version has been commited by bootloader */
		commit_version(); /* Reboot system */
	}

	return NRF_ERROR_INTERNAL;
}
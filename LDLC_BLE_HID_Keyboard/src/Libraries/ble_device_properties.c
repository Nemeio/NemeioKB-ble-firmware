#include <string.h>
#include "ble_device_properties.h"

static char software_revision[25] = "0.0";

ret_code_t set_ble_software_revision(const char *sw_rev)
{
	if (sw_rev == NULL ||
	    strnlen(sw_rev, sizeof(software_revision)) >= sizeof(software_revision)) {
		return NRF_ERROR_DATA_SIZE;
	}

	strncpy(software_revision, sw_rev, sizeof(software_revision) - sizeof('\0'));

	return NRF_SUCCESS;
}

const char *get_ble_software_revision()
{
	return software_revision;
}
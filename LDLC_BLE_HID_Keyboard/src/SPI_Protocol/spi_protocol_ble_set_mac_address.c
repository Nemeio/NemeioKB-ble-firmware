#include "spi_protocol_ble_set_mac_address.h"
#include "peer_manager.h"
#include "app_error.h"

#define IEEE_PUBLIC_MAC_ADDRESS

static uint8_t macAddress[6];

ret_code_t set_mac_address_start(size_t data_size)
{
	memset(macAddress, 0, sizeof(macAddress));

	return NRF_SUCCESS;
}

ret_code_t set_mac_address_receive(uint32_t data_index, uint8_t data)
{
	if (data_index > sizeof(macAddress)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	// Revert mac address for NRF
	macAddress[sizeof(macAddress) - 1 - data_index] = data;

	return NRF_SUCCESS;
}

ret_code_t set_mac_address_stop(size_t data_size)
{
	ret_code_t ret = NRF_ERROR_INVALID_DATA;
	if (data_size != sizeof(macAddress)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	ble_gap_addr_t gapAddr = { 0 };
	memcpy(gapAddr.addr, macAddress, sizeof(gapAddr.addr));

#ifdef IEEE_PUBLIC_MAC_ADDRESS
	gapAddr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;
	ret = pm_id_addr_set(&gapAddr);
#else
	/* For random static address, the most significant two bits must be equals to 1 */
	if ((gapAddr.addr[5] & 0xC0) == 0xC0) {
		gapAddr.addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
		ret = pm_id_addr_set(&gapAddr);
	}
#endif

	APP_ERROR_CHECK(ret);

	return ret;
}
#include "app_lesc.h"
#include "app_error.h"
#include "nrf_log.h"
#include "spi_protocol_numeric_comparison.h"
#include "spi_protocol_pairing_abort.h"

static uint16_t m_conn_handle_numcomp = BLE_CONN_HANDLE_INVALID;

/** @brief Function for accepting or rejecting a numeric comparison. */
static void app_lesc_num_comp_reply(uint16_t conn_handle, bool b_accept)
{
	uint8_t key_type;

	if (b_accept) {
		NRF_LOG_INFO("Numeric Match. Conn handle: %d", conn_handle);
		key_type = BLE_GAP_AUTH_KEY_TYPE_PASSKEY;
	} else {
		NRF_LOG_INFO("Numeric REJECT. Conn handle: %d", conn_handle);
		key_type = BLE_GAP_AUTH_KEY_TYPE_NONE;
	}

	sd_ble_gap_auth_key_reply(conn_handle, key_type, NULL);
}

ret_code_t app_lesc_start_num_comp(uint16_t conn_handle, const uint8_t passkey[BLE_GAP_PASSKEY_LEN],
				   size_t length)
{
	ret_code_t ret = NRF_SUCCESS;

	if (length != BLE_GAP_PASSKEY_LEN) {
		ret = NRF_ERROR_INVALID_PARAM;
	} else {
		m_conn_handle_numcomp = conn_handle;
		ret = spi_protocol_numeric_comparison_send_passkey(passkey, length);
	}

	return ret;
}

void app_lesc_end_num_comp(bool b_accept)
{
	app_lesc_num_comp_reply(m_conn_handle_numcomp, b_accept);

	m_conn_handle_numcomp = BLE_CONN_HANDLE_INVALID;
}

void app_lesc_abort_pairing()
{
	spi_protocol_pairing_abort();
}
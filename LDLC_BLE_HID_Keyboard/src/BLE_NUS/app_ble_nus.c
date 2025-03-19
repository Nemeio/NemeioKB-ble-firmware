#include "app_ble_nus.h"
#include "ble_nus.h"
#include "app_error.h"
#include "ble_management.h"
#include "nrf_ringbuf.h"
#include "spi_protocol_serial_data.h"
#include "app_trace.h"

#include "application_defs.h"

#define NUS_RINGBUF_SIZE 2048

BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);

NRF_RINGBUF_DEF(m_tx_ringbuf, NUS_RINGBUF_SIZE);
static bool m_conn_flag = false;

static void app_ble_nus_tx_try()
{
	uint8_t *p_tx_data;
	size_t tx_data_len = BLE_NUS_MAX_DATA_LEN;

	ret_code_t err_code = nrf_ringbuf_get(&m_tx_ringbuf, &p_tx_data, &tx_data_len, true);

	if (NRF_SUCCESS == err_code && tx_data_len != 0) {
		ASSERT(tx_data_len < UINT16_MAX);

		err_code = ble_nus_data_send(&m_nus, (uint8_t *)p_tx_data, (uint16_t *)&tx_data_len,
					     ble_man_get_conn_handle());

		if (NRF_ERROR_BUSY == err_code || NRF_ERROR_RESOURCES == err_code) {
			// Retry will happen when the current TX is completed
			tx_data_len = 0;
		}

		err_code = nrf_ringbuf_free(&m_tx_ringbuf, tx_data_len);
		ASSERT(NRF_SUCCESS == err_code);
	}
}

static void app_ble_nus_on_rx_data(const uint8_t *p_data, size_t len)
{
	ret_code_t err_code = spi_protocol_serial_data_send(p_data, len);
	if (err_code != NRF_ERROR_NO_MEM) {
		APP_ERROR_CHECK(err_code);
	}
#ifdef DEBUG
	else {
		ASSERT(false);
	}
#endif // DEBUG
}

static void app_ble_nus_on_tx_rdy()
{
	app_ble_nus_tx_try();
}

static void app_ble_nus_on_comm_started()
{
	m_conn_flag = true;
}

static void app_ble_nus_on_comm_stopped()
{
	m_conn_flag = false;
}

static void app_ble_nus_data_handler(ble_nus_evt_t *p_evt)
{
	switch (p_evt->type) {
	case BLE_NUS_EVT_RX_DATA:
		app_ble_nus_on_rx_data(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);
		break;
	case BLE_NUS_EVT_TX_RDY:
		app_ble_nus_on_tx_rdy();
		break;
	case BLE_NUS_EVT_COMM_STARTED:
		app_ble_nus_on_comm_started();
		break;
	case BLE_NUS_EVT_COMM_STOPPED:
		app_ble_nus_on_comm_stopped();
		break;
	}
}

ret_code_t app_ble_nus_send_data(const uint8_t *p_data, size_t length, size_t *ret_length)
{
	// We consider the tx operation a success if no central is connected
	ret_code_t err_code = NRF_SUCCESS;

	ASSERT(p_data);
	ASSERT(ret_length);

	*ret_length = length;

	if (m_conn_flag) {
		err_code = nrf_ringbuf_cpy_put(&m_tx_ringbuf, p_data, ret_length);

#ifdef DEBUG
		if (*ret_length != length) {
			ASSERT(false);
		}
#endif // DEBUG
		app_ble_nus_tx_try();
	}

	return err_code;
}

void app_ble_nus_init(void)
{
	ret_code_t err_code;
	ble_nus_init_t nus_init;

	nrf_ringbuf_init(&m_tx_ringbuf);

	memset(&nus_init, 0, sizeof(nus_init));
	nus_init.data_handler = app_ble_nus_data_handler;
	nus_init.nus_char_rd_sec = DEFAULT_CHAR_SECURITY_LEVEL;

	err_code = ble_nus_init(&m_nus, &nus_init);
	APP_ERROR_CHECK(err_code);
}
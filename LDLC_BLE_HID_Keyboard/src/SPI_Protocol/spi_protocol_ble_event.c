#include "spi_protocol_ble_event.h"
#include "spi_protocol.h"
#include "spi_protocol_tx.h"

enum protocol_ble_evt_t {
	PROTOCOL_BLE_EVT_ADV_STARTED = 0,
	PROTOCOL_BLE_EVT_ADV_STOPPED = 1,
	PROTOCOL_BLE_EVT_CONNECTED = 2,
	PROTOCOL_BLE_EVT_DISCONNECTED = 3
};

static ret_code_t spi_protocol_ble_event_send(enum protocol_ble_evt_t evt)
{
	uint8_t evt_value = evt;
	return spi_protocol_tx_set_data_to_send(&evt_value, sizeof(evt_value), BLE_EVENT);
}

ret_code_t spi_protocol_ble_event_advertising_started()
{
	return spi_protocol_ble_event_send(PROTOCOL_BLE_EVT_ADV_STARTED);
}

ret_code_t spi_protocol_ble_event_advertising_stopped()
{
	return spi_protocol_ble_event_send(PROTOCOL_BLE_EVT_ADV_STOPPED);
}

ret_code_t spi_protocol_ble_event_connected()
{
	return spi_protocol_ble_event_send(PROTOCOL_BLE_EVT_CONNECTED);
}

ret_code_t spi_protocol_ble_event_disconnected()
{
	return spi_protocol_ble_event_send(PROTOCOL_BLE_EVT_DISCONNECTED);
}
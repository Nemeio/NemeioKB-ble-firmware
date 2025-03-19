#include "ble_event_notification.h"
#include "spi_protocol_ble_event.h"

ret_code_t ble_event_notification_advertising_started()
{
	return spi_protocol_ble_event_advertising_started();
}

ret_code_t ble_event_notification_advertising_stopped()
{
	return spi_protocol_ble_event_advertising_stopped();
}

ret_code_t ble_event_notification_connected()
{
	return spi_protocol_ble_event_connected();
}

ret_code_t ble_event_notification_disconnected()
{
	return spi_protocol_ble_event_disconnected();
}
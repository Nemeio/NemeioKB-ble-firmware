#ifndef SPI_PROTOCOL_BLE_EVENT_H
#define SPI_PROTOCOL_BLE_EVENT_H

#include "sdk_common.h"

ret_code_t spi_protocol_ble_event_advertising_started();
ret_code_t spi_protocol_ble_event_advertising_stopped();
ret_code_t spi_protocol_ble_event_connected();
ret_code_t spi_protocol_ble_event_disconnected();

#endif // SPI_PROTOCOL_BLE_EVENT_H
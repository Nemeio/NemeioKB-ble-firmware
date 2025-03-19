#ifndef BLE_EVENT_NOTIFICATION_H
#define BLE_EVENT_NOTIFICATION_H

#include "sdk_common.h"

ret_code_t ble_event_notification_advertising_started();
ret_code_t ble_event_notification_advertising_stopped();
ret_code_t ble_event_notification_connected();
ret_code_t ble_event_notification_disconnected();

#endif // BLE_EVENT_NOTIFICATION_H
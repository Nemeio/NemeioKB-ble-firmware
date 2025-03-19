#ifndef APP_BLE_HID_H
#define APP_BLE_HID_H

#include "ble_hids.h"

ret_code_t app_ble_hid_input_report_init();

ret_code_t app_ble_hid_input_report_data_init(uint32_t report_length);

ret_code_t app_ble_hid_input_report_set_id(uint8_t report_id);

ret_code_t app_ble_hid_input_report_write(uint32_t data_index, uint8_t data);

ret_code_t app_ble_hid_input_report_send();

ret_code_t app_ble_hid_report_map_init();

ret_code_t app_ble_hid_report_map_data_init(uint32_t report_map_length);

ret_code_t app_ble_hid_report_map_write(uint32_t data_index, uint8_t data);

ret_code_t app_ble_hid_report_map_send();

ret_code_t app_ble_hid_set_input_report_id_max(uint8_t report_id_max);

ret_code_t app_ble_hid_set_input_report_length(uint8_t report_id, uint16_t report_length);

#endif // APP_BLE_HID_H
#ifndef SPI_PROTOCOL_BLE_HID_REPORT_MAP_H
#define SPI_PROTOCOL_BLE_HID_REPORT_MAP_H

#include "sdk_common.h"

ret_code_t update_hid_report_desc_start(size_t data_size);

ret_code_t update_hid_report_desc_stop(size_t data_size);

ret_code_t update_hid_report_desc_receive_start(size_t data_size);

ret_code_t update_hid_report_desc_receive(uint32_t data_index, uint8_t data);

ret_code_t update_hid_report_desc_get_report_count(uint32_t data_index, uint8_t data);

ret_code_t update_hid_report_desc_get_report_lengths(uint32_t data_index, uint8_t data);

#endif // SPI_PROTOCOL_BLE_HID_REPORT_MAP_H

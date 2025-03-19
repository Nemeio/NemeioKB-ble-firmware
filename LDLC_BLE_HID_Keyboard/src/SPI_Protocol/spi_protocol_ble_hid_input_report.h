#ifndef SPI_PROTOCOL_BLE_HID_INPUT_REPORT_H
#define SPI_PROTOCOL_BLE_HID_INPUT_REPORT_H

#include "sdk_common.h"

ret_code_t send_hid_input_report_start(size_t data_size);

ret_code_t send_hid_input_report_stop(size_t data_size);

ret_code_t send_hid_input_report_data_receive(uint32_t data_index, uint8_t data);

ret_code_t send_hid_input_report_data_receive_start(size_t data_size);

ret_code_t send_hid_input_report_id_receive(uint32_t data_index, uint8_t data);

#endif // SPI_PROTOCOL_BLE_HID_INPUT_REPORT_H

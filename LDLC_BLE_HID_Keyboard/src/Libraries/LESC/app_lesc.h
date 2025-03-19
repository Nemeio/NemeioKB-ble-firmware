#ifndef APP_LESC_H
#define APP_LESC_H

#include "sdk_common.h"
#include "ble_gap.h"

ret_code_t app_lesc_start_num_comp(uint16_t conn_handle, const uint8_t passkey[BLE_GAP_PASSKEY_LEN],
				   size_t length);

void app_lesc_end_num_comp(bool b_accept);

void app_lesc_abort_pairing();

#endif // APP_LESC_H
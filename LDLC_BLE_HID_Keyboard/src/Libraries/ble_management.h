#ifndef BLE_MANAGEMENT_H
#define BLE_MANAGEMENT_H

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_assert.h"
#include "app_error.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advertising.h"
#include "ble_advdata.h"
#include "ble_hids.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "bsp.h"
#include "sensorsim.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "bsp_btn_ble.h"
#include "app_button.h"
#include "fds.h"
#include "nrf_fstorage.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "app_ble_hid.h"
#include "spi_protocol.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
void log_init(void);

void power_management_init(void);

void idle_state_handle(void);

void sleep_mode_enter(void);

void timers_init(void);

void app_bsp_init();

void ble_stack_init(void);

void scheduler_init(void);

void gap_params_init(void);

void gatt_init(void);

void advertising_init(void);

void services_init(void);

void conn_params_init(void);

void peer_manager_init(void);

ret_code_t radio_power_off();

void radio_power_on();

void delete_bonds();

void delete_whitelist();

void set_whitelist_advertising(bool enable);

ret_code_t ble_man_start_advertising();

ret_code_t ble_man_stop_advertising();

uint16_t ble_man_get_conn_handle();

void ble_man_set_whitelist(bool enable);

void ble_man_set_directed(bool enable);
#endif //BLE_MANAGEMENT_H
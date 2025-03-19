#ifndef BLE_DEVICE_PROPERTIES_H
#define BLE_DEVICE_PROPERTIES_H

#include "sdk_common.h"

ret_code_t set_ble_software_revision(const char *software_revision);
const char *get_ble_software_revision();

#endif //BLE_DEVICE_PROPERTIES_H
#ifndef SPI_PROTOCOL_FACTORY_RESET_H
#define SPI_PROTOCOL_FACTORY_RESET_H

#include "sdk_common.h"

typedef enum _factory_reset_cmd__err_code_t {
	FACTORY_RESET_SUCCESS = 0,
	FACTORY_RESET_UNKNOWN
} factory_reset_cmd_err_code_t;

ret_code_t factory_reset_stop(size_t data_size);

#endif //SPI_PROTOCOL_BLE_HID_PEER_MANAGEMENT_H

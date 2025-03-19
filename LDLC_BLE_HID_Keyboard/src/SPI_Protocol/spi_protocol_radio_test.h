#ifndef SPI_PROTOCOL_RADIO_TEST_H
#define SPI_PROTOCOL_RADIO_TEST_H

#include "sdk_common.h"

ret_code_t set_radio_test_mode_receive(uint32_t data_index, uint8_t data);

ret_code_t set_radio_test_stop(size_t data_size);

#endif // SPI_PROTOCOL_RADIO_TEST_H
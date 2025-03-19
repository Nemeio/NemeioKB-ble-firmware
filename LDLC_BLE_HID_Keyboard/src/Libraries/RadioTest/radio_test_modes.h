#ifndef RADIO_TEST_MODES_H
#define RADIO_TEST_MODES_H

#include <stdbool.h>

enum TestMode_t {
	TEST_MODE_DISABLED =
		0, // unhandled by nRF firmware, MCU will be set in reset state by the master
	TEST_MODE_TX_LOWEST_CHANNEL,
	TEST_MODE_TX_MIDDLE_CHANNEL,
	TEST_MODE_TX_HIGHEST_CHANNEL,
	TEST_MODE_RX,
	TEST_MODE_MAX
};

void radio_test_modes_set_mode(enum TestMode_t mode);

#endif // RADIO_TEST_MODES_H
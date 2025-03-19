#ifndef RADIO_TEST_H
#define RADIO_TEST_H

#include <stdint.h>

#include "nrf.h"

void radio_test_modulated_tx_carrier(uint8_t tx_power, uint8_t mode, uint8_t channel);
void radio_test_rx(uint8_t mode, uint8_t channel);
void radio_test_disable(void);
void radio_test_init(void);

#endif // RADIO_TEST_H
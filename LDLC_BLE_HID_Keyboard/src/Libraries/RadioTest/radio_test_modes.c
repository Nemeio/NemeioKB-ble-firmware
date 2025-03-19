#include "radio_test_modes.h"

#include "ble_management.h"
#include "nrf_sdh.h"
#include "radio_test.h"
#include "nrf_drv_clock.h"

#define TX_POWER RADIO_TXPOWER_TXPOWER_Pos4dBm
#define RADIO_MODE RADIO_MODE_MODE_Ble_1Mbit

#define LOWEST_CHANNEL 2
#define MIDDLE_CHANNEL 40
#define HIGHEST_CHANNEL 80

static bool m_initialized = false;

static void disable_sd(void)
{
	if (!nrf_sdh_is_enabled()) {
		return;
	}

	ble_man_stop_advertising();

	APP_ERROR_CHECK(nrf_sdh_disable_request());
	while (nrf_sdh_is_enabled())
		;
}

static void clock_init(void)
{
	ret_code_t err_code = nrf_drv_clock_init();
	ASSERT((err_code == NRF_SUCCESS) || (err_code == NRF_ERROR_MODULE_ALREADY_INITIALIZED));

	nrf_drv_clock_hfclk_request(NULL);
	while (!nrf_drv_clock_hfclk_is_running()) {
		// spin lock
	}

	nrf_drv_clock_lfclk_request(NULL);
	while (!nrf_drv_clock_lfclk_is_running()) {
		// spin lock
	}
}

static void init_test(void)
{
	if (!m_initialized) {
		// SoftDevice must be disabled to be able to control the radio
		disable_sd();

		// The HFCLK must be re-enabled
		clock_init();

		radio_test_init();

		m_initialized = true;
	}
}

static void start_tx(uint8_t channel)
{
	init_test();
	radio_test_modulated_tx_carrier(TX_POWER, RADIO_MODE, channel);
}

static void start_tx_lowest_channel()
{
	start_tx(LOWEST_CHANNEL);
}

static void start_tx_middle_channel()
{
	start_tx(MIDDLE_CHANNEL);
}

static void start_tx_highest_channel()
{
	start_tx(HIGHEST_CHANNEL);
}

static void start_rx()
{
	init_test();
	radio_test_rx(RADIO_MODE, MIDDLE_CHANNEL);
}

void radio_test_modes_set_mode(enum TestMode_t mode)
{
	switch (mode) {
	case TEST_MODE_TX_LOWEST_CHANNEL:
		start_tx_lowest_channel();
		break;
	case TEST_MODE_TX_MIDDLE_CHANNEL:
		start_tx_middle_channel();
		break;
	case TEST_MODE_TX_HIGHEST_CHANNEL:
		start_tx_highest_channel();
		break;
	case TEST_MODE_RX:
		start_rx();
		break;
	default:
		break;
	}
}
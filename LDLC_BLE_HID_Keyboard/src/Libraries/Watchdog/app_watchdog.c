#include "app_watchdog.h"
#include "app_timer.h"
#include "app_scheduler.h"
#include "nrfx_wdt.h"
#include "nrf_log_ctrl.h"

APP_TIMER_DEF(m_watchdog_timer_id);

#define WATCHDOG_REFRESH_INTERVAL APP_TIMER_TICKS(10000)

#define WATCHDOG_MARGIN_TICKS APP_TIMER_TICKS(100)
#define WATCHDOG_MIN_REFRESH_TICKS APP_TIMER_TICKS(100)

static void wdt_feed(void)
{
	/* Find the enabled channels and refresh them */
	if (nrf_wdt_started()) {
		for (nrf_wdt_rr_register_t i = NRF_WDT_RR0; i < NRF_WDT_RR7; i++) {
			if (nrf_wdt_reload_request_is_enabled(i)) {
				nrf_wdt_reload_request_set(i);
			}
		}
	}
}

static void app_watchog_refresh_scheduled_exec(void *p_event_data, uint16_t event_size)
{
	UNUSED_PARAMETER(p_event_data);
	UNUSED_PARAMETER(event_size);

	wdt_feed();
}

static void app_watchdog_timeout_handler(void *p_context)
{
	UNUSED_PARAMETER(p_context);

	app_sched_event_put(NULL, 0, app_watchog_refresh_scheduled_exec);
}

void WDT_IRQHandler(void)
{
	nrf_wdt_event_clear(NRF_WDT_EVENT_TIMEOUT);
	NRF_LOG_FINAL_FLUSH();
}

void app_watchdog_init(void)
{
	/* Watchdog should have been started by bootloader so it is not possible to use the nrfx driver for WDT
     * because of the need of channel allocation.
     * Use the same method as the secure bootloader for automated feeding of the watchdog. */
	if (nrf_wdt_started()) {
		ret_code_t err_code =
			app_timer_create(&m_watchdog_timer_id, APP_TIMER_MODE_REPEATED,
					 app_watchdog_timeout_handler);
		APP_ERROR_CHECK(err_code);

		uint32_t wdt_ticks = nrf_wdt_reload_value_get();

		uint32_t reduced_timeout_ticks =
			MAX((int32_t)wdt_ticks - WATCHDOG_MARGIN_TICKS, WATCHDOG_MIN_REFRESH_TICKS);

		wdt_feed();

		err_code = app_timer_start(m_watchdog_timer_id, reduced_timeout_ticks, NULL);
		APP_ERROR_CHECK(err_code);

		NVIC_EnableIRQ(WDT_IRQn);
	}
}
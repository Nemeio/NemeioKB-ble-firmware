#include "ble_management.h"
#include "app_trace.h"
#include "device_list.h"
#include "peer_manager_handler.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "app_ble_nus.h"
#include "nrf_ble_lesc.h"
#include "app_lesc.h"
#include "ble_event_notification.h"
#include "nrf_log_default_backends.h"

#include "application_defs.h"
#include "ble_device_properties.h"

#define DEVICE_NAME                                                                                \
	"Nemeio Keypad" /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                                                                          \
	"Groupe LDLC" /**< Manufacturer. Will be passed to Device Information Service. */

#define APP_BLE_OBSERVER_PRIO                                                                      \
	3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1 /**< A tag identifying the SoftDevice BLE configuration. */

#define PNP_ID_VENDOR_ID_SOURCE 0x02 /**< Vendor ID Source. */
#define PNP_ID_VENDOR_ID 0x3600 /**< Vendor ID. */
#define PNP_ID_PRODUCT_ID 0x0002 /**< Product ID. */
#define PNP_ID_PRODUCT_VERSION 0x0001 /**< Product Version. */

#define APP_ADV_FAST_INTERVAL                                                                      \
	0x0028 /**< Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.). */
#define APP_ADV_SLOW_INTERVAL                                                                      \
	0x0C80 /**< Slow advertising interval (in units of 0.625 ms. This value corrsponds to 2 seconds). */
#define APP_ADV_FAST_TIMEOUT 18000 /**< The duration of the fast advertising period (in seconds). */
#define APP_ADV_SLOW_TIMEOUT 18000 /**< The duration of the slow advertising period (in seconds). */

/*lint -emacro(524, MIN_CONN_INTERVAL) // Loss of precision */
#define MIN_CONN_INTERVAL                                                                          \
	MSEC_TO_UNITS(7.5, UNIT_1_25_MS) /**< Minimum connection interval (7.5 ms) */
#define MAX_CONN_INTERVAL                                                                          \
	MSEC_TO_UNITS(30, UNIT_1_25_MS) /**< Maximum connection interval (30 ms). */
#define SLAVE_LATENCY 0 /**< Slave latency. */
#define CONN_SUP_TIMEOUT                                                                           \
	MSEC_TO_UNITS(430, UNIT_10_MS) /**< Connection supervisory timeout (430 ms). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY                                                             \
	APP_TIMER_TICKS(                                                                           \
		5000) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY                                                              \
	APP_TIMER_TICKS(                                                                           \
		30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT                                                               \
	3 /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND 1 /**< Perform bonding. */
#define SEC_PARAM_MITM 1 /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC 1 /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS 0 /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES BLE_GAP_IO_CAPS_KEYBOARD_DISPLAY /**< No I/O capabilities. */
#define SEC_PARAM_OOB 0 /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE 7 /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE 16 /**< Maximum encryption key size. */

#define OUTPUT_REPORT_INDEX 0 /**< Index of Output Report. */
#define OUTPUT_REPORT_MAX_LEN 1 /**< Maximum length of Output Report. */
#define INPUT_REPORT_KEYS_INDEX 0 /**< Index of Input Report. */
#define OUTPUT_REPORT_BIT_MASK_CAPS_LOCK                                                           \
	0x02 /**< CAPS LOCK bit in Output Report (based on 'LED Page (0x08)' of the Universal Serial Bus HID Usage Tables). */
#define INPUT_REP_REF_ID 0 /**< Id of reference to Keyboard Input Report. */
#define OUTPUT_REP_REF_ID 0 /**< Id of reference to Keyboard Output Report. */
#define FEATURE_REP_REF_ID 0 /**< ID of reference to Keyboard Feature Report. */
#define FEATURE_REPORT_MAX_LEN 2 /**< Maximum length of Feature Report. */
#define FEATURE_REPORT_INDEX 0 /**< Index of Feature Report. */

#define APP_FEATURE_NOT_SUPPORTED                                                                  \
	BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2 /**< Reply when unsupported features are requested. */

#define MAX_BUFFER_ENTRIES 5 /**< Number of elements that can be enqueued */

#define BASE_USB_HID_SPEC_VERSION                                                                  \
	0x0101 /**< Version number of base USB HID Specification implemented by this application. */

#define INPUT_REPORT_KEYS_MAX_LEN 8 /**< Maximum length of the Input Report characteristic. */

#define DEAD_BEEF                                                                                  \
	0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define SCHED_MAX_EVENT_DATA_SIZE                                                                  \
	MAX(APP_TIMER_SCHED_EVENT_DATA_SIZE,                                                       \
	    MAX(SPI_PROTOCOL_EVENT_DATA_SIZE,                                                      \
		DEVICE_LIST_EVENT_DATA_SIZE)) /**< Maximum size of scheduler events. */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                                                                           \
	20 /**< Maximum number of events in the scheduler queue. More is needed in case of Serialization. */
#else
#define SCHED_QUEUE_SIZE 20 /**< Maximum number of events in the scheduler queue. */
#endif

#define MAX_KEYS_IN_ONE_REPORT                                                                     \
	(INPUT_REPORT_KEYS_MAX_LEN -                                                               \
	 SCAN_CODE_POS) /**< Maximum number of key presses that can be sent in one Input Report. */
#define NUS_SERVICE_UUID_TYPE                                                                      \
	BLE_UUID_TYPE_VENDOR_BEGIN /**< UUID type for the Nordic UART Service (vendor specific). */

NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr); /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising); /**< Advertising module instance. */

#define DEFAULT_ADVERTISING_INIT_OPTIONS                                                           \
	{ .ble_adv_whitelist_enabled = false,                                                      \
	  .ble_adv_directed_high_duty_enabled = false,                                             \
	  .ble_adv_directed_enabled = false,                                                       \
	  .ble_adv_directed_interval = 0,                                                          \
	  .ble_adv_directed_timeout = 0,                                                           \
	  .ble_adv_fast_enabled = true,                                                            \
	  .ble_adv_fast_interval = APP_ADV_FAST_INTERVAL,                                          \
	  .ble_adv_fast_timeout = APP_ADV_FAST_TIMEOUT,                                            \
	  .ble_adv_slow_enabled = false,                                                           \
	  .ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL,                                          \
	  .ble_adv_slow_timeout = APP_ADV_SLOW_TIMEOUT };

static ble_adv_modes_config_t advertising_init_options = DEFAULT_ADVERTISING_INIT_OPTIONS;

static bool m_b_erase_bonds = false;

/** Abstracts buffer element */
typedef struct hid_key_buffer {
	uint8_t data_offset; /**< Max Data that can be buffered for all entries */
	uint8_t data_len; /**< Total length of data */
	uint8_t *p_data; /**< Scanned key pattern */
	ble_hids_t *p_instance; /**< Identifies peer and service instance */
} buffer_entry_t;

STATIC_ASSERT(sizeof(buffer_entry_t) % 4 == 0);

/** Circular buffer list */
typedef struct {
	buffer_entry_t buffer
		[MAX_BUFFER_ENTRIES]; /**< Maximum number of entries that can enqueued in the list */
	uint8_t rp; /**< Index to the read location */
	uint8_t wp; /**< Index to write location */
	uint8_t count; /**< Number of elements in the list */
} buffer_list_t;

STATIC_ASSERT(sizeof(buffer_list_t) % 4 == 0);

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

static pm_peer_id_t m_peer_id; /**< Device reference handle to the current bonded central. */

static ble_uuid_t m_adv_uuids[] = { { BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE, BLE_UUID_TYPE_BLE },
				    { BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE } };

typedef enum { INVALID_REQ, GET_DEVICE_NAME_REQ } request_type_t;

static request_type_t m_current_request = INVALID_REQ;

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
	app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Fetch the list of peer manager peer IDs.
 *
 * @param[inout] p_peers   The buffer where to store the list of peer IDs.
 * @param[inout] p_size    In: The size of the @p p_peers buffer.
 *                         Out: The number of peers copied in the buffer.
 */
void peer_list_get(pm_peer_id_t *p_peers, uint32_t *p_size)
{
	pm_peer_id_t peer_id;
	uint32_t peers_to_copy;

	peers_to_copy = (*p_size < BLE_GAP_WHITELIST_ADDR_MAX_COUNT) ?
				      *p_size :
				      BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

	peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
	*p_size = 0;

	while ((peer_id != PM_PEER_ID_INVALID) && (peers_to_copy--)) {
		p_peers[(*p_size)++] = peer_id;
		peer_id = pm_next_peer_id_get(peer_id);
	}
}

/**@brief Function for setting filtered device identities.
 *
 * @param[in] skip  Filter passed to @ref pm_peer_id_list.
 */
static void identities_set(pm_peer_id_list_skip_t skip)
{
	pm_peer_id_t peer_ids[BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT];
	uint32_t peer_id_count = BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT;

	ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip);
	APP_ERROR_CHECK(err_code);

	err_code = pm_device_identities_list_set(peer_ids, peer_id_count);
	APP_ERROR_CHECK(err_code);
}

/**@brief Clear bond information from persistent storage.
 */
void delete_bonds(void)
{
	ret_code_t err_code;

	NRF_LOG_INFO("Erase bonds!\r\n");

	err_code = pm_peers_delete();
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for setting filtered whitelist.
 *
 * @param[in] skip  Filter passed to @ref pm_peer_id_list.
 */
static void whitelist_set(pm_peer_id_list_skip_t skip)
{
	pm_peer_id_t peer_ids[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
	uint32_t peer_id_count = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

	ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip);
	APP_ERROR_CHECK(err_code);

	NRF_LOG_INFO("\tm_whitelist_peer_cnt %d, MAX_PEERS_WLIST %d", peer_id_count + 1,
		     BLE_GAP_WHITELIST_ADDR_MAX_COUNT);

	err_code = pm_whitelist_set(peer_ids, peer_id_count);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting advertising.
 */
void bonds_init(bool erase_bonds)
{
	if (erase_bonds == true) {
		delete_bonds();
		// Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
	} else {
		whitelist_set(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR);
	}
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
void pm_evt_handler(pm_evt_t const *p_evt)
{
	ret_code_t err_code;

	pm_handler_on_pm_evt(p_evt);
	pm_handler_flash_clean(p_evt);

	switch (p_evt->evt_id) {
	case PM_EVT_PEERS_DELETE_SUCCEEDED:
		bonds_init(false);
		break;

	case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
		if (p_evt->params.peer_data_update_succeeded.flash_changed &&
		    (p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING)) {
			NRF_LOG_INFO("New Bond, add the peer to the whitelist if possible");
			// Note: You should check on what kind of white list policy your application should use.

			whitelist_set(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR);
		}
		break;
	case PM_EVT_CONN_SEC_SUCCEEDED: {
		ble_uuid_t p_uuid = { .uuid = BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME,
				      .type = BLE_UUID_TYPE_BLE };
		ble_gattc_handle_range_t p_handle_range = { .start_handle = 0x0001,
							    .end_handle = 0xFFFF };

		m_current_request = GET_DEVICE_NAME_REQ;
		err_code = sd_ble_gattc_char_value_by_uuid_read(p_evt->conn_handle, &p_uuid,
								&p_handle_range);
		if (NRF_SUCCESS != err_code) {
			m_current_request = INVALID_REQ;
		}
	} break;

	case PM_EVT_CONN_SEC_CONFIG_REQ: {
		pm_conn_sec_config_t conn_sec_config = { .allow_repairing = true };
		pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
	} break;

	default:
		break;
	}

	device_list_pm_evt_handler(p_evt);
}

/**@brief Function for handling advertising errors.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
void ble_advertising_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
void timers_init(void)
{
	ret_code_t err_code;

	err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void)
{
	ret_code_t err_code;
	ble_gap_conn_params_t gap_conn_params;
	ble_gap_conn_sec_mode_t sec_mode;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

	/* Set default device name */
	err_code = sd_ble_gap_device_name_set(&sec_mode, DEVICE_NAME, strlen(DEVICE_NAME));
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HID_KEYBOARD);
	APP_ERROR_CHECK(err_code);

	memset(&gap_conn_params, 0, sizeof(gap_conn_params));

	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the GATT module.
 */
void gatt_init(void)
{
	ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Queued Write Module.
 */
static void qwr_init(void)
{
	ret_code_t err_code;
	nrf_ble_qwr_init_t qwr_init_obj = { 0 };

	qwr_init_obj.error_handler = nrf_qwr_error_handler;

	err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init_obj);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing Device Information Service.
 */
void dis_init(void)
{
	ret_code_t err_code;
	ble_dis_init_t dis_init_obj;
	ble_dis_pnp_id_t pnp_id;

	pnp_id.vendor_id_source = PNP_ID_VENDOR_ID_SOURCE;
	pnp_id.vendor_id = PNP_ID_VENDOR_ID;
	pnp_id.product_id = PNP_ID_PRODUCT_ID;
	pnp_id.product_version = PNP_ID_PRODUCT_VERSION;

	memset(&dis_init_obj, 0, sizeof(dis_init_obj));

	ble_srv_ascii_to_utf8(&dis_init_obj.manufact_name_str, MANUFACTURER_NAME);
	dis_init_obj.p_pnp_id = &pnp_id;

	ble_srv_ascii_to_utf8(&dis_init_obj.sw_rev_str, get_ble_software_revision());

	dis_init_obj.dis_char_rd_sec = DEFAULT_CHAR_SECURITY_LEVEL;

	err_code = ble_dis_init(&dis_init_obj);
	APP_ERROR_CHECK(err_code);
}

static void nus_init()
{
	app_ble_nus_init();
}

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void)
{
	qwr_init();
	dis_init();
	nus_init();
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void conn_params_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
{
	ret_code_t err_code;
	ble_conn_params_init_t cp_init;

	memset(&cp_init, 0, sizeof(cp_init));

	cp_init.p_conn_params = NULL;
	cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
	cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
	cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
	cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
	cp_init.disconnect_on_fail = false;
	cp_init.evt_handler = NULL;
	cp_init.error_handler = conn_params_error_handler;

	err_code = ble_conn_params_init(&cp_init);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void sleep_mode_enter(void)
{
	ret_code_t err_code;

	err_code = bsp_indication_set(BSP_INDICATE_IDLE);
	APP_ERROR_CHECK(err_code);

	// Prepare wakeup buttons.
	err_code = bsp_btn_ble_sleep_mode_prepare();
	APP_ERROR_CHECK(err_code);

	// Go to system-off mode (this function will not return; wakeup will cause a reset).
	err_code = sd_power_system_off();
	for (;;) {
	}
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
	ret_code_t err_code;

	switch (ble_adv_evt) {
	case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:
		NRF_LOG_INFO("High Duty Directed advertising.");

		err_code = ble_event_notification_advertising_started();
		APP_ERROR_CHECK(err_code);

		err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_ADV_EVT_DIRECTED:
		NRF_LOG_INFO("Directed advertising.");

		err_code = ble_event_notification_advertising_started();
		APP_ERROR_CHECK(err_code);

		err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_ADV_EVT_FAST:
		NRF_LOG_INFO("Fast advertising.");

		err_code = ble_event_notification_advertising_started();
		APP_ERROR_CHECK(err_code);

		err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_ADV_EVT_SLOW:
		NRF_LOG_INFO("Slow advertising.");

		err_code = ble_event_notification_advertising_started();
		APP_ERROR_CHECK(err_code);

		err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_ADV_EVT_FAST_WHITELIST:
		NRF_LOG_INFO("Fast advertising with whitelist.");

		err_code = ble_event_notification_advertising_started();
		APP_ERROR_CHECK(err_code);

		err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_ADV_EVT_SLOW_WHITELIST:
		NRF_LOG_INFO("Slow advertising with whitelist.");

		err_code = ble_event_notification_advertising_started();
		APP_ERROR_CHECK(err_code);

		err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_ADV_EVT_IDLE:
		err_code = ble_event_notification_advertising_stopped();
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_ADV_EVT_WHITELIST_REQUEST: {
		ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
		ble_gap_irk_t whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
		uint32_t addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
		uint32_t irk_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

		err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt, whitelist_irks, &irk_cnt);
		APP_ERROR_CHECK(err_code);
		NRF_LOG_DEBUG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist",
			      addr_cnt, irk_cnt);

		// Set the correct identities list (no excluding peers with no Central Address Resolution).
		identities_set(PM_PEER_ID_LIST_SKIP_NO_IRK);

		// Apply the whitelist.
		err_code = ble_advertising_whitelist_reply(&m_advertising, whitelist_addrs,
							   addr_cnt, whitelist_irks, irk_cnt);
		APP_ERROR_CHECK(err_code);
	} break; //BLE_ADV_EVT_WHITELIST_REQUEST

	case BLE_ADV_EVT_PEER_ADDR_REQUEST: {
		pm_peer_data_bonding_t peer_bonding_data;

		// Only Give peer address if we have a handle to the bonded peer.
		if (m_peer_id != PM_PEER_ID_INVALID) {
			err_code = pm_peer_data_bonding_load(m_peer_id, &peer_bonding_data);
			if (err_code != NRF_ERROR_NOT_FOUND) {
				APP_ERROR_CHECK(err_code);

				// Manipulate identities to exclude peers with no Central Address Resolution.
				identities_set(PM_PEER_ID_LIST_SKIP_ALL);

				ble_gap_addr_t *p_peer_addr =
					&(peer_bonding_data.peer_ble_id.id_addr_info);
				err_code = ble_advertising_peer_addr_reply(&m_advertising,
									   p_peer_addr);
				APP_ERROR_CHECK(err_code);
			}
		}
	} break; //BLE_ADV_EVT_PEER_ADDR_REQUEST

	default:
		break;
	}
}

static void renegociate_slave_latency(ble_gap_evt_connected_t const *p_ble_evt_connected)
{
	ble_gap_conn_params_t p_conn_params;

	ASSERT(p_ble_evt_connected != NULL);

	p_conn_params.min_conn_interval = p_ble_evt_connected->conn_params.min_conn_interval;
	p_conn_params.max_conn_interval = p_ble_evt_connected->conn_params.max_conn_interval;
	p_conn_params.slave_latency = SLAVE_LATENCY;
	p_conn_params.conn_sup_timeout = p_ble_evt_connected->conn_params.conn_sup_timeout;

	ret_code_t err_code = sd_ble_gap_conn_param_update(m_conn_handle, &p_conn_params);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
	static device_info_t device_info = { 0 };
	static int offset = 0;
	static char *pBuff = device_info.name_buff;
	ret_code_t err_code;

	switch (p_ble_evt->header.evt_id) {
	case BLE_GAP_EVT_CONNECTED:
		NRF_LOG_DEBUG("Connected\r\n");

		err_code = ble_event_notification_connected();
		APP_ERROR_CHECK(err_code);

		err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
		APP_ERROR_CHECK(err_code);

		m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

		err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
		APP_ERROR_CHECK(err_code);

		// Slave latency may have been changed by the central, try to
		// renegociate its value
		renegociate_slave_latency(&p_ble_evt->evt.gap_evt.params.connected);

		break; // BLE_GAP_EVT_CONNECTED

	case BLE_GAP_EVT_DISCONNECTED:
		NRF_LOG_DEBUG("Disconnected\r\n");

		m_conn_handle = BLE_CONN_HANDLE_INVALID;

		err_code = ble_event_notification_disconnected();
		APP_ERROR_CHECK(err_code);

		// disabling alert 3. signal - used for capslock ON
		err_code = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
		APP_ERROR_CHECK(err_code);
		break; // BLE_GAP_EVT_DISCONNECTED

	case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
		NRF_LOG_DEBUG("PHY update request.");
		ble_gap_phys_t const phys = {
			.rx_phys = BLE_GAP_PHY_AUTO,
			.tx_phys = BLE_GAP_PHY_AUTO,
		};
		err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
		APP_ERROR_CHECK(err_code);
	} break;

	case BLE_GATTC_EVT_TIMEOUT:
		// Disconnect on GATT Client timeout event.
		NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
						 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		APP_ERROR_CHECK(err_code);
		break; // BLE_GATTC_EVT_TIMEOUT

	case BLE_GATTS_EVT_TIMEOUT:
		// Disconnect on GATT Server timeout event.
		NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
						 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		APP_ERROR_CHECK(err_code);
		break; // BLE_GATTS_EVT_TIMEOUT

	case BLE_GATTS_EVT_SYS_ATTR_MISSING:
		// No system attributes have been stored.
		err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
		APP_ERROR_CHECK(err_code);
		break;

	case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP: {
		app_trace_log("BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP\n");
		ble_gattc_evt_char_val_by_uuid_read_rsp_t rsp;
		rsp = p_ble_evt->evt.gattc_evt.params.char_val_by_uuid_read_rsp;

		ble_gattc_handle_value_t iter;
		memset(&iter, 0, sizeof(ble_gattc_handle_value_t));

		offset = 0;
		pBuff = device_info.name_buff;

		if ((p_ble_evt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_SUCCESS) &&
		    // Remove const to p_ble_evt (because of sd_ble_gattc_evt_char_val_by_uuid_read_rsp_iter prototype)
		    // sd_ble_gattc_evt_char_val_by_uuid_read_rsp_iter will not modify the data
		    (sd_ble_gattc_evt_char_val_by_uuid_read_rsp_iter(
			     (ble_gattc_evt_t *)&p_ble_evt->evt.gattc_evt, &iter) == NRF_SUCCESS) &&
		    (rsp.value_len <= DEVICE_NAME_MAX_LENGTH - 1)) { // Always keep 1 byte for \0
			memcpy(pBuff, iter.p_value, rsp.value_len);

			pBuff += rsp.value_len;
			offset += rsp.value_len;

			// TODO : get the complete name

			if (m_current_request == GET_DEVICE_NAME_REQ) {
				pm_peer_id_t peer_id = 0;

				ret_code_t err_code = pm_peer_id_get(
					p_ble_evt->evt.gattc_evt.conn_handle, &peer_id);
				if (err_code == NRF_SUCCESS) {
					device_list_store_device_info_sched(peer_id, &device_info);
					m_current_request = INVALID_REQ;
					offset = 0;
					pBuff = device_info.name_buff;
					memset(device_info.name_buff, 0, DEVICE_NAME_MAX_LENGTH);
				} else {
					app_trace_log("Failed to get peer id\n");
				}
			}

			//              err_code = sd_ble_gattc_read(p_ble_evt->evt.gattc_evt.conn_handle, iter.handle, offset);
		} else {
			m_current_request = INVALID_REQ;
		}
	} break;
	case BLE_GATTC_EVT_READ_RSP: {
		app_trace_log("BLE_GATTC_EVT_READ_RSP\n");
		ble_gattc_evt_read_rsp_t rsp;
		rsp = p_ble_evt->evt.gattc_evt.params.read_rsp;

		app_trace_log("status =0x%x, len=%d\n", p_ble_evt->evt.gattc_evt.gatt_status,
			      rsp.len);
		if (p_ble_evt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_SUCCESS &&
		    rsp.len != 0 && (offset + rsp.len <= DEVICE_NAME_MAX_LENGTH - 1)) {
			uint8_t len =
				1; // Workaround for invalid data (after 2nd byte) issue in rsp.data
			memcpy(pBuff, rsp.data, len);
			pBuff += len;

			offset += len;
			err_code = sd_ble_gattc_read(p_ble_evt->evt.gattc_evt.conn_handle,
						     rsp.handle, offset);
			APP_ERROR_CHECK(err_code);
		} else {
			app_trace_log("End of read\n");

			if (rsp.len == 0) {
				if (m_current_request == GET_DEVICE_NAME_REQ) {
					pm_peer_id_t peer_id = 0;

					ret_code_t err_code = pm_peer_id_get(
						p_ble_evt->evt.gattc_evt.conn_handle, &peer_id);
					if (err_code == NRF_SUCCESS) {
						device_list_store_device_info(peer_id,
									      &device_info);
					} else {
						app_trace_log("Failed to get peer id\n");
					}
				}
			}

			m_current_request = INVALID_REQ;
			offset = 0;
			pBuff = device_info.name_buff;
			memset(device_info.name_buff, 0, DEVICE_NAME_MAX_LENGTH);
		}
	} break; // BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP

	case BLE_GAP_EVT_PASSKEY_DISPLAY: {
		if (p_ble_evt->evt.gap_evt.params.passkey_display.match_request) {
			app_lesc_start_num_comp(
				p_ble_evt->evt.gap_evt.conn_handle,
				p_ble_evt->evt.gap_evt.params.passkey_display.passkey,
				sizeof(p_ble_evt->evt.gap_evt.params.passkey_display.passkey));
		}
	} break;

	case BLE_GAP_EVT_AUTH_STATUS:
		if (p_ble_evt->evt.gap_evt.params.auth_status.auth_status !=
		    BLE_GAP_SEC_STATUS_SUCCESS) {
			app_lesc_abort_pairing();
		}
		break;

	default:
		// No implementation needed.
		break;
	}
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void)
{
	ret_code_t err_code;

	err_code = nrf_sdh_enable_request();
	APP_ERROR_CHECK(err_code);

	// Configure the BLE stack using the default settings.
	// Fetch the start address of the application RAM.
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
	APP_ERROR_CHECK(err_code);

	// Enable BLE stack.
	err_code = nrf_sdh_ble_enable(&ram_start);
	APP_ERROR_CHECK(err_code);

	// Register a handler for BLE events.
	NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the Event Scheduler initialization.
 */
void scheduler_init(void)
{
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
	uint32_t err_code;

	switch (event) {
	case BSP_EVENT_SLEEP:
		sleep_mode_enter();
		break;

	case BSP_EVENT_DISCONNECT:
		err_code = sd_ble_gap_disconnect(m_conn_handle,
						 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		if (err_code != NRF_ERROR_INVALID_STATE) {
			APP_ERROR_CHECK(err_code);
		}
		break;

	case BSP_EVENT_WHITELIST_OFF:
		if (m_conn_handle == BLE_CONN_HANDLE_INVALID) {
			err_code = ble_advertising_restart_without_whitelist(&m_advertising);
			if (err_code != NRF_ERROR_INVALID_STATE) {
				APP_ERROR_CHECK(err_code);
			}
		}
		break;

	case BSP_EVENT_KEY_2:
		NRF_LOG_INFO("Send report desc\r\n");
		break;

	case BSP_EVENT_KEY_3:
		NRF_LOG_INFO("Pressed Button 4\r\n");
		break;

	default:
		break;
	}
}

/**@brief Function for the Peer Manager initialization.
 */
void peer_manager_init(void)
{
	ble_gap_sec_params_t sec_param;
	ret_code_t err_code;

	err_code = pm_init();
	APP_ERROR_CHECK(err_code);

	memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

	// Security parameters to be used for all security procedures.
	sec_param.bond = SEC_PARAM_BOND;
	sec_param.mitm = SEC_PARAM_MITM;
	sec_param.lesc = SEC_PARAM_LESC;
	sec_param.keypress = SEC_PARAM_KEYPRESS;
	sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
	sec_param.oob = SEC_PARAM_OOB;
	sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
	sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
	sec_param.kdist_own.enc = 1;
	sec_param.kdist_own.id = 1;
	sec_param.kdist_peer.enc = 1;
	sec_param.kdist_peer.id = 1;

	err_code = pm_sec_params_set(&sec_param);
	APP_ERROR_CHECK(err_code);

	err_code = pm_register(pm_evt_handler);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the Advertising functionality.
 */
void advertising_init(void)
{
	uint32_t err_code;
	uint8_t adv_flags;
	ble_advertising_init_t init;

	memset(&init, 0, sizeof(init));

	adv_flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
	init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
	init.advdata.include_appearance = true;
	init.advdata.flags = adv_flags;
	init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
	init.srdata.uuids_complete.p_uuids = m_adv_uuids;

	init.config = advertising_init_options;

	init.evt_handler = on_adv_evt;
	init.error_handler = ble_advertising_error_handler;

	err_code = ble_advertising_init(&m_advertising, &init);
	APP_ERROR_CHECK(err_code);

	ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
void app_bsp_init()
{
	ret_code_t err_code;
	bsp_event_t startup_event;

	err_code = bsp_init(BSP_INIT_BUTTONS, bsp_event_handler);
	APP_ERROR_CHECK(err_code);

	err_code = bsp_btn_ble_init(NULL, &startup_event);
	APP_ERROR_CHECK(err_code);

	m_b_erase_bonds = false; // (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
	if (m_b_erase_bonds) {
		app_trace_log("Clear bonding data event\r\n");
	}
	//m_b_erase_bonds = true;
}

/**@brief Function for initializing the nrf log module.
 */
void log_init(void)
{
	NRF_LOG_DEFAULT_BACKENDS_INIT();
	ret_code_t err_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing power management.
 */
void power_management_init(void)
{
	ret_code_t err_code;
	err_code = nrf_pwr_mgmt_init();
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void idle_state_handle(void)
{
	ret_code_t err_code = nrf_ble_lesc_request_handler();
	if (err_code != NRF_ERROR_INVALID_STATE) {
		APP_ERROR_CHECK(err_code);
	}

	app_sched_execute();

	if (NRF_LOG_PROCESS() == false) {
		nrf_pwr_mgmt_run();
	}
}

ret_code_t radio_power_off()
{
	ret_code_t err_code =
		sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
	APP_ERROR_CHECK(err_code);
	NRF_RADIO->TASKS_DISABLE = 1;
	return err_code;
}

void radio_power_on()
{
	NRF_RADIO->TASKS_DISABLE = 0;
}

void delete_whitelist()
{
	pm_whitelist_set(NULL, 0);
}

ret_code_t ble_man_start_advertising()
{
	static bool init = false;

	if (!init) {
		services_init();
		conn_params_init();
		advertising_init();
		bonds_init(m_b_erase_bonds);

		init = true;
	}

	ble_man_stop_advertising();

	return ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
}

ret_code_t ble_man_stop_advertising()
{
	ret_code_t err_code = ble_advertising_stop(&m_advertising);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
	if (err_code != NRF_ERROR_INVALID_STATE && err_code != BLE_ERROR_INVALID_CONN_HANDLE) {
		APP_ERROR_CHECK(err_code);
	}

	return NRF_SUCCESS;
}

uint16_t ble_man_get_conn_handle()
{
	return m_conn_handle;
}

void ble_man_set_whitelist(bool enable)
{
	advertising_init_options.ble_adv_whitelist_enabled = enable;
}

void ble_man_set_directed(bool enable)
{
	advertising_init_options.ble_adv_directed_enabled = enable;
}
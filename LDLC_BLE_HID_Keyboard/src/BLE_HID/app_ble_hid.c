#include "spi_protocol.h"
#include "spi_protocol_ble_hid_input_report.h"
#include "spi_protocol_ble_hid_report_map.h"
#include "spi_protocol_ble_hid_output_report.h"
#include "ble_management.h"

#include "app_ble_hid.h"
#include "sdk_common.h"
#include "app_error.h"
#include "nrf_log.h"

#include "application_defs.h"

#define BLE_HID_INPUT_REPORT_KEYS_INDEX 0 /**< Index of Input Report. */
#define BLE_HID_INPUT_REPORT_REF_ID 0 /**< Id of reference to Keyboard Input Report. */
#define BLE_HID_OUTPUT_REP_REF_ID 0 /**< Id of reference to Keyboard Output Report. */

#define BLE_HID_NB_OUTPUT_REPORT 3
#define BLE_HID_OUTPUT_REPORT_MAX_LEN 2 /**< Maximum length of Output Report. */

#define BASE_USB_HID_SPEC_VERSION                                                                  \
	0x0111 /**< Version number of base USB HID Specification implemented by this application. */

static bool m_b_hid_report_map_initialized = false;
static bool m_b_hids_initialized = false;

#define OPCODE_LENGTH 1
#define HANDLE_LENGTH 2

#define MAX_INPUT_REPORT_ID 3
#define BLE_HID_INPUT_REPORT_MAX_LEN 8

#define BLE_HID_REPORT_MAP_MAX_LEN BLE_GATTS_VAR_ATTR_LEN_MAX

BLE_HIDS_DEF(m_hids, NRF_SDH_BLE_TOTAL_LINK_COUNT,
	     // Input reports : add (BLE_HID_INPUT_REPORT_MAX_LEN) (MAX_INPUT_REPORT_ID + 1) times
	     BLE_HID_INPUT_REPORT_MAX_LEN, BLE_HID_INPUT_REPORT_MAX_LEN,
	     BLE_HID_INPUT_REPORT_MAX_LEN, BLE_HID_INPUT_REPORT_MAX_LEN,
	     // Output reports : add (BLE_HID_OUTPUT_REPORT_MAX_LEN) (BLE_HID_NB_OUTPUT_REPORT) times
	     BLE_HID_OUTPUT_REPORT_MAX_LEN, BLE_HID_OUTPUT_REPORT_MAX_LEN,
	     BLE_HID_OUTPUT_REPORT_MAX_LEN);

typedef struct _input_report_t {
	uint8_t report_data[BLE_HID_INPUT_REPORT_MAX_LEN];
	uint8_t report_id;
	uint32_t report_size;
} input_report_t;

static input_report_t m_hid_input_report = { 0 };

typedef struct _report_map_t {
	uint8_t report_map_data[BLE_HID_REPORT_MAP_MAX_LEN];
	uint8_t report_map_size;
	uint8_t input_report_id_max;
	uint16_t input_report_lengths[MAX_INPUT_REPORT_ID + 1];
} report_map_t;

static report_map_t m_hid_report_map = { 0 };

static void on_hids_evt(ble_hids_t *p_hids, ble_hids_evt_t *p_evt);

ret_code_t app_ble_hid_input_report_init()
{
	if (!m_b_hids_initialized) {
		// Ignore the command if the HID Service has not been initialized
		return NRF_ERROR_INVALID_STATE;
	}

	return NRF_SUCCESS;
}

ret_code_t app_ble_hid_input_report_data_init(uint32_t report_length)
{
	ret_code_t ret = NRF_SUCCESS;

	if (report_length > BLE_HID_INPUT_REPORT_MAX_LEN) {
		ret = NRF_ERROR_INVALID_LENGTH;
	} else {
		memset(m_hid_input_report.report_data, 0, BLE_HID_INPUT_REPORT_MAX_LEN);
		m_hid_input_report.report_size = report_length;
	}

	return ret;
}

ret_code_t app_ble_hid_input_report_set_id(uint8_t report_id)
{
	ret_code_t ret = NRF_SUCCESS;

	m_hid_input_report.report_id = report_id;

	return ret;
}

ret_code_t app_ble_hid_input_report_write(uint32_t data_index, uint8_t data)
{
	ret_code_t ret = NRF_SUCCESS;

	if (data_index < BLE_HID_INPUT_REPORT_MAX_LEN) {
		m_hid_input_report.report_data[data_index] = data;
	} else {
		ret = NRF_ERROR_NO_MEM;
	}

	return ret;
}

ret_code_t app_ble_hid_input_report_send()
{
	ret_code_t ret = NRF_SUCCESS;

	if (!m_b_hids_initialized) {
		return NRF_ERROR_INVALID_STATE;
	}

	ret = ble_hids_inp_rep_send(&m_hids, m_hid_input_report.report_id,
				    m_hid_input_report.report_size, m_hid_input_report.report_data,
				    ble_man_get_conn_handle());
	return ret;
}

ret_code_t app_ble_hid_report_map_init()
{
	if (m_b_hids_initialized) {
		// Ignore the command if the HID Service has already been initialized
		return NRF_ERROR_INVALID_STATE;
	}

	return NRF_SUCCESS;
}

ret_code_t app_ble_hid_set_input_report_id_max(uint8_t report_id_max)
{
	if (report_id_max > MAX_INPUT_REPORT_ID) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_hid_report_map.input_report_id_max = report_id_max;

	return NRF_SUCCESS;
}

ret_code_t app_ble_hid_set_input_report_length(uint8_t report_id, uint16_t report_length)
{
	if (report_id > m_hid_report_map.input_report_id_max) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_hid_report_map.input_report_lengths[report_id] = report_length;

	return NRF_SUCCESS;
}

ret_code_t app_ble_hid_report_map_data_init(uint32_t report_map_length)
{
	ret_code_t ret = NRF_SUCCESS;

	if (report_map_length > BLE_HID_REPORT_MAP_MAX_LEN) {
		ret = NRF_ERROR_INVALID_LENGTH;
	} else {
		memset(m_hid_report_map.report_map_data, 0, BLE_HID_REPORT_MAP_MAX_LEN);
		m_hid_report_map.report_map_size = report_map_length;
	}

	return ret;
}

ret_code_t app_ble_hid_report_map_write(uint32_t data_index, uint8_t data)
{
	ret_code_t ret = NRF_SUCCESS;

	if (data_index < BLE_HID_REPORT_MAP_MAX_LEN) {
		m_hid_report_map.report_map_data[data_index] = data;
	} else {
		ret = NRF_ERROR_NO_MEM;
	}

	return ret;
}

/**@brief Function for handling the HID Report Characteristic Write event.
 *
 * @param[in]   p_evt   HID service event.
 */
static void on_hid_rep_char_write(ble_hids_t *p_hids, ble_hids_evt_t *p_evt)
{
	ASSERT(p_hids != NULL);
	ASSERT(p_evt != NULL);

	if (p_evt->params.char_write.char_id.rep_type == BLE_HIDS_REP_TYPE_OUTPUT) {
		ret_code_t err_code;
		uint8_t report_val[BLE_HID_OUTPUT_REPORT_MAX_LEN];
		uint8_t *p_report_val = report_val;
		uint8_t report_index = p_evt->params.char_write.char_id.rep_index;
		uint16_t len = p_evt->params.char_write.len;
		size_t written = 0;

		if (len <= p_hids->p_outp_rep_init_array[report_index].max_len) {
			// Like USB HID, report 0 can be used by some OS (e.g. Windows 10) with first byte = report ID
			// Use the same format to transmit the output report to the master
			if (report_index != 0) {
				p_report_val[0] = p_hids->p_outp_rep_init_array[report_index]
							  .rep_ref.report_id;
				p_report_val++;
				written++;
			}

			if (BLE_HID_OUTPUT_REPORT_MAX_LEN - written >= len) {
				err_code = ble_hids_outp_rep_get(
					&m_hids, report_index, len, 0,
					p_evt->p_ble_evt->evt.gatts_evt.conn_handle, p_report_val);
				APP_ERROR_CHECK(err_code);
				written += len;

				spi_protocol_output_report(report_val, written);
			}
		}
	}
}

/**@brief Function for handling HID events.
 *
 * @details This function will be called for all HID events which are passed to the application.
 *
 * @param[in]   p_hids  HID service structure.
 * @param[in]   p_evt   Event received from the HID service.
 */
static void on_hids_evt(ble_hids_t *p_hids, ble_hids_evt_t *p_evt)
{
	switch (p_evt->evt_type) {
	case BLE_HIDS_EVT_BOOT_MODE_ENTERED:
		break;

	case BLE_HIDS_EVT_REPORT_MODE_ENTERED:
		break;

	case BLE_HIDS_EVT_REP_CHAR_WRITE:
		on_hid_rep_char_write(p_hids, p_evt);
		break;

	case BLE_HIDS_EVT_NOTIF_ENABLED:
		break;

	default:
		// No implementation needed.
		break;
	}
}

/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void service_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

static ret_code_t
app_ble_hid_init_input_report_for_hids(ble_hids_inp_rep_init_t *input_report_array)
{
	uint8_t report_id = 0;
	ble_hids_inp_rep_init_t *p_input_report = NULL;

	for (report_id = 0; report_id <= m_hid_report_map.input_report_id_max; report_id++) {
		p_input_report = &input_report_array[report_id];

		if (p_input_report != NULL) {
			p_input_report->max_len = m_hid_report_map.input_report_lengths[report_id];
			p_input_report->rep_ref.report_id = report_id;
			p_input_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_INPUT;

			p_input_report->sec.cccd_wr = DEFAULT_CHAR_SECURITY_LEVEL;
			p_input_report->sec.rd = DEFAULT_CHAR_SECURITY_LEVEL;
			p_input_report->sec.wr = DEFAULT_CHAR_SECURITY_LEVEL;
		}
	}

	return NRF_SUCCESS;
}

static ret_code_t app_ble_hid_hids_init()
{
	if (!m_b_hid_report_map_initialized) {
		return NRF_ERROR_INVALID_STATE;
	}

	ret_code_t err_code;
	ble_hids_init_t hids_init_obj;
	static ble_hids_inp_rep_init_t input_report_array[MAX_INPUT_REPORT_ID + 1] = { 0 };
	static ble_hids_outp_rep_init_t output_report_array[BLE_HID_NB_OUTPUT_REPORT];
	ble_hids_outp_rep_init_t *p_output_report;
	uint8_t hid_info_flags;

	memset((void *)input_report_array, 0, sizeof(ble_hids_inp_rep_init_t));
	memset((void *)output_report_array, 0, sizeof(ble_hids_outp_rep_init_t));

	// Initialize HID Service
	// Init input report according to the received report map
	app_ble_hid_init_input_report_for_hids(input_report_array);

	// Init output report
	size_t i;
	for (i = 0; i < BLE_HID_NB_OUTPUT_REPORT; ++i) {
		p_output_report = &output_report_array[i];
		p_output_report->max_len = BLE_HID_OUTPUT_REPORT_MAX_LEN;
		p_output_report->rep_ref.report_id = i;
		p_output_report->rep_ref.report_type = BLE_HIDS_REP_TYPE_OUTPUT;

		p_output_report->sec.rd = SEC_JUST_WORKS;
		p_output_report->sec.wr = SEC_JUST_WORKS;
	}

	hid_info_flags = HID_INFO_FLAG_REMOTE_WAKE_MSK | HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK;

	memset(&hids_init_obj, 0, sizeof(hids_init_obj));

	hids_init_obj.evt_handler = on_hids_evt;
	hids_init_obj.error_handler = service_error_handler;
	hids_init_obj.is_kb = true;
	hids_init_obj.is_mouse = false;
	hids_init_obj.inp_rep_count = m_hid_report_map.input_report_id_max + 1;
	hids_init_obj.p_inp_rep_array = input_report_array;
	hids_init_obj.outp_rep_count = BLE_HID_NB_OUTPUT_REPORT;
	hids_init_obj.p_outp_rep_array = output_report_array;
	hids_init_obj.feature_rep_count = 0;
	hids_init_obj.p_feature_rep_array = NULL;
	hids_init_obj.rep_map.data_len = m_hid_report_map.report_map_size;
	hids_init_obj.rep_map.p_data = m_hid_report_map.report_map_data;
	hids_init_obj.hid_information.bcd_hid = BASE_USB_HID_SPEC_VERSION;
	hids_init_obj.hid_information.b_country_code = 0;
	hids_init_obj.hid_information.flags = hid_info_flags;
	hids_init_obj.included_services_count = 0;
	hids_init_obj.p_included_services_array = NULL;

	hids_init_obj.rep_map.rd_sec = DEFAULT_CHAR_SECURITY_LEVEL;
	hids_init_obj.hid_information.rd_sec = DEFAULT_CHAR_SECURITY_LEVEL;

	hids_init_obj.boot_kb_inp_rep_sec.cccd_wr = DEFAULT_CHAR_SECURITY_LEVEL;
	hids_init_obj.boot_kb_inp_rep_sec.rd = DEFAULT_CHAR_SECURITY_LEVEL;

	hids_init_obj.boot_kb_outp_rep_sec.rd = DEFAULT_CHAR_SECURITY_LEVEL;
	hids_init_obj.boot_kb_outp_rep_sec.wr = DEFAULT_CHAR_SECURITY_LEVEL;

	hids_init_obj.protocol_mode_rd_sec = DEFAULT_CHAR_SECURITY_LEVEL;
	hids_init_obj.protocol_mode_wr_sec = DEFAULT_CHAR_SECURITY_LEVEL;
	hids_init_obj.ctrl_point_wr_sec = DEFAULT_CHAR_SECURITY_LEVEL;

	err_code = ble_hids_init(&m_hids, &hids_init_obj);
	APP_ERROR_CHECK(err_code);

	m_b_hids_initialized = true;

	m_hid_input_report.report_size = 0;
	m_hid_input_report.report_id = 0;

	return NRF_SUCCESS;
}
ret_code_t app_ble_hid_report_map_send()
{
	if (m_b_hids_initialized) {
		// Ignore the command if the HID Service has already been initialized
		return NRF_ERROR_INVALID_STATE;
	}

	m_b_hid_report_map_initialized = true;

	return app_ble_hid_hids_init();
}
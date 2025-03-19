#include "nrf_drv_spis.h"
#include "app_scheduler.h"
#include "spi_config.h"

#include "spi_protocol.h"
#include "spi_protocol_tx.h"

#include "spi_protocol_common_actions.h"

#define NRF_LOG_MODULE_NAME PROT
#include "nrf_log.h"

#ifdef SPI_PROTOCOL_DEBUG
#define SPI_PROTOCOL_LOG(...) NRF_LOG_INFO(__VA_ARGS__)
#else
#define SPI_PROTOCOL_LOG(...)
#endif

NRF_LOG_MODULE_REGISTER();

typedef enum { CMD_STATE = 0, DATA_STATE } spi_protocol_state;

typedef enum { PARAM_LENGTH_STATE = 0, PARAM_DATA_STATE } spi_param_state;

//==== Example data ======

//static uint8_t get_device_count_buff[] = {0x09,
//                                        0x00, 0x03,
//                                        0x00, 0x01,
//                                        0x00};
//
//static uint8_t get_device_info_buff[] = {0x0A,
//                                        0x00, 0x04,
//                                        0x00, 0x02,
//                                        0x00, 0x01};
//
//static uint8_t launch_browser_buff[] = {0x02,
//                                        0x00, 0x07,
//                                        0x00, 0x01,
//                                        0x01,
//                                        0x00, 0x02,
//                                        0x10, 0x00};

//static uint8_t launch_android_mail_buff[] = {0x02,
//                                        0x00, 0x07,
//                                        0x00, 0x01,
//                                        0x01,
//                                        0x00, 0x02,
//                                        0x20, 0x00};

//static uint8_t launch_calc_buff[] =  {0x02,
//                                      0x00, 0x07,
//                                      0x00, 0x01,
//                                      0x01,
//                                      0x00, 0x02,
//                                      0x80, 0x00};

//static uint8_t launch_task_man_buff[] = {0x02,
//                                         0x00, 0x0D,
//                                         0x00, 0x01,
//                                         0x02,
//                                         0x00, 0x08,
//                                         0x05, 0x00, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00};

//static uint8_t HID_KB_ReportDesc[] = {
//                        0x01,
//                        0x00, 0x8E,
//                        0x00, 0x01,
//                        0x02,
//                        0x00, 0x06,
//                        0x00, 0x02, 0x00, 0x02, 0x00, 0x08,
//                        0x00, 0x81,
//			0x05, 0x0C, //Usage page consumer
//			0x09, 0x01,  //Usage (Consumer control)
//			0xA1, 0x01,  //collection (application)
//			0x85, 0x01,  //Report_ID 1
//			0x15, 0x00,  //Logical_minimum (0)
//			0x25, 0x01,  //Logical maximum (1)
//			0x75, 0x01,  //Report_Size(1)
//			0x95, 0x01,  //Report_Count (1)
//			0x0A, 0x24, 0x02, // Application Control (AC) "Back"
//			0x81, 0x06,  //Input
//			0x0A, 0x25, 0x02, //AC "Forward"
//			0x81, 0x06,  //Input
//			0x0A, 0x26, 0x02, //AC "Stop"
//			0x81, 0x06,  //Input
//			0x0A, 0x27, 0x02, //AC "Refresh"
//			0x81, 0x06,  //Input
//			0x0A, 0x23, 0x02, //AC "Home"
//			0x81, 0x06,  //Input
//			0x0A, 0x8A, 0x01, //Application Launch (AL) "Email Reader"
//			0x81, 0x06,  //Input
//			0x0A, 0x94, 0x01, //AL "Local Machine Browser"
//			0x81, 0x06,  //Input
//			0x0A, 0x92, 0x01, //AL "Calculator"
//			0x81, 0x06,  //Input
//			0x09, 0xB5, //Button Page, Button 181 (B5)
//			0x81, 0x06,  //Input
//			0x09, 0xB6, //Button Page, Button 182 (B6)
//			0x81, 0x06,  //Input
//			0x09, 0xCD, //Button Page, Button 205 (CD)
//			0x81, 0x06,  //Input
//			0x09, 0xB7, //Button Page, Button 183 (B7)
//			0x81, 0x06,  //Input
//			0x0A, 0x83, 0x01, //AL "Consumer Control Configuration"
//			0x81, 0x06,  //Input
//			//0x95, 0x0B, //Report_Count(B)
//			0x95, 0x03, //Report_Count(03)//Padding to reach a multiple of bytes
//			0x81, 0x01,  //Input (Constant)
//			0xC0, //End collection
//			0x05, 0x01, // USAGE_PAGE (Generic Desktop)
//			0x09, 0x06, // USAGE (Keyboard)
//			0xa1, 0x01, // COLLECTION (Application)
//			0x85, 0x02, //Report_ID 2
//			0x75, 0x01,  //   REPORT_SIZE (1)
//			0x95, 0x08,  //   REPORT_COUNT (8)
//			0x05, 0x07,  //   USAGE_PAGE (Keyboard)(Key Codes)
//			0x19, 0xe0,  //   USAGE_MINIMUM (Keyboard LeftControl)(224)
//			0x29, 0xe7,  //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
//			0x15, 0x00,  //   LOGICAL_MINIMUM (0)
//			0x25, 0x01,  //   LOGICAL_MAXIMUM (1)
//			0x81, 0x02,  //   INPUT (Data,Var,Abs) ; Modifier byte
//			0x95, 0x01,  //   REPORT_COUNT (1)
//			0x75, 0x08,  //   REPORT_SIZE (8)
//			0x81, 0x03,  //   INPUT (Cnst,Var,Abs) ; Reserved byte
//			0x95, 0x06,  //   REPORT_COUNT (6)
//			0x75, 0x08,  //   REPORT_SIZE (8)
//			0x15, 0x00,  //   LOGICAL_MINIMUM (0)
//			0x25, 0x65,  //   LOGICAL_MAXIMUM (101)
//			0x05, 0x07,  //   USAGE_PAGE (Keyboard)(Key Codes)
//			0x19, 0x00,  //   USAGE_MINIMUM (Reserved (no event indicated))(0)
//			0x29, 0x65,  //   USAGE_MAXIMUM (Keyboard Application)(101)
//			0x81, 0x00,  //   INPUT (Data,Ary,Abs)
//			0xc0,  // END_COLLECTION
//
// };
//========================

static spi_protocol_state m_protocol_state = CMD_STATE;
static spi_protocol_parse_action *m_p_action = NULL;
static uint32_t m_buffer_pos = 0;
static uint32_t m_cmd_data_pos = 0;
static uint16_t m_cmd_data_length = 0;
static uint32_t m_cmd_data_remaining_data_length = 0;
static spi_param_state m_parameter_state = PARAM_LENGTH_STATE;
static uint32_t m_parameter_index = 0;
static uint32_t m_parameter_data_pos = 0;
static uint16_t m_parameter_length = 0;
static uint32_t m_parameter_length_pos = 0;

#define SPIS_INSTANCE 1 /**< SPIS instance index. */
static const nrf_drv_spis_t m_spis = NRF_DRV_SPIS_INSTANCE(SPIS_INSTANCE); /**< SPIS instance. */

static uint8_t m_spis_rx_buf[SPI_BUFFER_MAX_LEN] = { 0 }; /**< RX buffer. */

static volatile bool m_spis_xfer_done =
	false; /**< Flag used to indicate that SPIS instance completed the transfer. */

static ret_code_t spi_protocol_parse_parameter_end()
{
	m_parameter_index++;

	return NRF_SUCCESS;
}

static ret_code_t spi_protocol_parse_parameter_init()
{
	m_parameter_state = PARAM_LENGTH_STATE;
	m_parameter_data_pos = 0;
	m_parameter_length = 0;
	m_parameter_length_pos = 0;

	return NRF_SUCCESS;
}

static ret_code_t spi_protocol_parse_cmd(uint8_t data_byte)
{
	ret_code_t ret = NRF_ERROR_NOT_SUPPORTED;
	uint8_t i = 0;

	SPI_PROTOCOL_LOG("++spi_protocol_parse_cmd %d\r\n", data_byte);

	if (m_buffer_pos == 0) {
		for (i = 0; m_actions[i].command != -1; i++) {
			if (m_actions[i].command == data_byte) {
				SPI_PROTOCOL_LOG("spi_protocol_parse_cmd: cmd = %d\r\n", data_byte);

				m_p_action = &m_actions[i];

				ret = NRF_SUCCESS;
				break;
			}
		}

		m_cmd_data_length = 0;
	} else if (m_buffer_pos >= 1 && m_buffer_pos <= 2) {
		m_cmd_data_length |= data_byte << ((2 - m_buffer_pos) * 8);

		if (m_buffer_pos == 2) {
			SPI_PROTOCOL_LOG("spi_protocol_parse_cmd: length = %d\r\n",
					 m_cmd_data_length);
			m_protocol_state = DATA_STATE;
			m_cmd_data_remaining_data_length = m_cmd_data_length;
			spi_protocol_parse_parameter_init();
		}

		ret = NRF_SUCCESS;
	}

	return ret;
}

static ret_code_t spi_protocol_reset()
{
	m_protocol_state = CMD_STATE;
	m_cmd_data_pos = 0;
	m_cmd_data_length = 0;
	m_cmd_data_remaining_data_length = 0;

	m_parameter_state = PARAM_LENGTH_STATE;
	m_parameter_index = 0;
	m_parameter_data_pos = 0;
	m_parameter_length = 0;
	m_parameter_length_pos = 0;

	return NRF_SUCCESS;
}

static ret_code_t spi_protocol_parse_parameter_length(uint8_t data_byte)
{
	ret_code_t ret = NRF_ERROR_NOT_SUPPORTED;

	SPI_PROTOCOL_LOG("++spi_protocol_parse_parameter_length %d, pos %d\r\n", data_byte,
			 m_parameter_length_pos);

	if (m_parameter_length_pos <= sizeof(m_parameter_length) - 1) {
		m_parameter_length |=
			data_byte
			<< ((sizeof(m_parameter_length) - 1 - m_parameter_length_pos) * 8);

		ret = NRF_SUCCESS;

		if (m_parameter_length_pos == sizeof(m_parameter_length) - 1) {
			SPI_PROTOCOL_LOG("spi_protocol_parse_parameter_length: length = %d\r\n",
					 m_parameter_length);
			m_parameter_state = PARAM_DATA_STATE;
			m_parameter_length_pos = 0;
			m_parameter_data_pos = 0;
			if (m_parameter_length + sizeof(m_parameter_length) <=
			    m_cmd_data_remaining_data_length) {
				m_cmd_data_remaining_data_length -=
					(m_parameter_length + sizeof(m_parameter_length));
			} else {
				ret = NRF_ERROR_INVALID_PARAM;
			}
		}

		m_parameter_length_pos++;
	}

	return ret;
}

static ret_code_t spi_protocol_parse_parameter_data(uint8_t data_byte)
{
	ret_code_t ret = NRF_SUCCESS;

	SPI_PROTOCOL_LOG("++spi_protocol_parse_parameter_data %d pos %d\r\n", data_byte,
			 m_parameter_data_pos);

	if (m_parameter_index >= SPI_PROTOCOL_MAX_PARAMETERS) {
		return NRF_ERROR_INVALID_PARAM;
	}

	if (m_parameter_data_pos == 0) {
		SPI_PROTOCOL_LOG("++start parsing parameter %d\r\n", m_parameter_data_pos);
		if (m_p_action->parameter_funcs[m_parameter_index].pfn_command_parameter_start !=
		    NULL) {
			ret = m_p_action->parameter_funcs[m_parameter_index]
				      .pfn_command_parameter_start(m_parameter_length);
			VERIFY_SUCCESS(ret);
		}
	}

	if (m_p_action->parameter_funcs[m_parameter_index].pfn_command_parameter_receive != NULL) {
		ret = m_p_action->parameter_funcs[m_parameter_index].pfn_command_parameter_receive(
			m_parameter_data_pos, data_byte);
		SPI_PROTOCOL_LOG("++parsing data parameter %d, ret=%d\r\n", m_parameter_data_pos,
				 ret);
		VERIFY_SUCCESS(ret);
	}

	if (m_parameter_data_pos == m_parameter_length - 1) {
		SPI_PROTOCOL_LOG("++end parsing parameter %d %d\r\n", m_parameter_data_pos,
				 m_parameter_length);
		if (m_p_action->parameter_funcs[m_parameter_index].pfn_command_parameter_stop !=
		    NULL) {
			ret = m_p_action->parameter_funcs[m_parameter_index]
				      .pfn_command_parameter_stop(m_parameter_length);
		}

		spi_protocol_parse_parameter_end();
		spi_protocol_parse_parameter_init();

		VERIFY_SUCCESS(ret);
	}

	m_parameter_data_pos++;

	return ret;
}

static ret_code_t spi_protocol_parse_parameter(uint8_t data_byte)
{
	ret_code_t ret = NRF_SUCCESS;

	SPI_PROTOCOL_LOG("++spi_protocol_parse_parameter %d\r\n", data_byte);

	if (m_parameter_index >= m_p_action->nb_parameters) {
		return NRF_ERROR_INVALID_PARAM;
	}

	switch (m_parameter_state) {
	case PARAM_LENGTH_STATE:
		ret = spi_protocol_parse_parameter_length(data_byte);
		break;
	case PARAM_DATA_STATE:
		ret = spi_protocol_parse_parameter_data(data_byte);
		break;
	default:
		ret = NRF_ERROR_INVALID_STATE;
		break;
	}

	//  if(ret != NRF_SUCCESS)
	//  {
	//    spi_protocol_reset();
	//  }

	return ret;
}

static ret_code_t spi_protocol_parse_data(uint8_t data_byte)
{
	ret_code_t ret = NRF_SUCCESS;

	SPI_PROTOCOL_LOG("++spi_protocol_parse_data %d, cmd_data_pos = %d\r\n", data_byte,
			 m_cmd_data_pos);

	if (m_cmd_data_pos == 0) {
		if (m_p_action->pfn_command_data_start != NULL) {
			SPI_PROTOCOL_LOG("++pfn_command_data_start %d\r\n", m_cmd_data_length);
			ret = m_p_action->pfn_command_data_start(m_cmd_data_length);
			VERIFY_SUCCESS(ret);
		}
	}

	ret = spi_protocol_parse_parameter(data_byte);
	VERIFY_SUCCESS(ret);

	if (m_cmd_data_pos == m_cmd_data_length - 1) {
		if (m_p_action->pfn_command_data_stop != NULL) {
			ret = m_p_action->pfn_command_data_stop(m_cmd_data_length);
		}
		spi_protocol_reset();
		return ret;
	}

	m_cmd_data_pos++;

	return ret;
}

static ret_code_t spi_protocol_parse(uint8_t data_byte)
{
	ret_code_t ret = NRF_SUCCESS;

	switch (m_protocol_state) {
	case CMD_STATE:
		ret = spi_protocol_parse_cmd(data_byte);
		break;
	case DATA_STATE:
		ret = spi_protocol_parse_data(data_byte);
		break;
	default:
		ret = NRF_ERROR_INVALID_STATE;
		break;
	}

	if (ret != NRF_SUCCESS) {
		spi_protocol_reset();
	}

	return ret;
}

static ret_code_t spi_protocol_parse_buffer(uint8_t *buffer, uint32_t buffer_length)
{
	ret_code_t ret = NRF_SUCCESS;

	for (m_buffer_pos = 0; m_buffer_pos < buffer_length; m_buffer_pos++) {
		ret = spi_protocol_parse(buffer[m_buffer_pos]);

		if (ret != NRF_SUCCESS) {
			// Do not continue to parse the buffer if an error occured
			break;
		}
	}

	return ret;
}

static void spi_protocol_set_host_ready_gpio(bool bActive)
{
	nrf_gpio_pin_write(APP_SPIS_HRDY_PIN, bActive ? 1 : 0);
}

static void spi_protocol_set_data_to_send_gpio(bool bActive)
{
	nrf_gpio_pin_write(APP_SPIS_DTS_PIN, bActive ? 1 : 0);
}

static void spi_protocol_init_gpios()
{
	// Default pin configuration for output is push-pull
	nrf_gpio_cfg_output(APP_SPIS_HRDY_PIN);
	nrf_gpio_cfg_output(APP_SPIS_DTS_PIN);

	spi_protocol_set_host_ready_gpio(false);
	spi_protocol_set_data_to_send_gpio(false);
}

static ret_code_t spi_protocol_spis_reset()
{
	uint8_t txrx_buffer_length = SPI_BUFFER_MAX_LEN; /**< TX RX Transfer length. */
	uint8_t *tx_buffer = spi_protocol_tx_get_tx_buff();

	memset(m_spis_rx_buf, 0, txrx_buffer_length);

	m_spis_xfer_done = false;
	APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&m_spis, tx_buffer, txrx_buffer_length,
						 m_spis_rx_buf, txrx_buffer_length));

	return NRF_SUCCESS;
}

static void spi_xfer_done_event_handler_scheduled_exec(void *p_event_data, uint16_t event_size)
{
	APP_ERROR_CHECK_BOOL(event_size == sizeof(spi_xfer_done_event_t));
	spi_xfer_done_event_t *p_spi_xfer_done_evt = (spi_xfer_done_event_t *)p_event_data;

	spi_protocol_parse_buffer(p_spi_xfer_done_evt->rx_buf, p_spi_xfer_done_evt->xfer_len);
}

/**
 * @brief SPIS user event handler.
 *
 * @param event
 */
static void spi_protocol_spis_event_handler(nrf_drv_spis_event_t event)
{
	if (event.evt_type == NRF_DRV_SPIS_XFER_DONE) {
		spi_xfer_done_event_t spi_xfer_done_evt = { 0 };

		spi_protocol_set_host_ready_gpio(false);

		memcpy(spi_xfer_done_evt.rx_buf, m_spis_rx_buf, event.rx_amount);
		spi_xfer_done_evt.xfer_len = event.rx_amount;

		spi_protocol_tx_data_sent(event.tx_amount);

		app_sched_event_put(&spi_xfer_done_evt, sizeof(spi_xfer_done_evt),
				    spi_xfer_done_event_handler_scheduled_exec);
		m_spis_xfer_done = true;

		spi_protocol_spis_reset();
	} else if (event.evt_type == NRF_DRV_SPIS_BUFFERS_SET_DONE) {
		spi_protocol_set_host_ready_gpio(true);
	}
}

static ret_code_t spi_protocol_spis_init()
{
	spi_protocol_reset();

	spi_protocol_init_gpios();

	nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG;

	spis_config.mode = APP_SPIS_MODE;

	spis_config.csn_pin = APP_SPIS_CS_PIN;
	spis_config.miso_pin = APP_SPIS_MISO_PIN;
	spis_config.mosi_pin = APP_SPIS_MOSI_PIN;
	spis_config.sck_pin = APP_SPIS_SCK_PIN;

	APP_ERROR_CHECK(nrf_drv_spis_init(&m_spis, &spis_config, spi_protocol_spis_event_handler));

	spi_protocol_spis_reset();

	return NRF_SUCCESS;
}

ret_code_t spi_protocol_uninit()
{
	nrfx_spis_uninit(&m_spis);

	return NRF_SUCCESS;
}

ret_code_t spi_protocol_init()
{
	spi_protocol_tx_init();
	spi_protocol_spis_init();

	return NRF_SUCCESS;
}

void spi_protocol_notify_data_to_send(bool b_data_to_send)
{
	spi_protocol_set_data_to_send_gpio(b_data_to_send);
}
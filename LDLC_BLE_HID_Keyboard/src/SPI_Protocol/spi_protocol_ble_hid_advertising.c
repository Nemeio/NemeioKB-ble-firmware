#include "spi_protocol_ble_hid_advertising.h"
#include "ble_gap.h"
#include "ble_management.h"

#define COMMAND_INDEX 0
#define DATA_LENGTH_INDEX 1
#define DATA_INDEX 2
#define PARSE_DATA_LENGTH 1

#define ENABLE_WHITELIST 0x01
#define ENABLE_DIRECTED_ADV 0x02

typedef struct {
	uint8_t command;
	uint32_t (*parse_data)(uint8_t data_length, uint8_t *data);
} advertising_configuration_parse_action;

static advertising_configuration_parse_action m_actions[] = {
	{ ENABLE_WHITELIST, parse_enable_whitelist },
	{ ENABLE_DIRECTED_ADV, parse_enable_directed },
};

ret_code_t set_advertising_data_receive(uint32_t data_index, uint8_t data)
{
	ret_code_t ret = NRF_ERROR_NULL;

	if (data) {
		ret = ble_man_start_advertising();
	} else {
		ret = ble_man_stop_advertising();
	}
	return ret;
}

ret_code_t configure_advertising_data_receive(uint32_t data_index, uint8_t data)
{
	static uint16_t m_subparameter_data_length = 0;
	static uint8_t m_subparameter_data[PARSE_DATA_LENGTH] = { 0 };
	static uint8_t m_subparameter_cmd = 0;

	ret_code_t ret = NRF_ERROR_NULL;

	if (COMMAND_INDEX == data_index) {
		m_subparameter_cmd = data;
		ret = NRF_SUCCESS;
	} else if (DATA_LENGTH_INDEX == data_index) {
		m_subparameter_data_length = data;
		ret = NRF_SUCCESS;
	} else if ((data_index <= (DATA_INDEX + m_subparameter_data_length)) &&
		   ((data_index - DATA_INDEX) < ARRAY_SIZE(m_subparameter_data))) {
		m_subparameter_data[data_index - DATA_INDEX] = data;
		ret = NRF_SUCCESS;

	} else {
		for (uint8_t i = 0; i < ARRAY_SIZE(m_actions); ++i) {
			if (m_subparameter_cmd == m_actions[i].command) {
				m_actions[i].parse_data(m_subparameter_data_length,
							&m_subparameter_data[0]);
				ret = NRF_SUCCESS;
			}
		}
	}
	return ret;
}

ret_code_t parse_enable_whitelist(uint8_t data_length, uint8_t *data)
{
	ret_code_t ret = NRF_ERROR_NULL;
	if (0 == *data) {
		ble_man_set_whitelist(false);
		ret = NRF_SUCCESS;
	} else {
		ble_man_set_whitelist(true);
		ret = NRF_SUCCESS;
	}
	return ret;
}

ret_code_t parse_enable_directed(uint8_t data_length, uint8_t *data)
{
	ret_code_t ret = NRF_ERROR_NULL;
	if (0 == *data) {
		ble_man_set_directed(false);
		ret = NRF_SUCCESS;
	} else {
		ble_man_set_directed(true);
		ret = NRF_SUCCESS;
	}
	return ret;
}
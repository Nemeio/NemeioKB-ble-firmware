#include "spi_protocol_get_device_list.h"
#include "spi_protocol_tx.h"
#include "device_list.h"
#include "spi_protocol.h"
#include "app_trace.h"

typedef struct {
	int8_t status;
	device_info_t device_info;
} __packed device_info_resp_t;

#define DEVICE_INFO_STATUS_SUCCESS (0)
#define DEVICE_INFO_STATUS_NOT_FOUND (-1)
#define DEVICE_INFO_STATUS_NOT_READY (-2)
#define DEVICE_INFO_STATUS_UNKNOWN (-3)

static uint8_t m_device_count_buff[sizeof(uint32_t)] = { 0 };
static device_info_resp_t m_device_info_resp = { 0 };
static uint16_t m_current_device_info_id = 0;
static uint16_t m_current_delete_device_id = 0;

ret_code_t get_device_count_stop(size_t data_size)
{
	uint8_t idx = 0;
	uint32_t device_count = device_list_get_device_count();

	m_device_count_buff[idx++] = (uint8_t)((device_count >> 0) & 0x000000FF);
	m_device_count_buff[idx++] = (uint8_t)((device_count >> 8) & 0x000000FF);
	m_device_count_buff[idx++] = (uint8_t)((device_count >> 16) & 0x000000FF);
	m_device_count_buff[idx++] = (uint8_t)((device_count >> 24) & 0x000000FF);

	return spi_protocol_tx_set_data_to_send((uint8_t *)(&m_device_count_buff),
						sizeof(m_device_count_buff), GET_DEVICE_COUNT);
}

ret_code_t get_device_info_device_id_start(size_t data_size)
{
	m_current_device_info_id = 0;

	if (data_size != sizeof(m_current_device_info_id)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	return NRF_SUCCESS;
}

ret_code_t get_device_info_device_id_receive(uint32_t data_index, uint8_t data)
{
	if (data_index > sizeof(m_current_device_info_id)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_current_device_info_id |= data << ((sizeof(uint16_t) - 1 - data_index) * 8);
	return NRF_SUCCESS;
}

ret_code_t get_device_info_stop(size_t data_size)
{
	memset(&m_device_info_resp, 0, sizeof(m_device_info_resp));
	ret_code_t err_code = device_list_get_device_info(m_current_device_info_id,
							  &m_device_info_resp.device_info);

	if (err_code == NRF_SUCCESS) {
		m_device_info_resp.status = DEVICE_INFO_STATUS_SUCCESS;
	} else if (err_code == NRF_ERROR_NOT_FOUND) {
		m_device_info_resp.status = DEVICE_INFO_STATUS_NOT_FOUND;
	} else if (err_code == NRF_ERROR_INVALID_DATA) {
		m_device_info_resp.status = DEVICE_INFO_STATUS_NOT_READY;
	} else {
		m_device_info_resp.status = DEVICE_INFO_STATUS_UNKNOWN;
	}

	err_code = spi_protocol_tx_set_data_to_send((uint8_t *)(&m_device_info_resp),
						    sizeof(m_device_info_resp), GET_DEVICE_INFO);

	return err_code;
}

ret_code_t delete_device_device_id_start(size_t data_size)
{
	m_current_delete_device_id = 0;

	if (data_size != sizeof(m_current_delete_device_id)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	return NRF_SUCCESS;
}

ret_code_t delete_device_device_id_receive(uint32_t data_index, uint8_t data)
{
	if (data_index > sizeof(m_current_delete_device_id)) {
		return NRF_ERROR_INVALID_PARAM;
	}

	m_current_delete_device_id |= data << ((sizeof(uint16_t) - 1 - data_index) * 8);
	return NRF_SUCCESS;
}

ret_code_t delete_device_stop(size_t data_size)
{
	ret_code_t err_code = device_list_delete_device(m_current_delete_device_id);

	return err_code;
}

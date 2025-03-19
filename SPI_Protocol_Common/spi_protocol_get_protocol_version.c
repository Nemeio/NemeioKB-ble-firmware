#include "spi_protocol_get_protocol_version.h"
#include "spi_protocol_tx.h"
#include "spi_protocol.h"

static uint8_t m_protocol_version_buff = PROTOCOL_VERSION;

ret_code_t get_protocol_version_stop(size_t data_size)
{
	return spi_protocol_tx_set_data_to_send(
		&m_protocol_version_buff, sizeof(m_protocol_version_buff), GET_PROTOCOL_VERSION);
}

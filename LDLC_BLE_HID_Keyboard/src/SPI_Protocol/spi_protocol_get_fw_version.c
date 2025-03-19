#include "spi_protocol_get_fw_version.h"
#include "spi_protocol_tx.h"
#include "spi_protocol.h"
#include "application_defs.h"

static firmware_version_t m_fw_version = { APP_MAJOR_NUMBER, APP_BUILD_NUMBER };

ret_code_t get_fw_version_stop(size_t data_size)
{
	return spi_protocol_tx_set_data_to_send((uint8_t *)(&m_fw_version), sizeof(m_fw_version),
						GET_FW_VERSION);
}

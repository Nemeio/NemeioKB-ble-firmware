#include "spi_protocol_factory_reset.h"
#include "spi_protocol_tx.h"
#include "spi_protocol.h"
#include "factory_reset.h"

ret_code_t factory_reset_stop(size_t data_size)
{
	ret_code_t retCode = factory_reset();

	uint8_t retCodeCmd = NRF_SUCCESS == retCode ? (uint8_t)FACTORY_RESET_SUCCESS :
							    (uint8_t)FACTORY_RESET_UNKNOWN;
	return spi_protocol_tx_set_data_to_send(&retCodeCmd, sizeof(retCodeCmd), FACTORY_RESET);
}

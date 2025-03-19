#include "spi_protocol_numeric_comparison.h"
#include "spi_protocol.h"
#include "spi_protocol_tx.h"

ret_code_t spi_protocol_pairing_abort()
{
	ret_code_t ret = NRF_SUCCESS;
	uint8_t dummy_param = 0;

	return spi_protocol_tx_set_data_to_send(&dummy_param, sizeof(dummy_param), PAIRING_ABORT);
}

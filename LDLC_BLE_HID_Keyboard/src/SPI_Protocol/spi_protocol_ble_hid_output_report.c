#include "spi_protocol_ble_hid_output_report.h"
#include "spi_protocol.h"
#include "spi_protocol_tx.h"

ret_code_t spi_protocol_output_report(const uint8_t *p_data, size_t length)
{
	return spi_protocol_tx_set_data_to_send(p_data, length, HID_OUTPUT_REPORT);
}
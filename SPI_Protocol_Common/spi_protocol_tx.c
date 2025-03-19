#include "spi_protocol_tx.h"
#include "spi_protocol.h"
#include "app_fifo.h"
#include "app_error.h"

static uint8_t m_current_tx_buff_length = 0;
static uint8_t m_current_tx_buff_read_offset = 0;

#define TX_BUFFER_LEN (4096)
static uint8_t m_tx_fifo_buffer[TX_BUFFER_LEN];
static app_fifo_t m_tx_fifo;
static uint8_t m_tx_buffer[SPI_BUFFER_MAX_LEN];

/* TX protocol :
 * The commands are sent as a stream, see frame description below.
 * Theses frames are sent into packets in the SPI TX buffer.
 * The first byte in the SPI TX buffer gives the packet length.
 */

/* Frame :
 *|   0    |       1      |     2      |   3    |      4      |   5    ...   N    | N + 1  |
 *|********|**************|************|********|*************|*******************|********|
 *|  SOF   | frame length |total length| cmd id | payload ofs |      payload      |  EOF   |
 */

/* SPI TX buffer :
 *|         0        |       1 ... N       |
 *|******************|*********************|
 *|  packet length   |     packet data     | 
 */

#define SPI_PROTOCOL_TX_SOF '\x01'
#define SPI_PROTOCOL_TX_EOF '\x04'

#define PACKET_DEFAULT_VALUE 0x00
#define PACKET_DATA_LENGTH_SIZE sizeof(uint8_t)
#define PACKET_DATA_LENGTH_IDX (0)
#define PACKET_MAX_LEN (SPI_BUFFER_MAX_LEN - PACKET_DATA_LENGTH_SIZE)

#define FRAME_SOF_SIZE sizeof(char)
#define FRAME_LENGTH_SIZE sizeof(uint8_t)
#define FRAME_TOTAL_LENGTH_SIZE sizeof(uint16_t)
#define FRAME_COMMAND_ID_SIZE sizeof(uint8_t)
#define FRAME_OFFSET_SIZE sizeof(uint16_t)

#define FRAME_HEADER_SIZE                                                                          \
	(FRAME_SOF_SIZE + FRAME_LENGTH_SIZE + FRAME_TOTAL_LENGTH_SIZE + FRAME_COMMAND_ID_SIZE +    \
	 FRAME_OFFSET_SIZE)

#define FRAME_EOF_SIZE sizeof(char)
#define FRAME_FOOTER_SIZE (FRAME_EOF_SIZE)

#define FRAME_MAX_DATA_LENGTH (PACKET_MAX_LEN - FRAME_HEADER_SIZE - FRAME_FOOTER_SIZE)

static ret_code_t prepare_buffer(const uint8_t *p_data, uint32_t data_length,
				 uint16_t total_data_length, uint8_t command_id,
				 uint16_t data_offset)
{
	uint32_t fifo_data_write_size = data_length;
	ret_code_t err_code = NRF_SUCCESS;

	if (p_data == NULL) {
		return NRF_ERROR_INVALID_PARAM;
	}

	if (data_length > FRAME_MAX_DATA_LENGTH || data_offset + data_length > total_data_length) {
		return NRF_ERROR_NO_MEM;
	}

	err_code = app_fifo_put(&m_tx_fifo, SPI_PROTOCOL_TX_SOF);
	APP_ERROR_CHECK(err_code);

	err_code = app_fifo_put(&m_tx_fifo, FRAME_TOTAL_LENGTH_SIZE + FRAME_COMMAND_ID_SIZE +
						    FRAME_OFFSET_SIZE + data_length +
						    FRAME_FOOTER_SIZE);
	APP_ERROR_CHECK(err_code);

	err_code = app_fifo_put(&m_tx_fifo, (uint8_t)((total_data_length >> 8) & 0x00FF));
	APP_ERROR_CHECK(err_code);
	err_code = app_fifo_put(&m_tx_fifo, (uint8_t)(total_data_length & 0x00FF));
	APP_ERROR_CHECK(err_code);

	err_code = app_fifo_put(&m_tx_fifo, command_id);
	APP_ERROR_CHECK(err_code);

	err_code = app_fifo_put(&m_tx_fifo, (uint8_t)((data_offset >> 8) & 0x00FF));
	APP_ERROR_CHECK(err_code);
	err_code = app_fifo_put(&m_tx_fifo, (uint8_t)(data_offset & 0x00FF));
	APP_ERROR_CHECK(err_code);

	err_code = app_fifo_write(&m_tx_fifo, p_data, &fifo_data_write_size);
	if (fifo_data_write_size != data_length) {
		err_code = NRF_ERROR_NO_MEM;
	}
	APP_ERROR_CHECK(err_code);

	err_code = app_fifo_put(&m_tx_fifo, SPI_PROTOCOL_TX_EOF);
	APP_ERROR_CHECK(err_code);

	return NRF_SUCCESS;
}

static ret_code_t spi_protocol_tx_set_fifo(const uint8_t *p_buffer, uint16_t data_length,
					   uint8_t command_id)
{
	ret_code_t err_code = NRF_SUCCESS;
	uint16_t nb_frames = data_length / FRAME_MAX_DATA_LENGTH +
			     (data_length % FRAME_MAX_DATA_LENGTH != 0 ? 1 : 0);
	uint16_t total_length = nb_frames * (FRAME_HEADER_SIZE + FRAME_FOOTER_SIZE) + data_length;
	uint32_t fifo_write_available_size = 0;
	uint16_t remaining_data_length = data_length;

	if (p_buffer == NULL) {
		return NRF_ERROR_INVALID_PARAM;
	}

	// Check FIFO size
	app_fifo_write(&m_tx_fifo, NULL, &fifo_write_available_size);
	if (total_length <= fifo_write_available_size) {
		while (remaining_data_length > 0) {
			uint16_t data_chunk_length =
				MIN(FRAME_MAX_DATA_LENGTH, remaining_data_length);

			err_code = prepare_buffer(p_buffer, data_chunk_length, data_length,
						  command_id, data_length - remaining_data_length);
			if (err_code != NRF_SUCCESS) {
				break;
			}

			remaining_data_length -= data_chunk_length;
			p_buffer += data_chunk_length;
		}
	} else {
		err_code = NRF_ERROR_NO_MEM;
	}

	return err_code;
}

ret_code_t spi_protocol_tx_set_data_to_send(const uint8_t *p_buffer, uint32_t data_length,
					    uint8_t command_id)
{
	ret_code_t ret_code = NRF_SUCCESS;

	while (NRF_SUCCESS == ret_code && data_length) {
		uint16_t chunk_size = MIN(data_length, UINT16_MAX);
		ret_code = spi_protocol_tx_set_fifo(p_buffer, chunk_size, command_id);

		p_buffer += chunk_size;
		data_length -= chunk_size;
	}

	if (NRF_SUCCESS == ret_code) {
		spi_protocol_notify_data_to_send(true);
	}

	return ret_code;
}

static uint32_t shift_tx_data(uint8_t *p_tx_buffer, uint8_t data_shift_offset)
{
	uint8_t data_to_shift_length = PACKET_MAX_LEN - data_shift_offset;
	uint32_t packet_data_length = 0;
	uint32_t read_fifo_size = 0;
	uint8_t *p_current_tx_buffer = p_tx_buffer;
	uint32_t available_space_in_buffer = 0;

	memmove(p_current_tx_buffer, p_current_tx_buffer + data_shift_offset, data_to_shift_length);
	p_current_tx_buffer += data_to_shift_length;
	available_space_in_buffer = data_shift_offset;

	read_fifo_size = available_space_in_buffer;
	app_fifo_read(&m_tx_fifo, p_current_tx_buffer, &read_fifo_size);
	p_current_tx_buffer += read_fifo_size;
	available_space_in_buffer -= read_fifo_size;

	packet_data_length = PACKET_MAX_LEN - available_space_in_buffer;

	memset(p_current_tx_buffer, PACKET_DEFAULT_VALUE, available_space_in_buffer);

	return packet_data_length;
}

static void reset_tx_buffer()
{
	m_current_tx_buff_read_offset = 0;
	m_current_tx_buff_length = 0;
	memset(m_tx_buffer, PACKET_DEFAULT_VALUE, SPI_BUFFER_MAX_LEN);
}

void spi_protocol_tx_data_sent(uint8_t tx_amount)
{
	uint32_t read_size = PACKET_MAX_LEN;

	if (tx_amount <= PACKET_DATA_LENGTH_SIZE) {
		return;
	}

	tx_amount -= PACKET_DATA_LENGTH_SIZE;

	uint8_t *p_packet_data = m_tx_buffer + PACKET_DATA_LENGTH_SIZE;
	uint8_t packet_data_length = 0;

	m_current_tx_buff_read_offset += tx_amount;
	if (m_current_tx_buff_read_offset < m_current_tx_buff_length) {
		packet_data_length = shift_tx_data(p_packet_data, tx_amount);
	} else {
		reset_tx_buffer();
		app_fifo_read(&m_tx_fifo, p_packet_data, &read_size);
		m_current_tx_buff_length = read_size;
		packet_data_length = read_size;
	}

	m_tx_buffer[PACKET_DATA_LENGTH_IDX] = packet_data_length;

	spi_protocol_notify_data_to_send(packet_data_length != 0);
}

void spi_protocol_tx_init()
{
	reset_tx_buffer();
	app_fifo_init(&m_tx_fifo, m_tx_fifo_buffer, sizeof(m_tx_fifo_buffer));
}

uint8_t *spi_protocol_tx_get_tx_buff()
{
	return m_tx_buffer;
}
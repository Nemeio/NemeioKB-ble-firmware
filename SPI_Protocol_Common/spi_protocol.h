#ifndef SPI_PROTOCOL_H
#define SPI_PROTOCOL_H

#include <stdbool.h>
#include "sdk_errors.h"

#define PROTOCOL_VERSION 2

#define SPI_BUFFER_MAX_LEN ((uint8_t)UINT8_MAX)

#define UPDATE_HID_REPORT_DESCRIPTOR 0x01
#define SEND_HID_INPUT_REPORT 0x02
#define SET_HIBERNATION 0x03
#define SET_ADVERTISING 0x04
#define CONFIGURE_ADVERTISING 0x05
#define SET_RADIO_POWER 0x06
#define DELETE_WHITELIST 0x07
#define DELETE_BONDS 0x08
#define GET_DEVICE_COUNT 0x09
#define GET_DEVICE_INFO 0x0A
#define DELETE_DEVICE 0x0B
#define START_DFU 0x0C
#define DFU_DATA 0x0D
#define SERIAL_DATA 0x0E
#define GET_FW_VERSION 0x0F
#define NUMERIC_COMPARISON 0x10
#define PAIRING_ABORT 0x11
#define HID_OUTPUT_REPORT 0x12
#define BLE_EVENT 0x13
#define GET_PROTOCOL_VERSION 0x14
#define FACTORY_RESET 0x15
#define SET_MAC_ADDRESS 0x16
#define SET_RADIO_TEST 0x17
#define SET_DEVICE_NAME 0x18
#define SET_DEVICE_PROPERTIES 0x19
#define COMMIT_VERSION 0x1A
#define NRF_STARTED 0x1B
#define ENTERED_DFU 0x1C

typedef struct spi_xfer_done_event {
	uint8_t rx_buf[SPI_BUFFER_MAX_LEN];
	uint8_t xfer_len;
} spi_xfer_done_event_t;

#define SPI_PROTOCOL_EVENT_DATA_SIZE sizeof(spi_xfer_done_event_t)

ret_code_t spi_protocol_init();

ret_code_t spi_protocol_uninit();

ret_code_t spi_protocol_reset_tx_buffer();

ret_code_t spi_protocol_set_tx_buffer(uint8_t *p_data, uint8_t len);

void spi_protocol_notify_data_to_send(bool b_data_to_send);

void spi_protocol_test_bidir();

#endif // SPI_PROTOCOL_H

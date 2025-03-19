#ifndef SPI_PROTOCOL_COMMON_ACTIONS_H
#define SPI_PROTOCOL_COMMON_ACTIONS_H

#include "spi_protocol.h"

typedef ret_code_t (*pfn_command_parameter_receive_t)(uint32_t data_index, uint8_t data);

typedef ret_code_t (*pfn_protocol_start_stop_t)(size_t data_size);

typedef struct _parameter_funcs_t {
	pfn_protocol_start_stop_t pfn_command_parameter_start;
	pfn_command_parameter_receive_t pfn_command_parameter_receive;
	pfn_protocol_start_stop_t pfn_command_parameter_stop;
} parameter_funcs_t;

#define SPI_PROTOCOL_MAX_PARAMETERS 3

typedef struct {
	int16_t command; /**< Command value. */
	pfn_protocol_start_stop_t pfn_command_data_start; /**< Command Executed at state start. */
	pfn_protocol_start_stop_t pfn_command_data_stop; /**< Command Executed at state ends. */
	uint16_t nb_parameters;
	parameter_funcs_t parameter_funcs
		[SPI_PROTOCOL_MAX_PARAMETERS]; /**< Command Executed at data reception. */
} spi_protocol_parse_action;

#ifdef NEMEIO_BOOTLOADER
#include "spi_protocol_bootloader_actions.h"
#else
#include "spi_protocol_app_actions.h"
#endif

#endif // SPI_PROTOCOL_COMMON_ACTIONS_H

#ifndef SPI_PROTOCOL_APP_ACTIONS_H
#define SPI_PROTOCOL_APP_ACTIONS_H

#include "spi_protocol_ble_hid_report_map.h"
#include "spi_protocol_ble_hid_input_report.h"
#include "spi_protocol_ble_hid_advertising.h"
#include "spi_protocol_ble_hid_hibernation.h"
#include "spi_protocol_ble_hid_radio_power.h"
#include "spi_protocol_ble_hid_peer_management.h"
#include "spi_protocol_get_device_list.h"
#include "spi_protocol_dfu_cmd.h"
#include "spi_protocol_serial_data.h"
#include "spi_protocol_get_fw_version.h"
#include "spi_protocol_numeric_comparison.h"
#include "spi_protocol_get_protocol_version.h"
#include "spi_protocol_factory_reset.h"
#include "spi_protocol_ble_set_mac_address.h"
#include "spi_protocol_radio_test.h"
#include "spi_protocol_ble_set_device_name.h"
#include "spi_protocol_ble_set_device_properties.h"
#include "spi_protocol_dfu_commit_version.h"

#define UPDATE_HID_REPORT_PARSE_ACTION                                                             \
	{                                                                                          \
		UPDATE_HID_REPORT_DESCRIPTOR,                                                      \
			.pfn_command_data_start = update_hid_report_desc_start,                    \
			.pfn_command_data_stop = update_hid_report_desc_stop, .nb_parameters = 3,  \
			.parameter_funcs = {                                                       \
				{ .pfn_command_parameter_start = NULL,                             \
				  .pfn_command_parameter_receive =                                 \
					  update_hid_report_desc_get_report_count,                 \
				  .pfn_command_parameter_stop = NULL },                            \
				{ .pfn_command_parameter_start = NULL,                             \
				  .pfn_command_parameter_receive =                                 \
					  update_hid_report_desc_get_report_lengths,               \
				  .pfn_command_parameter_stop = NULL },                            \
				{ .pfn_command_parameter_start =                                   \
					  update_hid_report_desc_receive_start,                    \
				  .pfn_command_parameter_receive = update_hid_report_desc_receive, \
				  .pfn_command_parameter_stop = NULL }                             \
			}                                                                          \
	}

#define SEND_HID_INPUT_REPORT_PARSE_ACTION                                                         \
	{                                                                                          \
		SEND_HID_INPUT_REPORT, .pfn_command_data_start = send_hid_input_report_start,      \
				       .pfn_command_data_stop = send_hid_input_report_stop,        \
				       .nb_parameters = 2, .parameter_funcs = {                    \
					       { .pfn_command_parameter_start = NULL,              \
						 .pfn_command_parameter_receive =                  \
							 send_hid_input_report_id_receive,         \
						 .pfn_command_parameter_stop = NULL },             \
					       { .pfn_command_parameter_start =                    \
							 send_hid_input_report_data_receive_start, \
						 .pfn_command_parameter_receive =                  \
							 send_hid_input_report_data_receive,       \
						 .pfn_command_parameter_stop = NULL },             \
					       { .pfn_command_parameter_start = NULL,              \
						 .pfn_command_parameter_receive = NULL,            \
						 .pfn_command_parameter_stop = NULL }              \
				       }                                                           \
	}

#define SET_ADVERTISING_PARSE_ACTION                                                               \
	{                                                                                          \
		.command = SET_ADVERTISING, .pfn_command_data_start = NULL,                        \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = set_advertising_data_receive,           \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define SET_HIBERNATION_PARSE_ACTION                                                               \
	{                                                                                          \
		.command = SET_HIBERNATION, .pfn_command_data_start = NULL,                        \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = set_hibernation_action,                   \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define SET_RADIO_POWER_PARSE_ACTION                                                               \
	{                                                                                          \
		.command = SET_RADIO_POWER, .pfn_command_data_start = NULL,                        \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = set_radio_power_data_receive,           \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define CONFIGURE_ADVERTISING_PARSE_ACTION                                                         \
	{                                                                                          \
		.command = CONFIGURE_ADVERTISING, .pfn_command_data_start = NULL,                  \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = configure_advertising_data_receive,     \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define DELETE_WHITELIST_PARSE_ACTION                                                              \
	{                                                                                          \
		.command = DELETE_WHITELIST, .pfn_command_data_start = NULL,                       \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = delete_whitelist_action,                  \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define DELETE_BONDS_PARSE_ACTION                                                                  \
	{                                                                                          \
		.command = DELETE_BONDS, .pfn_command_data_start = NULL,                           \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = delete_bonds_action,                      \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define GET_DEVICE_COUNT_PARSE_ACTION                                                              \
	{                                                                                          \
		.command = GET_DEVICE_COUNT, .pfn_command_data_start = NULL,                       \
		.pfn_command_data_stop = get_device_count_stop, .nb_parameters = 1,                \
		.parameter_funcs = {                                                               \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define GET_DEVICE_INFO_PARSE_ACTION                                                               \
	{                                                                                          \
		.command = GET_DEVICE_INFO, .pfn_command_data_start = NULL,                        \
		.pfn_command_data_stop = get_device_info_stop, .nb_parameters = 1,                 \
		.parameter_funcs = {                                                               \
			{ .pfn_command_parameter_start = get_device_info_device_id_start,          \
			  .pfn_command_parameter_receive = get_device_info_device_id_receive,      \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define DELETE_DEVICE_PARSE_ACTION                                                                 \
	{                                                                                          \
		.command = DELETE_DEVICE, .pfn_command_data_start = NULL,                          \
		.pfn_command_data_stop = delete_device_stop, .nb_parameters = 1,                   \
		.parameter_funcs = {                                                               \
			{ .pfn_command_parameter_start = delete_device_device_id_start,            \
			  .pfn_command_parameter_receive = delete_device_device_id_receive,        \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define START_DFU_PARSE_ACTION                                                                     \
	{                                                                                          \
		.command = START_DFU, .pfn_command_data_start = NULL,                              \
		.pfn_command_data_stop = spi_protocol_start_dfu_stop, .nb_parameters = 1,          \
		.parameter_funcs = {                                                               \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define SERIAL_DATA_PARSE_ACTION                                                                   \
	{                                                                                          \
		.command = SERIAL_DATA, .pfn_command_data_start = NULL,                            \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = spi_protocol_serial_data_receive,       \
			  .pfn_command_parameter_stop = spi_protocol_serial_data_stop },           \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define GET_FW_VERSION_PARSE_ACTION                                                                \
	{                                                                                          \
		.command = GET_FW_VERSION, .pfn_command_data_start = NULL,                         \
		.pfn_command_data_stop = get_fw_version_stop, .nb_parameters = 1,                  \
		.parameter_funcs = {                                                               \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define NUMERIC_COMPARISON_PARSE_ACTION                                                            \
	{                                                                                          \
		.command = NUMERIC_COMPARISON, .pfn_command_data_start = NULL,                     \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive =                                         \
				  spi_protocol_numeric_comparison_reply_receive,                   \
			  .pfn_command_parameter_stop =                                            \
				  spi_protocol_numeric_comparison_reply_stop },                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define GET_PROTOCOL_VERSION_PARSE_ACTION                                                          \
	{                                                                                          \
		.command = GET_PROTOCOL_VERSION, .pfn_command_data_start = NULL,                   \
		.pfn_command_data_stop = get_protocol_version_stop, .nb_parameters = 1,            \
		.parameter_funcs = {                                                               \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define FACTORY_RESET_PARSE_ACTION                                                                 \
	{                                                                                          \
		.command = FACTORY_RESET, .pfn_command_data_start = NULL,                          \
		.pfn_command_data_stop = factory_reset_stop, .nb_parameters = 1,                   \
		.parameter_funcs = {                                                               \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL }                                     \
		}                                                                                  \
	}

#define SET_MAC_ADDRESS_PARSE_ACTION                                                               \
	{                                                                                          \
		.command = SET_MAC_ADDRESS, .pfn_command_data_start = NULL,                        \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = set_mac_address_start,                    \
			  .pfn_command_parameter_receive = set_mac_address_receive,                \
			  .pfn_command_parameter_stop = set_mac_address_stop },                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
		}                                                                                  \
	}

#define SET_RADIO_TEST_PARSE_ACTION                                                                \
	{                                                                                          \
		.command = SET_RADIO_TEST, .pfn_command_data_start = NULL,                         \
		.pfn_command_data_stop = set_radio_test_stop, .nb_parameters = 1,                  \
		.parameter_funcs = {                                                               \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = set_radio_test_mode_receive,            \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
		}                                                                                  \
	}

#define SET_DEVICE_NAME_PARSE_ACTION                                                               \
	{                                                                                          \
		.command = SET_DEVICE_NAME, .pfn_command_data_start = NULL,                        \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = set_device_name_start,                    \
			  .pfn_command_parameter_receive = set_device_name_receive,                \
			  .pfn_command_parameter_stop = set_device_name_stop },                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
		}                                                                                  \
	}

#define SET_DEVICE_PROPERTIES_PARSE_ACTION                                                         \
	{                                                                                          \
		.command = SET_DEVICE_PROPERTIES, .pfn_command_data_start = NULL,                  \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start =                                           \
				  set_device_properties_protocol_version_start,                    \
			  .pfn_command_parameter_receive =                                         \
				  set_device_properties_protocol_version_receive,                  \
			  .pfn_command_parameter_stop =                                            \
				  set_device_properties_protocol_version_stop },                   \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
		}                                                                                  \
	}

#define DFU_COMMIT_VERSION_ACTION                                                                  \
	{                                                                                          \
		.command = COMMIT_VERSION, .pfn_command_data_start = NULL,                         \
		.pfn_command_data_stop = NULL, .nb_parameters = 1, .parameter_funcs = {            \
			{ .pfn_command_parameter_start = spi_protocol_dfu_commit_version_start,    \
			  .pfn_command_parameter_receive =                                         \
				  spi_protocol_dfu_commit_version_receive,                         \
			  .pfn_command_parameter_stop = spi_protocol_dfu_commit_version_stop },    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
			{ .pfn_command_parameter_start = NULL,                                     \
			  .pfn_command_parameter_receive = NULL,                                   \
			  .pfn_command_parameter_stop = NULL },                                    \
		}                                                                                  \
	}

static spi_protocol_parse_action m_actions[] = {
	UPDATE_HID_REPORT_PARSE_ACTION, // Update HID report descriptor
	SEND_HID_INPUT_REPORT_PARSE_ACTION, // Send HID report
	SET_ADVERTISING_PARSE_ACTION, //start and stop advertising
	SET_HIBERNATION_PARSE_ACTION, //enter hibernation mode
	SET_RADIO_POWER_PARSE_ACTION, //start and stop radio device
	CONFIGURE_ADVERTISING_PARSE_ACTION, //Configure advertising parameters
	DELETE_WHITELIST_PARSE_ACTION, //clear whitelist
	DELETE_BONDS_PARSE_ACTION, //delete all bonds
	GET_DEVICE_COUNT_PARSE_ACTION,	    GET_DEVICE_INFO_PARSE_ACTION,
	DELETE_DEVICE_PARSE_ACTION,	    START_DFU_PARSE_ACTION,
	SERIAL_DATA_PARSE_ACTION,	    GET_FW_VERSION_PARSE_ACTION,
	NUMERIC_COMPARISON_PARSE_ACTION,    GET_PROTOCOL_VERSION_PARSE_ACTION,
	FACTORY_RESET_PARSE_ACTION,	    SET_MAC_ADDRESS_PARSE_ACTION,
#ifdef CEM_TESTS
	SET_RADIO_TEST_PARSE_ACTION,
#endif // CEM_TESTS
	SET_DEVICE_NAME_PARSE_ACTION,	    SET_DEVICE_PROPERTIES_PARSE_ACTION,
	DFU_COMMIT_VERSION_ACTION,	    { -1, NULL, NULL, 0, { NULL, NULL } },
};

#endif // SPI_PROTOCOL_APP_ACTIONS_H

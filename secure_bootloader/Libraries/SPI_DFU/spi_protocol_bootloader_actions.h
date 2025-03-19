#ifndef SPI_PROTOCOL_BOOTLOADER_ACTIONS
#define SPI_PROTOCOL_BOOTLOADER_ACTIONS

#include "spi_protocol_dfu_cmd.h"
#include "spi_protocol_dfu_data.h"
#include "spi_protocol_get_protocol_version.h"

#define START_DFU_PARSE_ACTION               {START_DFU, \
                                              NULL, \
                                              spi_protocol_start_dfu_stop, \
                                              1, \
                                                {{NULL, NULL, NULL}, \
                                                {NULL, NULL, NULL}, \
                                                  {NULL, NULL, NULL}}}

#define DFU_DATA_PARSE_ACTION                {DFU_DATA, \
                                              NULL, \
                                              NULL, \
                                              1, \
                                                {{NULL, spi_protocol_dfu_data_receive, NULL}, \
                                                {NULL, NULL, NULL}, \
                                                  {NULL, NULL, NULL}}}
												  
#define GET_PROTOCOL_VERSION_PARSE_ACTION    {GET_PROTOCOL_VERSION, \
                                              NULL, \
                                              get_protocol_version_stop, \
                                              1, \
                                                {{NULL, NULL, NULL}, \
                                                {NULL, NULL, NULL}, \
                                                  {NULL, NULL, NULL}}}

static spi_protocol_parse_action m_actions[] = {
  START_DFU_PARSE_ACTION,
  DFU_DATA_PARSE_ACTION,
  GET_PROTOCOL_VERSION_PARSE_ACTION,
  {-1, NULL, NULL, 0, {NULL, NULL}},
};


#endif // SPI_PROTOCOL_BOOTLOADER_ACTIONS
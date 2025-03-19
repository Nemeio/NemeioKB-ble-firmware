#ifndef APPLICATION_DEFS_H
#define APPLICATION_DEFS_H

#include "ble_srv_common.h"

typedef struct {
	uint16_t major;
	uint8_t minor;
} __packed firmware_version_t;

#define VERSION_MAX_SZ 16
#define APP_MAJOR_NUMBER APP_MAJOR_NUMBER_DEF
#define APP_BUILD_NUMBER APP_BUILD_NUMBER_DEF

#define DEFAULT_CHAR_SECURITY_LEVEL SEC_MITM_LESC

#endif // APPLICATION_DEFS_H

#ifndef NRF_DFU_COMMIT_VERSION_H
#define NRF_DFU_COMMIT_VERSION_H

#include "nrf_bootloader_info.h"

#define BOOTLOADER_DFU_COMMIT_VERSION_BIT_MASK           (0x02)      /**< Bit mask to signal from main application to commit pending version. */

#define BOOTLOADER_DFU_COMMIT_VERSION   (BOOTLOADER_DFU_GPREGRET | BOOTLOADER_DFU_COMMIT_VERSION_BIT_MASK)      /**< Magic number to signal that bootloader should commit pending version mode because of signal from main app.*/

#endif //NRF_DFU_COMMIT_VERSION_H
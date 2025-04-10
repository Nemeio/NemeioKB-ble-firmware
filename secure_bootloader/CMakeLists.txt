cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER_WORKS 1)

set(CMAKE_SYSTEM_NAME FreeRTOS)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_LD arm-none-eabi-ld)

project(LDLC-Karmeliet-nRF-secure-bootloader C ASM)

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(BOOTLOADER_BINARY_NAME LDLC-Karmeliet-nRF-secure-bootloader)

if(NOT DEFINED SOURCE_KEY)
    message(FATAL_ERROR "Source key was not provided")
endif(NOT DEFINED SOURCE_KEY)

set(BOOTLOADER_BINARY_NAME ${BOOTLOADER_BINARY_NAME}-${SOURCE_KEY})

if(INTERNAL_ENABLE_SECURE_BOOT)
    set(BOOTLOADER_BINARY_NAME ${BOOTLOADER_BINARY_NAME}-APPROTECT)
    add_compile_options(-DENABLE_APPROTECT)
    message("Secure bootloader is built (APPROTECT support)")
else(INTERNAL_ENABLE_SECURE_BOOT)
    set(BOOTLOADER_BINARY_NAME ${BOOTLOADER_BINARY_NAME}-UNSECURE)
    message("Unsecure bootloader is built")
endif(INTERNAL_ENABLE_SECURE_BOOT)

include_directories(./karmeliet_spi/config)
include_directories(./../nRF5_SDK/components/boards)
include_directories(./../nRF5_SDK/components/drivers_nrf/nrf_soc_nosd)
include_directories(./../nRF5_SDK/components/libraries/atomic)
include_directories(./../nRF5_SDK/components/libraries/balloc)
include_directories(./../nRF5_SDK/components/libraries/bootloader)
include_directories(./../nRF5_SDK/components/libraries/bootloader/dfu)
include_directories(./../nRF5_SDK/components/libraries/bootloader/serial_dfu)
include_directories(./../nRF5_SDK/components/libraries/crc32)
include_directories(./../nRF5_SDK/components/libraries/crypto)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/cc310)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/cc310_bl)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/cifra)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/mbedtls)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/micro_ecc)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/nrf_hw)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/nrf_sw)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/oberon)
include_directories(./../nRF5_SDK/components/libraries/crypto/backend/optiga)
include_directories(./../nRF5_SDK/components/libraries/delay)
include_directories(./../nRF5_SDK/components/libraries/experimental_section_vars)
include_directories(./../nRF5_SDK/components/libraries/fifo)
include_directories(./../nRF5_SDK/components/libraries/fstorage)
include_directories(./../nRF5_SDK/components/libraries/log)
include_directories(./../nRF5_SDK/components/libraries/log/src)
include_directories(./../nRF5_SDK/components/libraries/mem_manager)
include_directories(./../nRF5_SDK/components/libraries/memobj)
include_directories(./../nRF5_SDK/components/libraries/queue)
include_directories(./../nRF5_SDK/components/libraries/ringbuf)
include_directories(./../nRF5_SDK/components/libraries/scheduler)
include_directories(./../nRF5_SDK/components/libraries/sha256)
include_directories(./../nRF5_SDK/components/libraries/slip)
include_directories(./../nRF5_SDK/components/libraries/stack_info)
include_directories(./../nRF5_SDK/components/libraries/strerror)
include_directories(./../nRF5_SDK/components/libraries/util)
include_directories(./../nRF5_SDK/components/softdevice/s132/headers/nrf52)
include_directories(./../nRF5_SDK/components/softdevice/s132/headers)
include_directories(./../nRF5_SDK/components/toolchain/cmsis/include)
include_directories(./../nRF5_SDK/external/fprintf)
include_directories(./../nRF5_SDK/external/micro-ecc/micro-ecc)
include_directories(./../nRF5_SDK/external/nano-pb)
include_directories(./../nRF5_SDK/external/nrf_oberon)
include_directories(./../nRF5_SDK/external/nrf_oberon/include)
include_directories(./../nRF5_SDK/integration/nrfx)
include_directories(./../nRF5_SDK/integration/nrfx/legacy)
include_directories(./../nRF5_SDK/modules/nrfx)
include_directories(./../nRF5_SDK/modules/nrfx/drivers/include)
include_directories(./../nRF5_SDK/modules/nrfx/hal)
include_directories(./../nRF5_SDK/modules/nrfx/mdk)
include_directories(./../SPI_Protocol_Common)
include_directories(./Libraries/SPI_DFU)
include_directories(../nRF5_SDK/external/nrf_oberon/)

include_directories(../secure_bootloader/Libraries/bootloader_custom/dfu)

add_compile_options(-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_compile_options(-D__packed=__attribute__\(\(packed\)\))
add_compile_options(-DBOARD_PCA10040 -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD -DMBR_PRESENT -DNRF52 -DNRF52832_XXAA -DNRF52_PAN_74)
add_compile_options(-DNRF_DFU_SETTINGS_VERSION=130 -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION -DuECC_ENABLE_VLI_API=0 -DuECC_OPTIMIZATION_LEVEL=3)
add_compile_options(-DuECC_SQUARE_FUNC=0 -DuECC_SUPPORT_COMPRESSED_POINT=0 -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1 -DNEMEIO_BOOTLOADER)
add_compile_options(-D__STARTUP_CLEAR_BSS)
add_compile_options(-Os -g)
add_compile_options(-fdata-sections -ffunction-sections)

set(SOURCE_FILES ./dfu_public_key.c)
set(SOURCE_FILES ${SOURCE_FILES} ./main.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/boards/boards.c)
set(SOURCE_FILES ${SOURCE_FILES} ./Libraries/SPI_DFU/dfu_transport_spi.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../SPI_Protocol_Common/spi_protocol_dfu_cmd.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../SPI_Protocol_Common/spi_protocol_get_protocol_version.c)
set(SOURCE_FILES ${SOURCE_FILES} ./Libraries/SPI_DFU/spi_protocol_dfu_data.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/external/nano-pb/pb_common.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/external/nano-pb/pb_decode.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/modules/nrfx/mdk/gcc_startup_nrf52.S)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/modules/nrfx/mdk/system_nrf52.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/modules/nrfx/drivers/src/nrfx_gpiote.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/nrf_bootloader.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/nrf_bootloader_app_start.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/nrf_bootloader_app_start_final.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/nrf_bootloader_dfu_timers.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/nrf_bootloader_fw_activation.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/nrf_bootloader_info.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/nrf_bootloader_wdt.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_settings.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_validation.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_ver_validation.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/nrf_crypto_ecc.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/nrf_crypto_ecdsa.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/nrf_crypto_hash.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/nrf_crypto_init.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/nrf_crypto_shared.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/nrf_sw/nrf_sw_backend_hash.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_hash.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecc.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdh.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdsa.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/dfu-cc.pb.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_flash.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_handling_error.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_mbr.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_transport.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/dfu/nrf_dfu_utils.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/slip/slip.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/integration/nrfx/legacy/nrf_drv_spis.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/integration/nrfx/legacy/nrf_drv_uart.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/modules/nrfx/hal/nrf_nvmc.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/drivers_nrf/nrf_soc_nosd/nrf_soc.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/modules/nrfx/soc/nrfx_atomic.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/modules/nrfx/drivers/src/prs/nrfx_prs.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/modules/nrfx/drivers/src/nrfx_spis.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/modules/nrfx/drivers/src/nrfx_uart.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/modules/nrfx/drivers/src/nrfx_uarte.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/util/app_error_weak.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/fifo/app_fifo.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/scheduler/app_scheduler.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/util/app_util_platform.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/crc32/crc32.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/mem_manager/mem_manager.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/util/nrf_assert.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/atomic/nrf_atomic.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/balloc/nrf_balloc.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/external/fprintf/nrf_fprintf.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/external/fprintf/nrf_fprintf_format.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/fstorage/nrf_fstorage.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/fstorage/nrf_fstorage_nvmc.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/memobj/nrf_memobj.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/queue/nrf_queue.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/ringbuf/nrf_ringbuf.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/strerror/nrf_strerror.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/sha256/sha256.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/slip/slip.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/log/src/nrf_log_frontend.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/log/src/nrf_log_str_formatter.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../nRF5_SDK/components/libraries/bootloader/serial_dfu/nrf_dfu_serial.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../SPI_Protocol_Common/spi_protocol.c)
set(SOURCE_FILES ${SOURCE_FILES} ./../SPI_Protocol_Common/spi_protocol_tx.c)

add_executable(${BOOTLOADER_BINARY_NAME}  ${SOURCE_FILES})

add_library(liboberon STATIC IMPORTED)
set_target_properties(liboberon PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../nRF5_SDK/external/nrf_oberon/lib/cortex-m4/hard-float/short-wchar/liboberon_3.0.8.a)

add_library(libmicroecc STATIC IMPORTED)
set_target_properties(libmicroecc PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../nRF5_SDK/external/micro-ecc/nrf52hf_armgcc/armgcc/micro_ecc_lib_nrf52.a)

target_link_libraries(${BOOTLOADER_BINARY_NAME} liboberon libmicroecc)

set (CMAKE_EXE_LINKER_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -specs=nosys.specs -specs=nano.specs -Xlinker -L ${CMAKE_CURRENT_SOURCE_DIR}/../nRF5_SDK/modules/nrfx/mdk -L ${CMAKE_CURRENT_SOURCE_DIR} -Tgcc_nrf52.ld -Wl,-Map=output.map -Wl,--gc-sections")

add_custom_command(TARGET ${BOOTLOADER_BINARY_NAME}
 POST_BUILD
 COMMAND arm-none-eabi-objcopy -O ihex "${BOOTLOADER_BINARY_NAME}" "${BOOTLOADER_BINARY_NAME}.hex"
 COMMENT "Invoking: Binary file creation"
)

add_custom_command(TARGET ${BOOTLOADER_BINARY_NAME}
 POST_BUILD
 COMMAND arm-none-eabi-size --format=berkeley "${BOOTLOADER_BINARY_NAME}"
 COMMENT "Invoking: Cross ARM GNU Print Size"
)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER_WORKS 1)

set(CMAKE_SYSTEM_NAME FreeRTOS)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_LD arm-none-eabi-ld)

project(LDLC-Karmeliet-nRF C ASM)

include("../cmake-tools/tools.cmake")

if(VERSION)
    message("Use version ${VERSION}")
else()
    message(FATAL_ERROR "No version provided")
endif()

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

file(GLOB_RECURSE HEADERS  "./*.h"  "../nRF5_SDK/*.h" "../SPI_Protocol_Common/*.h")
list(FILTER HEADERS EXCLUDE REGEX "nrf_soc_nosd")

include_directories(LDLC_Keyboard/s132/config)
# Add include directories depending on the header files path
FOREACH(header ${HEADERS})
 get_filename_component(dir ${header} DIRECTORY)
 include_directories(${dir})
ENDFOREACH()

include_directories(../nRF5_SDK/external/mbedtls/include)
include_directories(../nRF5_SDK/external/nrf_oberon/)

include_directories(../secure_bootloader/Libraries/bootloader_custom/dfu)

add_compile_options(-mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16)
add_compile_options(-D__packed=__attribute__\(\(packed\)\))
add_compile_options(-DBLE_STACK_SUPPORT_REQD -DBOARD_PCA10040 -DCONFIG_GPIO_AS_PINRESET -DNRF52 -DNRF52832_XXAA -DNRF52_PAN_12)
add_compile_options(-DNRF52_PAN_15 -DNRF52_PAN_20 -DNRF52_PAN_31 -DNRF52_PAN_36 -DNRF52_PAN_51 -DNRF52_PAN_54 -DNRF52_PAN_55)
add_compile_options(-DNRF52_PAN_58 -DNRF52_PAN_64 -DNRF52_PAN_74 -DNRF_SD_BLE_API_VERSION=4 -DS132 -DSOFTDEVICE_PRESENT -DSWI_DISABLE0)
add_compile_options(-D__STARTUP_CLEAR_BSS)
add_compile_options(-DAPP_MAJOR_NUMBER_DEF=${APP_MAJOR_NUMBER} -DAPP_BUILD_NUMBER_DEF=${APP_BUILD_NUMBER})
add_compile_options(-DAPP_TIMER_V2 -DAPP_TIMER_V2_RTC1_ENABLED)
add_compile_options(-Ofast -g)
add_compile_options(-fdata-sections -ffunction-sections)

if(INTERNAL_CEM_TESTS)
    add_compile_options(-DCEM_TESTS)
endif()

if(INTERNAL_ROLLBACK_TEST)
    add_compile_options(-DROLLBACK_TEST)
endif()

file(GLOB_RECURSE SOURCE_FILES "**" "../SPI_Protocol_Common/**" "../nRF5_SDK/modules/nrfx/drivers/**" )

set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/external/mbedtls/library/aes.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/external/mbedtls/library/ctr_drbg.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/external/mbedtls/library/platform_util.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/modules/nrfx/mdk/gcc_startup_nrf52.S)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/modules/nrfx/mdk/system_nrf52.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/boards/boards.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/atomic/nrf_atomic.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/atomic_fifo/nrf_atfifo.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/atomic_flags/nrf_atflags.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/bsp/bsp.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/bsp/bsp_btn_ble.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/button/app_button.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/nrf_crypto_init.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/nrf_crypto_ecc.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/nrf_crypto_rng.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/nrf_crypto_ecdh.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_init.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/experimental_section_vars/nrf_section_iter.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/fds/fds.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/fstorage/nrf_fstorage.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/fstorage/nrf_fstorage_sd.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/util/app_error.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/util/app_util_platform.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/util/app_error_weak.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/ringbuf/nrf_ringbuf.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/scheduler/app_scheduler.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/sortlist/nrf_sortlist.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/timer/app_timer2.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/timer/drv_rtc.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/fifo/app_fifo.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/queue/nrf_queue.c)

set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/ble_advertising/ble_advertising.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/common/ble_advdata.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/common/ble_conn_params.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/common/ble_conn_state.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/common/ble_srv_common.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/ble_services/ble_hids/ble_hids.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/ble_services/ble_dis/ble_dis.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/ble_services/ble_nus/ble_nus.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/nrf_ble_gatt/nrf_ble_gatt.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/nrf_ble_qwr/nrf_ble_qwr.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/gatts_cache_manager.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/gatt_cache_manager.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/id_manager.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/peer_database.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/peer_data_storage.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/peer_id.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/peer_manager.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/peer_manager_handler.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/pm_buffer.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/nrf_ble_lesc.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/security_manager.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/ble/peer_manager/security_dispatcher.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/softdevice/common/nrf_sdh.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/softdevice/common/nrf_sdh_ble.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/softdevice/common/nrf_sdh_soc.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/integration/nrfx/legacy/nrf_drv_spis.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/integration/nrfx/legacy/nrf_drv_rng.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/integration/nrfx/legacy/nrf_drv_clock.c)

if(NRF_TRACE)
add_compile_options(-DNRF_LOG_ENABLED)
add_compile_options(-DNRF_LOG_BACKEND_SERIAL_USES_SWO=1)
add_compile_options(-DNRF_LOG_BACKEND_UART_ENABLED=0)
add_compile_options(-DNRF_LOG_BACKEND_RTT_ENABLED=1)
add_compile_options(-DENABLE_DEBUG_LOG_SUPPORT)
add_compile_options(-DENABLE_SWO)
add_compile_options(-DDEBUG)
add_compile_options(-DNRF_LOG_DEFERRED)
add_compile_options(-DNRF_LOG_DEFAULT_LEVEL=4)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/strerror/nrf_strerror.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/log/src/nrf_log_frontend.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/log/src/nrf_log_default_backends.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/log/src/nrf_log_backend_rtt.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/memobj/nrf_memobj.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/balloc/nrf_balloc.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/util/app_error_handler_gcc.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/log/src/nrf_log_backend_serial.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/components/libraries/log/src/nrf_log_str_formatter.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/external/fprintf/nrf_fprintf_format.c)
set(SOURCE_FILES ${SOURCE_FILES} ../nRF5_SDK/external/fprintf/nrf_fprintf.c)

include_directories(../SEGGER/RTT)
set(SOURCE_FILES ${SOURCE_FILES} ../SEGGER/RTT/SEGGER_RTT.c)
set(SOURCE_FILES ${SOURCE_FILES} ../SEGGER/RTT/SEGGER_RTT_printf.c)

endif(NRF_TRACE)

add_executable(LDLC-Karmeliet-nRF  ${SOURCE_FILES})

add_library(liboberon STATIC IMPORTED)
set_target_properties(liboberon PROPERTIES
	IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../nRF5_SDK/external/nrf_oberon/lib/cortex-m4/soft-float/liboberon_3.0.8.a)

target_link_libraries(LDLC-Karmeliet-nRF liboberon)

set (CMAKE_EXE_LINKER_FLAGS "-g -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -specs=nosys.specs -specs=nano.specs -Xlinker -L ${CMAKE_CURRENT_SOURCE_DIR}/../nRF5_SDK/modules/nrfx/mdk -L ${CMAKE_CURRENT_SOURCE_DIR} -Tgcc_nrf52.ld -Wl,-Map=output.map -Wl,--gc-sections")

add_custom_command(TARGET LDLC-Karmeliet-nRF
 POST_BUILD
 COMMAND arm-none-eabi-objcopy -O ihex "LDLC-Karmeliet-nRF" "LDLC-Karmeliet-nRF.hex"
 COMMENT "Invoking: Binary file creation"
)

add_custom_command(TARGET LDLC-Karmeliet-nRF
 POST_BUILD
 COMMAND arm-none-eabi-size --format=berkeley "LDLC-Karmeliet-nRF"
 COMMENT "Invoking: Cross ARM GNU Print Size"
)

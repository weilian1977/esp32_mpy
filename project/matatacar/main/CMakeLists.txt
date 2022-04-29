# Set location of base MicroPython directory.
if(NOT MICROPY_DIR)
    get_filename_component(MICROPY_DIR ${PROJECT_DIR}/../../submodule/micropython ABSOLUTE)
endif()

# Include core source components.
include(${MICROPY_DIR}/py/py.cmake)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(${MICROPY_DIR}/py/usermod.cmake)
    include(${MICROPY_DIR}/extmod/extmod.cmake)
endif()

set(MICROPY_QSTRDEFS_PORT
    ${PROJECT_DIR}/src/adapter/qstrdefsport.h
)

set(MICROPY_SOURCE_SHARED
    ${MICROPY_DIR}/shared/readline/readline.c
    ${MICROPY_DIR}/shared/netutils/netutils.c
    ${MICROPY_DIR}/shared/timeutils/timeutils.c
    ${MICROPY_DIR}/shared/runtime/interrupt_char.c
    ${MICROPY_DIR}/shared/runtime/stdout_helpers.c
    ${MICROPY_DIR}/shared/runtime/sys_stdio_mphal.c
    ${MICROPY_DIR}/shared/runtime/pyexec.c
)

set(MICROPY_SOURCE_LIB
    ${MICROPY_DIR}/lib/littlefs/lfs1.c
    ${MICROPY_DIR}/lib/littlefs/lfs1_util.c
    ${MICROPY_DIR}/lib/littlefs/lfs2.c
    ${MICROPY_DIR}/lib/littlefs/lfs2_util.c
    ${MICROPY_DIR}/lib/mbedtls_errors/mp_mbedtls_errors.c
    ${MICROPY_DIR}/lib/oofatfs/ff.c
    ${MICROPY_DIR}/lib/oofatfs/ffunicode.c
)
if(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND MICROPY_SOURCE_LIB ${MICROPY_DIR}/shared/runtime/gchelper_generic.c)
endif()

set(MICROPY_SOURCE_DRIVERS
    ${MICROPY_DIR}/drivers/bus/softspi.c
    ${MICROPY_DIR}/drivers/dht/dht.c
)

set(MICROPY_SOURCE_PORT
    ${PROJECT_DIR}/src/main.c
    ${PROJECT_DIR}/src/adapter/uart.c
    ${PROJECT_DIR}/src/adapter/usb.c
    ${PROJECT_DIR}/src/adapter/usb_msc.c
    ${PROJECT_DIR}/src/adapter/usb_serial_jtag.c
    ${PROJECT_DIR}/src/adapter/gccollect.c
    ${PROJECT_DIR}/src/adapter/mphalport.c
    ${PROJECT_DIR}/src/adapter/fatfs_port.c
    ${PROJECT_DIR}/src/adapter/help.c
    ${PROJECT_DIR}/src/adapter/modutime.c
    ${PROJECT_DIR}/src/adapter/moduos.c
    ${PROJECT_DIR}/src/adapter/machine_bitstream.c
    ${PROJECT_DIR}/src/adapter/machine_timer.c
    ${PROJECT_DIR}/src/adapter/machine_pin.c
    ${PROJECT_DIR}/src/adapter/machine_touchpad.c
    ${PROJECT_DIR}/src/adapter/machine_adc.c
    ${PROJECT_DIR}/src/adapter/machine_adcblock.c
    ${PROJECT_DIR}/src/adapter/machine_dac.c
    ${PROJECT_DIR}/src/adapter/machine_i2c.c
    ${PROJECT_DIR}/src/adapter/machine_i2s.c
    ${PROJECT_DIR}/src/adapter/machine_uart.c
    ${PROJECT_DIR}/src/adapter/modmachine.c
    ${PROJECT_DIR}/src/adapter/modnetwork.c
    ${PROJECT_DIR}/src/adapter/network_lan.c
    ${PROJECT_DIR}/src/adapter/network_ppp.c
    ${PROJECT_DIR}/src/adapter/network_wlan.c
    ${PROJECT_DIR}/src/adapter/mpnimbleport.c
    ${PROJECT_DIR}/src/adapter/modsocket.c
    ${PROJECT_DIR}/src/adapter/modesp.c
    ${PROJECT_DIR}/src/adapter/esp32_nvs.c
    ${PROJECT_DIR}/src/adapter/esp32_partition.c
    ${PROJECT_DIR}/src/adapter/esp32_rmt.c
    ${PROJECT_DIR}/src/adapter/esp32_ulp.c
    ${PROJECT_DIR}/src/adapter/modesp32.c
    ${PROJECT_DIR}/src/adapter/machine_hw_spi.c
    ${PROJECT_DIR}/src/adapter/machine_wdt.c
    ${PROJECT_DIR}/src/adapter/mpthreadport.c
    ${PROJECT_DIR}/src/adapter/machine_rtc.c
    ${PROJECT_DIR}/src/adapter/machine_sdcard.c
    ${PROJECT_DIR}/src/driver/drv_i2c.c
    ${PROJECT_DIR}/src/driver/drv_i2s.c
    ${PROJECT_DIR}/src/driver/drv_nvs.c
    ${PROJECT_DIR}/src/driver/vfs_stream.c
    ${PROJECT_DIR}/src/driver/mt_esp32_button.c
    ${PROJECT_DIR}/src/driver/drv_step_motor.c
    ${PROJECT_DIR}/src/driver/mt_event_mechanism.c
    ${PROJECT_DIR}/src/driver/drv_light_sensor.c
    ${PROJECT_DIR}/src/driver/drv_infrared_tube.c
    ${PROJECT_DIR}/src/driver/drv_aw20144.c
    ${PROJECT_DIR}/src/driver/driver_update.c
    ${PROJECT_DIR}/src/driver/drv_ble.c
    ${PROJECT_DIR}/src/driver/ble_prph/bleprph.c
    ${PROJECT_DIR}/src/driver/ble_prph/gatt_svr.c
    ${PROJECT_DIR}/src/driver/ble_prph/misc.c
    #${PROJECT_DIR}/src/driver/ble_prph/scli.c
    ${PROJECT_DIR}/src/driver/system/battery_check.c
    ${PROJECT_DIR}/src/driver/system/firmware_version.c
    ${PROJECT_DIR}/src/driver/system/indicator_led.c
    ${PROJECT_DIR}/src/driver/system/power_management.c
    ${PROJECT_DIR}/src/driver/system/system_management.c
    ${PROJECT_DIR}/src/driver/system/usb_detect.c
    ${PROJECT_DIR}/src/audio/audio_player.c
    ${PROJECT_DIR}/src/audio/audio_recorder.c
    ${PROJECT_DIR}/src/audio/speech_cn.c
    ${PROJECT_DIR}/src/audio/audio_tone_uri.c
    ${PROJECT_DIR}/src/mpy_modules/modmatatalab.c
    ${PROJECT_DIR}/src/mpy_modules/mt_mpy_button.c
    ${PROJECT_DIR}/src/mpy_modules/mt_mpy_event.c
    ${PROJECT_DIR}/src/mpy_modules/mt_mpy_ble.c
    ${PROJECT_DIR}/src/mpy_modules/mpy_leds.c
    ${PROJECT_DIR}/src/mpy_modules/motion/mpy_motion.c
    ${PROJECT_DIR}/src/mpy_modules/sensor/mpy_sensor.c
    ${PROJECT_DIR}/src/mpy_modules/modaudio.c
)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_USERMOD}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_PORT}
    ${MICROPY_SOURCE_BOARD}
)

set(IDF_COMPONENTS
    app_update
    bootloader_support
    bt
    driver
    esp_adc_cal
    esp_common
    esp_eth
    esp_event
    esp_ringbuf
    esp_rom
    esp_wifi
    freertos
    heap
    log
    lwip
    mbedtls
    mdns
    newlib
    nvs_flash
    sdmmc
    soc
    spi_flash
    tcpip_adapter
    ulp
    vfs
    xtensa
    json
)

if(IDF_VERSION_MINOR GREATER_EQUAL 1 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_netif)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 2 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_system)
    list(APPEND IDF_COMPONENTS esp_timer)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 3 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_hw_support)
    list(APPEND IDF_COMPONENTS esp_pm)
    list(APPEND IDF_COMPONENTS hal)
endif()

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND IDF_COMPONENTS esp32)
elseif(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND IDF_COMPONENTS esp32c3)
    list(APPEND IDF_COMPONENTS riscv)
elseif(IDF_TARGET STREQUAL "esp32s2")
    list(APPEND IDF_COMPONENTS esp32s2)
    list(APPEND IDF_COMPONENTS tinyusb)
elseif(IDF_TARGET STREQUAL "esp32s3")
    list(APPEND IDF_COMPONENTS esp32s3)
    list(APPEND IDF_COMPONENTS tinyusb)
    list(APPEND IDF_COMPONENTS audio_board)
    list(APPEND IDF_COMPONENTS audio_hal)
    list(APPEND IDF_COMPONENTS audio_pipeline)
    list(APPEND IDF_COMPONENTS audio_recorder)
    list(APPEND IDF_COMPONENTS audio_sal)
    list(APPEND IDF_COMPONENTS audio_stream)
    list(APPEND IDF_COMPONENTS display_service)
    list(APPEND IDF_COMPONENTS esp_actions)
    list(APPEND IDF_COMPONENTS esp_dispatcher)
    list(APPEND IDF_COMPONENTS esp_peripherals)
    list(APPEND IDF_COMPONENTS esp-adf-libs)
    list(APPEND IDF_COMPONENTS esp-sr)
    list(APPEND IDF_COMPONENTS tone_partition)
endif()

# Register the main IDF component.
idf_component_register(
    SRCS
        ${MICROPY_SOURCE_PY}
        ${MICROPY_SOURCE_EXTMOD}
        ${MICROPY_SOURCE_SHARED}
        ${MICROPY_SOURCE_LIB}
        ${MICROPY_SOURCE_DRIVERS}
        ${MICROPY_SOURCE_PORT}
        ${MICROPY_SOURCE_BOARD}
    INCLUDE_DIRS
        ${MICROPY_INC_CORE}
        ${MICROPY_INC_USERMOD}
        ${MICROPY_PORT_DIR}
        ${MICROPY_PORT_DIR}/src/adapter/
        ${MICROPY_PORT_DIR}/src/audio/
        ${MICROPY_PORT_DIR}/src/driver/
        ${MICROPY_PORT_DIR}/src/driver/ble_prph/
        ${MICROPY_PORT_DIR}/src/driver/system/
        ${MICROPY_PORT_DIR}/src/mpy_modules/motion/
        ${MICROPY_PORT_DIR}/src/mpy_modules/sensor/
        ${MICROPY_BOARD_DIR}
        ${CMAKE_BINARY_DIR}
        ${MICROPY_DIR}/py
        ${PROJECT_DIR}
        ${PROJECT_DIR}/components
    REQUIRES
        ${IDF_COMPONENTS}
)

# Set the MicroPython target as the current (main) IDF component target.
set(MICROPY_TARGET ${COMPONENT_TARGET})

# Define mpy-cross flags, for use with frozen code.
set(MICROPY_CROSS_FLAGS -march=xtensawin)

# Set compile options for this port.
target_compile_definitions(${MICROPY_TARGET} PUBLIC
    ${MICROPY_DEF_CORE}
    MICROPY_ESP_IDF_4=1
    MICROPY_VFS_FAT=1
    #MICROPY_VFS_LFS2=1
    FFCONF_H=\"${MICROPY_OOFATFS_DIR}/ffconf.h\"
    LFS1_NO_MALLOC LFS1_NO_DEBUG LFS1_NO_WARN LFS1_NO_ERROR LFS1_NO_ASSERT
    LFS2_NO_MALLOC LFS2_NO_DEBUG LFS2_NO_WARN LFS2_NO_ERROR LFS2_NO_ASSERT
)

# Disable some warnings to keep the build output clean.
target_compile_options(${MICROPY_TARGET} PUBLIC
    -Wno-clobbered
    -Wno-deprecated-declarations
    -Wno-missing-field-initializers
)

# Add additional extmod and usermod components.
target_link_libraries(${MICROPY_TARGET} micropy_extmod_btree)
target_link_libraries(${MICROPY_TARGET} usermod)


# Collect all of the include directories and compile definitions for the IDF components.
foreach(comp ${IDF_COMPONENTS})
    micropy_gather_target_properties(__idf_${comp})
endforeach()

if(IDF_VERSION_MINOR GREATER_EQUAL 2 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    # These paths cannot currently be found by the IDF_COMPONENTS search loop above,
    # so add them explicitly.
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/soc/${IDF_TARGET}/include)
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/soc/include)
    if(IDF_VERSION_MINOR GREATER_EQUAL 3)
        list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/tinyusb/additions/include)
        list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/tinyusb/tinyusb/src)
    endif()
endif()

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)

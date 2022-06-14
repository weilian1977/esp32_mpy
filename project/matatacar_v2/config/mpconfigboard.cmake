set(IDF_TARGET esp32s3)

set(SDKCONFIG_DEFAULTS
    config/sdkconfig.base
    config/sdkconfig.usb
    config/sdkconfig.spiram_sx
    config/sdkconfig.board
    config/sdkconfig.base_s3
    config/sdkconfig.freertos
    config/sdkconfig.ble
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/config/manifest.py)
endif()

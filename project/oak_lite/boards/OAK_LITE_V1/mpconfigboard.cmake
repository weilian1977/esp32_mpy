set(IDF_TARGET esp32s3)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/OAK_LITE_V1/sdkconfig.usb
    boards/OAK_LITE_V1/sdkconfig.spiram_sx
    boards/OAK_LITE_V1/sdkconfig.board
    boards/OAK_LITE_V1/sdkconfig.base_s3
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py)
endif()

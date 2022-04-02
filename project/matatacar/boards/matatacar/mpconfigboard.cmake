set(IDF_TARGET esp32s3)

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/${CMAKE_PROJECT_NAME}/sdkconfig.usb
    boards/${CMAKE_PROJECT_NAME}/sdkconfig.spiram_sx
    boards/${CMAKE_PROJECT_NAME}/sdkconfig.board
    boards/${CMAKE_PROJECT_NAME}/sdkconfig.base_s3
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py)
endif()

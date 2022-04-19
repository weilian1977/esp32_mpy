set(IDF_TARGET esp32s3)

get_filename_component(CURRENT_FOLDER ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" CURRENT_FOLDER ${CURRENT_FOLDER})

set(SDKCONFIG_DEFAULTS
    boards/sdkconfig.base
    boards/${CURRENT_FOLDER}/sdkconfig.usb
    boards/${CURRENT_FOLDER}/sdkconfig.spiram_sx
    boards/${CURRENT_FOLDER}/sdkconfig.board
    boards/${CURRENT_FOLDER}/sdkconfig.base_s3
    boards/${CURRENT_FOLDER}/sdkconfig.ble
)

if(NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${MICROPY_PORT_DIR}/boards/manifest.py)
endif()

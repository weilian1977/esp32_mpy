# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(UPDATE_COMPONENT_TOP "../managed_components/tinyusb")
set(IDF_COMPONENT_TOP "../../../submodule/esp-idf/components")

set(SDKCONFIG_DEFAULTS
    sdkconfig.base_s3
    sdkconfig.240mhz_s3
    sdkconfig.spiram_s3
    sdkconfig.usb
)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(cdc_msc)
idf_component_register(SRCS "main.c" "msc_disk.c"
                    INCLUDE_DIRS "."
                    "${IDF_PATH}/components/tinyusb/tinyusb/src"
                    "${UPDATE_COMPONENT_TOP}/additions/include"
                    "${UPDATE_COMPONENT_TOP}/additions/include_private"
                    "${IDF_COMPONENT_TOP}/freertos/include/freertos"
                    REQUIRES soc freertos fatfs)
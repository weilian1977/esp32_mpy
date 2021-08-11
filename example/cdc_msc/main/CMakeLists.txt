idf_component_register(SRCS "main.c" "msc_disk.c"
                    INCLUDE_DIRS "."
                    REQUIRES soc tinyusb freertos fatfs)
# Makefile for MicroPython on ESP32.
#
# This is a simple, convenience wrapper around idf.py (which uses cmake).

# Select the board to build for, defaulting to GENERIC.
#BOARD ?= GENERIC_S3_SPIRAM
BOARD ?= $(notdir $(CURDIR))

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

# Device serial settings.
PORT ?= /dev/ttyS4
BAUD ?= 921600

PYTHON ?= python3

GIT_SUBMODULES = lib/berkeley-db-1.xx

.PHONY: all clean deploy erase submodules FORCE

CMAKE_ARGS =

ifdef USER_C_MODULES
	CMAKE_ARGS += -DUSER_C_MODULES=${USER_C_MODULES}
endif

IDFPY_FLAGS += -D MICROPY_BOARD=$(BOARD) -B $(BUILD) $(CMAKE_ARGS)

ifdef FROZEN_MANIFEST
       IDFPY_FLAGS += -D MICROPY_FROZEN_MANIFEST=$(FROZEN_MANIFEST)
endif

all:
	rm -rf $(BUILD)/frozen_content.c && idf.py $(IDFPY_FLAGS) build
	@$(PYTHON) makeimg.py \
		$(BUILD)/sdkconfig \
		$(BUILD)/bootloader/bootloader.bin \
		$(BUILD)/partition_table/partition-table.bin \
		$(BUILD)/${BOARD}.bin \
		$(BUILD)/firmware.bin \
		$(BUILD)/${BOARD}.uf2

$(BUILD)/bootloader/bootloader.bin $(BUILD)/partition_table/partition-table.bin $(BUILD)/micropython.bin: FORCE

clean:
	idf.py $(IDFPY_FLAGS) fullclean

deploy:
	idf.py $(IDFPY_FLAGS) -p $(PORT) -b $(BAUD) flash

erase:
	idf.py $(IDFPY_FLAGS) -p $(PORT) -b $(BAUD) erase_flash

make_fatfs:
	./tools/mkfatfs/mkfatfs_linux_x86 -c ./tools/vfs/ -s 0x450000 -t fatfs ./tools/vfs.bin

upload_fatfs:
	./tools/esptool/esptool --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x250000 ./tools/vfs.bin

make_fatfs1:
	./tools/mkfatfs/mkfatfs_linux_x86 -c ./tools/vfs1/ -s 0x300000 -t fatfs ./tools/vfs1.bin
make_fatfs2:
	./tools/mkfatfs/mkfatfs_linux_x86 -c ./tools/vfs2/ -s 0x1A0000 -t fatfs ./tools/vfs2.bin
upload_fatfs1:
	./tools/esptool/esptool --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x250000 ./tools/vfs1.bin
upload_fatfs2:
	./tools/esptool/esptool --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x550000 ./tools/vfs2.bin
	
submodules:
	git submodule update --init $(addprefix ../../,$(GIT_SUBMODULES))

# Makefile for MicroPython on ESP32.
#
# This is a simple, convenience wrapper around idf.py (which uses cmake).

# Select the board to build for, defaulting to GENERIC.
#BOARD ?= GENERIC_S3_SPIRAM
BOARD ?= $(notdir $(CURDIR))

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

TOOLS ?= tools

# Device serial settings.
PORT ?= /dev/ttyS14
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

$(BUILD)/bootloader/bootloader.bin $(BUILD)/partition_table/partition-table.bin $(BUILD)/${BOARD}.bin: FORCE

clean:
	idf.py $(IDFPY_FLAGS) fullclean

deploy:
	idf.py $(IDFPY_FLAGS) -p $(PORT) -b $(BAUD) flash

erase:
	idf.py $(IDFPY_FLAGS) -p $(PORT) -b $(BAUD) erase_flash

make_fatfs:
	./tools/mkfatfs/mkfatfs_linux_x86 -c ./tools/fatdisk/vfsroot/ -s 0x600000 -t fatfs ./tools/fatdisk/vfs.bin

make_fatfs_music:
	./tools/mkfatfs/mkfatfs_linux_x86 -c ./tools/fatdisk/vfsroot_music/ -s 0x600000 -t fatfs ./tools/fatdisk/vfs_music.bin

upload_fatfs:
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x880000 ./tools/fatdisk/vfs.bin

upload_fatfs_music:
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x280000 ./tools/fatdisk/vfs_music.bin

#mpy_ota
upload_factory_app:
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0xE80000 ./tools/ota_bin/ota.bin
	#./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0xE80000 ../../../mpy_ota/build/talebotpro-ota.bin

#ota0
upload_ota:
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x10000 ./build-matatacar_v3/matatacar_v3.bin

upload_bootloader:
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x0 ./build-matatacar_v3/bootloader/bootloader.bin 0x8000 ./build-matatacar_v3/partition_table/partition-table.bin

upload_ota_data:
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0xd000 ./tools/ota_bin/ota_data.bin

upload_all:
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x0 ./build-matatacar_v3/bootloader/bootloader.bin 0x8000 ./build-matatacar_v3/partition_table/partition-table.bin
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0xd000 ./tools/ota_bin/ota_data.bin
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x10000 ./build-matatacar_v3/matatacar_v3.bin
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x280000 ./tools/fatdisk/vfs_music.bin
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x880000 ./tools/fatdisk/vfs.bin
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0xE80000 ./tools/ota_bin/ota.bin

total_bin:
	@$(PYTHON) $(TOOLS)/total_bin/makeimg_flash.py $(BUILD)/bootloader/bootloader.bin $(BUILD)/partition_table/partition-table.bin $(BUILD)/../tools/ota_bin/ota_data.bin $(BUILD)/matatacar_v3.bin  $(BUILD)/../tools/fatdisk/vfs_music.bin $(BUILD)/../tools/fatdisk/vfs.bin $(BUILD)/../tools/ota_bin/ota.bin $(BUILD)/flash.bin

upload_all2:
	./tools/esptool_py/esptool/esptool.py --before default_reset --after hard_reset -p $(PORT) -b $(BAUD) write_flash 0x0 $(BUILD)/flash.bin

submodules:
	git submodule update --init $(addprefix ../../,$(GIT_SUBMODULES))

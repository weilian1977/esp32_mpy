import sys

OFFSET_BOOTLOADER = 0x0
OFFSET_PARTITIONS = 0x8000
OFFSET_OTA_DATA = 0xd000
OFFSET_APPLICATION = 0x10000
OFFSET_VFS_MUSIC = 0x280000
OFFSET_VFS = 0x880000
OFFSET_OTA = 0xe80000
FLASH_SIZE = 0x1000000
#FLASH_SIZE = 0xFFFFFF

files_in = [
    ("bootloader", OFFSET_BOOTLOADER, sys.argv[1]),
    ("partitions", OFFSET_PARTITIONS, sys.argv[2]),
    ("ota_data", OFFSET_OTA_DATA, sys.argv[3]),
    ("application", OFFSET_APPLICATION, sys.argv[4]),
    ("vfs_musci", OFFSET_VFS_MUSIC, sys.argv[5]),
    ("vfs", OFFSET_VFS, sys.argv[6]),
    ("ota", OFFSET_OTA, sys.argv[7]),
]
file_out = sys.argv[8]

cur_offset = 0
#cur_offset = OFFSET_BOOTLOADER
with open(file_out, "wb") as fout:
    for name, offset, file_in in files_in:
        assert offset >= cur_offset
        fout.write(b"\xff" * (offset - cur_offset))
        cur_offset = offset
        with open(file_in, "rb") as fin:
            data = fin.read()
            fout.write(data)
            cur_offset += len(data)
            print("%-12s% 8d" % (name, len(data)))
    print("%-12s% 8d" % ("valid", cur_offset))
    print("%-12s% 8f" % ("size(M)", cur_offset/1024.0/1024))
    # assert FLASH_SIZE >= cur_offset
    # fout.write(b"\xff" * (FLASH_SIZE - cur_offset))
    fout.close()
    print("%-12s% 8d" % ("total", FLASH_SIZE))
    print("%-12s% 8f" % ("size(M)", FLASH_SIZE/1024.0/1024))



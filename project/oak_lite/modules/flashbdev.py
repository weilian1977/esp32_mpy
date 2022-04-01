from esp32 import Partition

# MicroPython's partition table uses "vfs", TinyUF2 uses "ffat".
bdev = Partition.find(Partition.TYPE_DATA, label="vfs1")
if not bdev:
    bdev = Partition.find(Partition.TYPE_DATA, label="ffat", block_size=4096)
bdev1 = bdev[0] if bdev else None
bdev = Partition.find(Partition.TYPE_DATA, label="vfs2")
bdev2 = bdev[0] if bdev else None
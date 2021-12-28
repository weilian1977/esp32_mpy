import gc
import os, machine
from flashbdev import bdev

try:
    if bdev:
        os.mount(machine.SDCard(width=1), '/')
        #os.mount(bdev, "/")
except OSError:
    import inisetup

    vfs = inisetup.setup()

gc.collect()

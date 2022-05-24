import gc
import uos, machine
from flashbdev import bdev1, bdev2

from oak_lite import *
import time
import audio

try:
    if bdev1:
        uos.mount(bdev1, "/")
        uos.mount(bdev2, "/vfs2")
        #uos.mount(bdev, "/vfs")
        #uos.mount(machine.SDCard(width=1), '/sdcard') 
except OSError:
    import inisetup

    vfs = inisetup.setup()

gc.collect()

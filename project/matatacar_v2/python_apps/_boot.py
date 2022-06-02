import gc
import uos, machine
from flashbdev import bdev

try:
    if bdev:
        uos.mount(bdev, "/")
        #uos.mount(bdev, "/vfs")
        #uos.mount(machine.SDCard(width=1), '/sdcard') 
except OSError:
    import inisetup

    vfs = inisetup.setup()

gc.collect()

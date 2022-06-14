import time
import matatalab
import _thread
from communication import communication_process
import audio
import binascii
import drv_system
import drv_led
import system_state
from _audio import player

if __name__ == '__main__':
    matatalab.indicator_led(matatalab.ON)
    drv_led.led_eye_rgb_cfg(2, 0, 0, 80)
    print("power on script begin")
    time.sleep(0.8)
    player().play('file://sdcard/music/system/2-on.mp3', 0, True , 0)
    drv_led.led_eye_rgb_cfg(2, 0, 0, 0)
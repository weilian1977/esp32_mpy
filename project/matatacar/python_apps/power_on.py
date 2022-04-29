import time
import matatalab
import _thread
from communication import communication_process
import audio_play
import binascii
import drv_system
import drv_led
import system_state

if __name__ == '__main__':
    matatalab.indicator_led(matatalab.ON)
    drv_led.led_eye_rgb_cfg(2, 0, 0, 200)
    print("power on script begin")
    time.sleep(0.8)
    audio_play.play('/sdcard/music/system/2-on.mp3', 70, True)
    drv_led.led_eye_rgb_cfg(2, 0, 0, 0)
from oak_lite import *
import time
while True:
    led.show_rgb(5, 0, 0)
    time.sleep(0.5)
    led.show_rgb(0, 5, 0)
    time.sleep(0.5)
    led.show_rgb(0, 0, 5)
    time.sleep(0.5)
    led.show_rgb(0, 0, 0)
    time.sleep(1)
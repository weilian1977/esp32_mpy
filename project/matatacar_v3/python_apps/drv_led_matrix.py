#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matatalab
import math
import time

_led_matrix = matatalab.led_matrix()

def show_image(data_array, time = "None"):
    _led_matrix.show_image(data_array)
    if time != "None":
        time.sleep(time)

def set_pixel(x, y, brightness):
    brightness_data = int((round(brightness * 255.0 / 100.0)));
    _led_matrix.set_pixel(x, y, brightness_data)

def write(string):
    print("write:%s" %(string))

def clear():
    _led_matrix.clear()

def set_brightness(brightness):
    brightness_data = int((round(brightness * 255.0 / 100.0)));
    _led_matrix.set_brightness(brightness_data)

def screen_rotate(rotate):
    print("screen_rotate:%s" %(rotate))

def set_orientation(orientation):
    print("set_orientation:%s" %(orientation))
    pass
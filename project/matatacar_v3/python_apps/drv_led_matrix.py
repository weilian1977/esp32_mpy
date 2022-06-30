#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matatalab
import math
import time
import led_matrix_data

_led_matrix = matatalab.led_matrix()

PICTURE_MODE = const(0x00)
ANIMATION_MODE = const(0x01)
CHARACTER_MODE = const(0x02)

class display():
    _width = 15
    _height = 7
    _brightness = 20
    _mode = PICTURE_MODE
    _currently_display_data = bytearray([0] * 16)
    _display_bank = [[0 for i in range(16)] for j in range(8)]
    _frame = 0

    def __init__(self):
        self._frame = None
        self.init()

    def init(self):
        self.sleep(True)
        self._mode = PICTURE_MODE
        for frame in range(8):
            empty_picture = bytearray([0] * 16)
            self.picture_bank(frame, empty_picture)
        self.sleep(False)
        data_array = bytearray(led_matrix_data.face_data_table.get('face1'))
        self.show_image(data_array, "None")

    def picture_bank(self, bank, data_array):
        if(bank < 0) or (bank > 7):
            return
        for i in range(len(data_array)):
            self._display_bank[bank][i] = data_array[i]

    def sleep(self, value):
        if value is True:
            self.set_brightness(0)
        else:
            self.set_brightness(self.brightness)

    def reset(self, time_value = 0.01):
        self.sleep(True)
        time.sleep(time_value)  # 10 MS pause to reset.
        self.sleep(False)

    def pixel_addr(self, x, y):
        if x > 7:
            x = 15 - x
            y += 8
        else:
            y = 7 - y
        return x * 16 + y

    def show_image(self, data_array, time_data = "None"):
        self._mode = PICTURE_MODE
        _led_matrix.show_image(data_array)
        if time_data != "None":
            time.sleep(time_data)
            _led_matrix.clear()

    def set_pixel(self, x, y, brightness):
        brightness_data = int((round(brightness * 255.0 / 100.0)));
        _led_matrix.set_pixel(x, y, brightness_data)

    def write(self, string):
        self._mode = CHARACTER_MODE
        print("write:%s" %(string))

    def clear():
        self._mode = PICTURE_MODE
        _led_matrix.clear()

    def set_brightness(self, brightness):
        self._brightness = brightness
        brightness_data = int((round(brightness * 255.0 / 100.0)));
        _led_matrix.set_brightness(brightness_data)

    def get_brightness(self):
        return self._brightness

    def screen_rotate(self, rotate):
        print("screen_rotate:%s" %(rotate))

    def set_orientation(self, orientation):
        print("set_orientation:%s" %(orientation))
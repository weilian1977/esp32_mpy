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

TURN_PAGES_MODE = const(0X00)
MOVE_UP_MODE = const(0X01)
MOVE_DOWN_MODE = const(0X02)
MOVE_LEFT_MODE = const(0X03)
MOVE_RIGHT_MODE = const(0X04)

class display():
    _width = 15
    _height = 7
    _brightness = 20
    _mode = PICTURE_MODE
    _currently_display_data = bytearray([0] * 16)
    _display_bank = [[0 for i in range(16)] for j in range(8)]
    _frame = 0
    _char_string = ""
    _refresh_mode = MOVE_LEFT_MODE
    _refresh_time = 0.1

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
            _led_matrix.set_brightness(0)
        else:
            brightness_data = int(self._brightness * 2)
            _led_matrix.set_brightness(brightness_data)

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
        self._mode = CHARACTER_MODE
        brightness_data = int(brightness * 2);
        _led_matrix.set_pixel(x, y, brightness_data)

    def write(self, input):
        strlist = str(input)
        print("write:%s" %(strlist))
        if len(strlist) > 2:
            self._char_string = "%s%s" %(strlist, "  ")
            self._mode = CHARACTER_MODE
        else:
            self._mode = PICTURE_MODE
            char_byte_list = [[0 for i in range(16)] for j in range(2)]
            for i in range(2):
                if(len(strlist) > i):
                    char_byte_list[i] = led_matrix_data.character_data_table.get(strlist[i])
                    if(char_byte_list[i] is None):
                        char_byte_list[i] = led_matrix_data.character_data_table.get(" ")
                else:
                    char_byte_list[i] = led_matrix_data.character_data_table.get(" ")

            for frame_byte in range(8):
                self._currently_display_data[frame_byte * 2] =  char_byte_list[0][frame_byte]
                self._currently_display_data[frame_byte * 2 + 1] =  char_byte_list[1][frame_byte]
                _led_matrix.show_image(bytearray(self._currently_display_data))

    def clear(self):
        self._mode = PICTURE_MODE
        self._char_string = ""
        _led_matrix.clear()

    def set_brightness(self, brightness):
        self._brightness = brightness
        brightness_data = int(self._brightness  * 2);
        _led_matrix.set_brightness(brightness_data)

    def get_brightness(self):
        return self._brightness

    def screen_rotate(self, rotate):
        print("screen_rotate:%s" %(rotate))

    def set_orientation(self, orientation):
        print("set_orientation:%s" %(orientation))
        if orientation == "upright":
            self._refresh_mode = MOVE_UP_MODE
        elif orientation == "left":
            self._refresh_mode = MOVE_LEFT_MODE
        elif orientation == "right":
            self._refresh_mode = MOVE_RIGHT_MODE
        elif orientation == "upside down":
            self._refresh_mode = MOVE_DOWN_MODE
        else:
            self._refresh_mode = MOVE_LEFT_MODE

    def set_refresh_time(self, time):
        self._refresh_time = time

    def set_refresh_mode(self, mode):
        self._refresh_mode = mode

_display = display()

def led_matrix_process():
    while True:
        if(_display._mode == CHARACTER_MODE):
            if (_display._refresh_mode == TURN_PAGES_MODE):
                for char_index in range(len(_display._char_string)):
                    char_data = _display._char_string[char_index % len(_display._char_string)]
                    char_bytes = led_matrix_data.character_data_table.get(char_data)
                    if (_display._refresh_mode != TURN_PAGES_MODE):
                        break
                    if(char_bytes is None):
                        char_bytes = led_matrix_data.character_data_table.get(" ")
                    if(char_index % 2 == 0):
                        for frame_byte in range(8):
                            _display._currently_display_data[frame_byte * 2] =  char_bytes[frame_byte]
                        if(char_index == len(_display._char_string) - 1):
                            char_bytes = led_matrix_data.character_data_table.get(" ")
                            for frame_byte in range(8):
                                _display._currently_display_data[frame_byte * 2 + 1] =  char_bytes[frame_byte]
                            _led_matrix.show_image(bytearray(_display._currently_display_data))
                            time.sleep(_display._refresh_time * 8)
                    else:
                        for frame_byte in range(8):
                            _display._currently_display_data[frame_byte * 2 + 1] =  char_bytes[frame_byte]
                        _led_matrix.show_image(bytearray(_display._currently_display_data))
                        time.sleep(_display._refresh_time * 8)
            elif (_display._refresh_mode == MOVE_LEFT_MODE) or (_display._refresh_mode == MOVE_RIGHT_MODE):
                char_data_buffer = [0] * 3
                char_byte_list = [[0 for i in range(16)] for j in range(3)]
                for char_index in range(len(_display._char_string) - 2 ):
                    for i in range(3):
                        char_data_buffer[i] = _display._char_string[(char_index + i) % len(_display._char_string)]
                        char_byte_list[i] = led_matrix_data.character_data_table.get(char_data_buffer[i])
                        if(char_byte_list[i] is None):
                            char_byte_list[i] = led_matrix_data.character_data_table.get(" ")
                    if(_display._refresh_mode == MOVE_LEFT_MODE):
                        for j in range(8):
                            for frame_byte in range(8):
                                _display._currently_display_data[frame_byte * 2] =  (char_byte_list[0][frame_byte] >> j) | (char_byte_list[1][frame_byte] << (8 - j))
                                _display._currently_display_data[frame_byte * 2 + 1] =  (char_byte_list[1][frame_byte] >> j) | (char_byte_list[2][frame_byte] << (8 -j))
                            _led_matrix.show_image(bytearray(_display._currently_display_data))
                            time.sleep(_display._refresh_time)
                    elif (_display._refresh_mode == MOVE_RIGHT_MODE):
                        for j in range(8):
                            for frame_byte in range(8):
                                _display._currently_display_data[frame_byte * 2 + 1] =  (char_byte_list[0][frame_byte] << j) | (char_byte_list[1][frame_byte] >> (8 - j))
                                _display._currently_display_data[frame_byte * 2] =  (char_byte_list[1][frame_byte] << j) | (char_byte_list[2][frame_byte] >> (8 -j))
                            _led_matrix.show_image(bytearray(_display._currently_display_data))
                            time.sleep(_display._refresh_time)
                    else:
                        break
                time.sleep(_display._refresh_time * 2)
            elif (_display._refresh_mode == MOVE_UP_MODE) or (_display._refresh_mode == MOVE_DOWN_MODE):
                if len(_display._char_string) % 2 != 0:
                    _display._char_string = "%s%s" %(_display._char_string, " ")
                char_data_buffer = [0] * 4
                char_byte_list = [[0 for i in range(16)] for j in range(4)]
                for char_index in range(len(_display._char_string) / 2):
                    for i in range(4):
                        char_data_buffer[i] = _display._char_string[(char_index * 2 + i) % len(_display._char_string)]
                        char_byte_list[i] = led_matrix_data.character_data_table.get(char_data_buffer[i])
                        if(char_byte_list[i] is None):
                            char_byte_list[i] = led_matrix_data.character_data_table.get(" ")
                    if char_index * 2 + 3 < len(_display._char_string):
                        print("data[%d]: %c,%c,%c,%c" %(char_index, _display._char_string[char_index * 2],_display._char_string[char_index * 2 + 1],_display._char_string[char_index * 2 + 2],_display._char_string[char_index * 2 + 3]))
                    if (_display._refresh_mode == MOVE_UP_MODE):
                        for j in range(8):
                            for frame_byte in range(8):
                                if((frame_byte + j) < 8):
                                    _display._currently_display_data[frame_byte * 2] = char_byte_list[0][frame_byte + j]
                                    _display._currently_display_data[frame_byte * 2 + 1] = char_byte_list[1][frame_byte + j]
                                else:
                                    _display._currently_display_data[frame_byte * 2] = char_byte_list[2][frame_byte + j - 8]
                                    _display._currently_display_data[frame_byte * 2 + 1] = char_byte_list[3][frame_byte + j -8]
                            _led_matrix.show_image(bytearray(_display._currently_display_data))
                            time.sleep(_display._refresh_time)

                    elif (_display._refresh_mode == MOVE_DOWN_MODE):
                        for j in range(8):
                            for frame_byte in range(8):
                                if(frame_byte < j):
                                    _display._currently_display_data[frame_byte * 2] = char_byte_list[2][frame_byte + 8 - j]
                                    _display._currently_display_data[frame_byte * 2 + 1] = char_byte_list[3][frame_byte + 8 - j]
                                else:
                                    _display._currently_display_data[frame_byte * 2] = char_byte_list[0][frame_byte - j]
                                    _display._currently_display_data[frame_byte * 2 + 1] = char_byte_list[1][frame_byte - j]
                            _led_matrix.show_image(bytearray(_display._currently_display_data))
                            time.sleep(_display._refresh_time)
                    else:
                        break
                time.sleep(_display._refresh_time * 2)
        else:
            time.sleep(0.2)
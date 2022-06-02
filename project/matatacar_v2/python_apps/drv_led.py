#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matatalab
import math
import machine, neopixel

_leds = neopixel.NeoPixel(machine.Pin(41), 6)

led_color = {
0:[0xff, 0xff, 0xff],
1:[0xff, 0x00, 0x00],
2:[0xff, 0xff, 0x00],
3:[0x00, 0xff, 0x00],
4:[0x00, 0x00, 0xff],
5:[0xff, 0x00, 0xff],
6:[0x00, 0x00, 0x00],
}

LEFT_EYE_MODE   = 0
RIGHT_EYE_MODE  = 1
BOTH_EYE_MODE   = 2

def show_all(r, g, b):
    v = (r, g, b)
    _leds.fill(v)
    _leds.write()

def show_side(mode, r, g, b):
    if mode == 0:
        _leds[0] = (r,g,b)
        _leds[1] = (r,g,b)
        _leds[2] = (r,g,b)
    elif mode == 1:
        _leds[3] = (r,g,b)
        _leds[4] = (r,g,b)
        _leds[5] = (r,g,b)
    _leds.write()

def display(data):
    data_len = len(data)
    for i in range(data_len / 3):
        _leds[i] = (data[3 * i], data[3 * i + 1], data[3 * i + 2])
    _leds.write()

def led_get_rgb(color, brightness):
    [r,g,b] = [int(round(i * brightness / 255.0)) for i in led_color[color]]
    return [r,g,b]

def led_eye_rgb_cfg(mode, r, g, b):
    if( mode == BOTH_EYE_MODE):
        show_all(r,g,b)
    else:
        show_side(mode, r, g, b)

def led_eye_set(mode, para):
    color = (para >> 4) & 0xF
    brightness = (para & 0xF)
    if brightness == 1:
        brightness = 1
    elif brightness == 2:
        brightness = 9
    elif brightness == 3:
        brightness = 32
    elif brightness == 4:
        brightness = 76
    elif brightness == 5:
        brightness = 148
    elif brightness == 6:
        brightness = 255
    else:
        brightness = 0
    [r,g,b] = led_get_rgb(color, brightness)
    led_eye_rgb_cfg(mode, r,g,b)

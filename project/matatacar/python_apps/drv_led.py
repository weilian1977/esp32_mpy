#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matatalab
import math


led = matatalab.leds()


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


def led_get_rgb(color, brightness):
    [r,g,b] = [int(round(i * brightness / 255.0)) for i in led_color[color]]
    return [r,g,b]

def led_eye_rgb_cfg(mode, r,g,b):
    if( mode == BOTH_EYE_MODE):
        led.show_all(r, g, b)
    else:
        led.show_single(mode, r, g, b)

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

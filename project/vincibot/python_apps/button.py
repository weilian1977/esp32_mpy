#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matatalab
import math

_button = matatalab.button()

def is_pressed(key_id):
    if key_id == 'triangle':
        return _button.is_pressed(3)
    elif key_id == 'square':
        return _button.is_pressed(2)
    elif key_id == 'circular':
        return _button.is_pressed(1)
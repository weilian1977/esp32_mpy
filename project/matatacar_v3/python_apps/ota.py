#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matatalab
import time
import nvs

ota_t = matatalab.ota()

def ota_start():            
    time.sleep(0.01)
    ota_t.ota_start()
    
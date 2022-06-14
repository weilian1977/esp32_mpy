#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time
import ble
import protocol_v1
import protocol_v2
import protocol_v3
import _thread
import matatalab
import drv_system
import audio

BLE_UNLINK = 0
BLE_LINK = 1

THREAD_BLE_STATE_UPDATE_SIZE = 8 * 1024
THREAD_COMMUNICATION_SIZE = 8 * 1024

msgbuffer = b''

carble = None
ble_state = BLE_UNLINK

def ble_state_monitor():
    global carble
    if(carble == None):
        return

    event = carble.get_event_state()
    if (event == ble._BLE_IRQ_CONNECT):
        carble.connect_state = True
    elif(event == ble._BLE_IRQ_DISCONNECT):
        carble.set_protocol(ble.PROTOCOL_VER_V1)
        carble.connect_state = False
    elif(event == ble._BLE_IRQ_READ):
        carble.recive_data()

def ble_process():
    global carble
    global ble_state
    if(ble_state == BLE_UNLINK):
        if(carble.get_connect_state() == BLE_LINK):
            print("ble connect")
            ble_state = BLE_LINK
            matatalab.indicator_led(matatalab.ON)
            audio.play('/sdcard/music/system/5-link.mp3' ,False)
    elif(ble_state == BLE_LINK):
        if(carble.get_connect_state() == BLE_UNLINK):
            print("ble disconnect")
            ble_state = BLE_UNLINK
            matatalab.indicator_led(matatalab.SINGLE_FLASH)
            audio.play('/sdcard/music/system/4-ulink.mp3' ,False)
        else:
            protocol_id = carble.get_protocol()
            if(protocol_id == ble.PROTOCOL_VER_V1):
                protocol_v1.protocol_v1_process(carble)
                
            if(protocol_id == ble.PROTOCOL_VER_V2):
                protocol_v2.protocol_v2_process(carble)
            
            if((carble.get_heart_time()) > 1000):
                carble.clear_heart_time()
                if(carble.get_heart_enable()):
                    carble.send_heart()

def communication():
    global carble, ble_state
    if(matatalab.is_first_start()):
        ble_state = carble.get_connect_state()
        if (ble_state == BLE_LINK):
            audio.play('/sdcard/music/system/5-link.mp3' ,False)
    else:
        ble_state = carble.get_connect_state()

    if(ble_state == BLE_UNLINK):
        matatalab.indicator_led(matatalab.SINGLE_FLASH)
    else:
        matatalab.indicator_led(matatalab.ON)
    time.sleep(1)
    while True:
        ble_process()
        time.sleep(0.02)


def communication_catch():
    communication()
    # try:
    #     communication()
    # except:
    #     print("communication_catch error")

def communication_process():
    global carble
    carble = ble.mpyBLE()
    drv_system.clear_idle_time()
    communication_catch()


#!/usr/bin/env python3
# -*- coding: utf-8 -*-

PLAY_IDLE = False
PLAY_RUN = True

play_state = PLAY_IDLE
stop_flag = False
power_off_state = False
code_stop = False
factory_state = False
ble_cmd_cancel_flag = False

def set_audio_stop_flag(state):
    global stop_flag
    stop_flag = state

def get_audio_stop_flag():
    global stop_flag
    return stop_flag

def set_play_state(state):
    global play_state
    play_state = state
    
def get_play_state():
    global play_state
    return play_state

def set_power_off_state(state):
    global power_off_state
    power_off_state = state
    
def get_power_off_state():
    global power_off_state
    return power_off_state
    
def set_factory_state(state):
    global factory_state
    factory_state = state
    
def get_factory_state():
    global factory_state
    return factory_state

def set_cmd_cancel_flag(flag):
    global ble_cmd_cancel_flag
    ble_cmd_cancel_flag = flag

def get_cmd_cancel_flag():
    global ble_cmd_cancel_flag
    return ble_cmd_cancel_flag
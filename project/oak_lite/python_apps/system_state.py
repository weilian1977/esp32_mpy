#!/usr/bin/env python3
# -*- coding: utf-8 -*-

SYSTEM_SPEED_NORMAL = 0
SYSTEM_SPEED_FAST = 1
SYSTEM_SPEED_SLOW = 2

## use
PLAY_IDLE = 0
PLAY_RUN = 1
PLAY_END = 2

SPEECH_IDLE =0
SPEECH_RUN = 1
SPEECH_END = 2

RECORD_IDLE = 0
RECORD_RUN = 1
RECORD_END = 2
RECORD_TIMEOUT = 3
RECORD_COPY = 4

play_state = PLAY_IDLE
speech_state = SPEECH_IDLE
record_state = RECORD_IDLE

##
code_state = False
stop_flag = False
file_count = 1
power_off_state = False
code_stop = False
factory_state = False
ble_cmd_cancel_flag = False
system_speed_mode = SYSTEM_SPEED_NORMAL
oid_map_state = False
low_power_state = False

## use
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

def set_record_state(state):
    global record_state
    record_state = state

def get_record_state():
    global record_state
    return record_state

def set_speech_state(state):
    global speech_state
    speech_state = state

def get_speech_state():
    global speech_state
    return speech_state
##

def set_code_stop_flag(state):
    global code_stop
    code_stop = state

def get_code_stop_flag():
    global code_stop
    return code_stop


def set_code_state(state):
    global code_state
    code_state = state

def get_code_state():
    global code_state
    return code_state

def set_file_count(count):
    global file_count
    file_count = count

def get_file_count():
    global file_count
    return file_count

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
    return ble_cmd_cancel_flag

def set_system_speed_mode(mode):
    global system_speed_mode
    system_speed_mode = mode

def get_system_speed_mode():
    global system_speed_mode
    return system_speed_mode

def set_oid_map_state(state):
    global oid_map_state
    oid_map_state = state

def get_oid_map_state():
    global oid_map_state
    return oid_map_state
    
def set_low_power_state(state):
    global low_power_state
    low_power_state = state

def get_low_power_state():
    global low_power_state
    return low_power_state
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from utility import if_file_exists

import time
import drv_led
import drv_motion
import matatalab
import system_state
import audio


MOTION_FORWARD    = 0
MOTION_TURN_LEFT  = 1
MOTION_TURN_RIGHT = 2
MOTION_BACKWARD   = 3

def play(file_name, sync):
    music_root = '/sdcard/music/'
    
    file_full_name = "%s%s" %(music_root, file_name)
    if(if_file_exists(file_full_name) == False):
        print("file not exist:",file_full_name)
        return
    audio.play(file_full_name, sync)


def play_move_time(index, delay):
    audio.play_move(index, 0)
    time.sleep_ms(delay)
        
def set_vol(value):
    audio.set_volume(value)
    
def get_vol():
    return audio.get_volume()

def action_motion(type, value):
    if(type == MOTION_FORWARD):
        drv_motion.move_position("forward", value, "mm", True)
    elif(type == MOTION_TURN_LEFT):
        drv_motion.move_angle("left", value, "degrees", True)
    elif(type == MOTION_TURN_RIGHT):
        drv_motion.move_angle("right", value, "degrees", True)
    elif(type == MOTION_BACKWARD):
        drv_motion.move_position("backward", value, "mm", True)
    
def action_eye(eye_mode, para):
    drv_led.led_eye_set(eye_mode, para)

def action_play_stop():
    audio.play_stop()

def action_cancel_cmd():
    action_play_stop()
    action_eye(2,0)
    drv_motion.stop(2)

def action_uint_unknow(p1,p2):
    pass

action_unit = {
0x01:[100, action_motion, MOTION_TURN_RIGHT, 90//6], 
0x02:[100, action_motion, MOTION_TURN_LEFT, 90//6],
0x03:[100, action_motion, MOTION_FORWARD, 100//3],
0x04:[100, action_motion, MOTION_BACKWARD, 100//3],
0x05:[100, action_motion, MOTION_TURN_LEFT, 360],
0x06:[100, action_motion, MOTION_TURN_RIGHT, 360],
0x07:[100, action_eye, drv_led.BOTH_EYE_MODE, 0x15],
0x08:[100, action_eye, drv_led.BOTH_EYE_MODE, 0x35],
0x09:[100, action_eye, drv_led.BOTH_EYE_MODE, 0x45],
0x0a:[100, action_eye, drv_led.BOTH_EYE_MODE, 0x60],
0x0b:[100, action_motion, MOTION_TURN_LEFT, 90//12],
0x0c:[100, action_motion, MOTION_TURN_RIGHT, 90//12],
0x0d:[0, play_move_time, 1, 100],
0x0e:[0, play_move_time, 2, 100],
0x0f:[0, play_move_time, 3, 100],
0x10:[0, play_move_time, 4, 100],
0x11:[0, play_move_time, 5, 100],
0x12:[0, play_move_time, 6, 100],
0x13:[1, action_motion, MOTION_FORWARD, 100//6],
0x14:[1, action_motion, MOTION_BACKWARD, 100//12],
0x15:[100, action_eye, drv_led.BOTH_EYE_MODE, 0x15],
0x16:[1, action_motion, MOTION_TURN_LEFT, 90//3],
0x17:[1, action_motion, MOTION_TURN_RIGHT, 90//3],
0x18:[1, action_motion, MOTION_TURN_LEFT, 90],
0x19:[1, action_motion, MOTION_TURN_RIGHT, 90],
0x1a:[1, action_motion, MOTION_FORWARD, 10],
0x1b:[1, action_motion, MOTION_BACKWARD, 10],
0x1c:[1, action_motion, MOTION_FORWARD, 100//30],
0x1d:[1, action_motion, MOTION_BACKWARD, 100//30],
0x1e:[100, action_eye, drv_led.BOTH_EYE_MODE, 0x55],
0x1f:[1, action_eye, drv_led.BOTH_EYE_MODE, 0x25],
0x20:[1, action_eye, drv_led.BOTH_EYE_MODE, 0x05],

}

dance_code = {
1:[3,4,1,2,5,6,7,8,9,0x0a],
2:[26,26,26,5,8,0x0a,27,27,27,6,9,0x0a],
3:[7,22,0x0a,23,8,22,0x0a,23,9,22,0x0a,23,30,5,0x0a],
4:[8,5,31,31,6,31,32,0x0a],
5:[1,1,2,2,19,21,20,20,0xa],
6:[28,28,28,27,24,28,28,28,27,25,28,28,28,27],
}

def action_dance_code_get(index):
    return dance_code.get(index,[])

movement_code = {
1:[7,13,0x0a],
2:[1,2,14,2,1],
3:[3,15,4],
4:[3,3,16,4,4],
5:[9,17,0x0a],
6:[6,18,5,0x0a],
}

def action_movement_code_get(index):
    return movement_code.get(index, [])

def action_parse(list):
    for byte in list:
        [delay, func, para1, para2] = action_unit.get(byte, [0, action_uint_unknow, 0, 0])
        cmd_cancel_flag = system_state.get_cmd_cancel_flag()
        if(cmd_cancel_flag == True):
            print("command cancle flag is true")
            return
        func(para1, para2)
        time.sleep_ms(delay + 100)
    time.sleep_ms(500)
    action_play_stop()

def action_dance(index):
    audio.play_dance(index, 0)
    dance_list = action_dance_code_get(index)
    print("action_dance", dance_list)
    action_parse(dance_list)

def action_movement(index):
    movement_list = action_movement_code_get(index)
    print("action_movement", movement_list)
    action_parse(movement_list)

def dance(dance_name):
    print("dance:%s" %(dance_name))
    audio.play_dance(1, 0)
    dance_list = action_dance_code_get(1)
    action_parse(dance_list)

def action_behavior(behavior_name):
    print("behavior:%s" %(behavior_name))
    movement_list = action_movement_code_get(1)
    action_parse(movement_list)

def emotion(emotion_name):
    print("emotion:%s" %(emotion_name))
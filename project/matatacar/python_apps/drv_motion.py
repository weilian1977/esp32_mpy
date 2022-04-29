#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import matatalab
import nvs

import drv_system

moto = matatalab.motion()

def motion_forward(step, wait_flag):
    step_distance_config = drv_system.system_info.get_step_distance()
    step_distance = int(step_distance_config)
    moto.forward((step * step_distance), wait_flag)

def motion_backward(step, wait_flag):
    step_distance_config = drv_system.system_info.get_step_distance()
    step_distance = int(step_distance_config)
    moto.backward((step * step_distance), wait_flag)

def motion_turn_left(angle, wait_flag):
    angle = int(round(angle * (nvs.save_left_angle) / 90.0))
    moto.turn_left(angle, wait_flag)

def motion_turn_right(angle, wait_flag):
    angle = int(round(angle * (nvs.save_right_angle) / 90.0))
    moto.turn_right(angle, wait_flag)

def motion_move_speed(speed_left, speed_right):
    moto.move_speed(speed_left, speed_right)

def motion_move_position(position, wait_flag):
    moto.move_position(position, wait_flag)

def motion_move_angle(angle, wait_flag):
    angle = int(round(angle * (nvs.save_angle) / 90.0))
    moto.move_angle(angle, wait_flag)
    
def motion_stop(mode):
    print("drv_motion stop")
    return moto.stop(mode)
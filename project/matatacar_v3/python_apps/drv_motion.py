#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import matatalab
import nvs
import time
import drv_system

moto = matatalab.motion()

def forward(step, wait_flag):
    step_distance_config = drv_system.system_info.get_step_distance()
    step_distance = int(step_distance_config)
    moto.forward((step * step_distance), wait_flag)

def backward(step, wait_flag):
    step_distance_config = drv_system.system_info.get_step_distance()
    step_distance = int(step_distance_config)
    moto.backward((step * step_distance), wait_flag)

def turn_left(angle, wait_flag):
    angle = int(round(angle * (nvs.save_left_angle) / 90.0))
    moto.turn_left(angle, wait_flag)

def turn_right(angle, wait_flag):
    angle = int(round(angle * (nvs.save_right_angle) / 90.0))
    moto.turn_right(angle, wait_flag)

def move_speed(speed_left, speed_right):
    moto.move_speed(speed_left, speed_right)

def move_position(dir, position, unit, wait_flag = False):
    moto.set_max_speed(700 * 32)
    position_value = position
    if dir == "backward":
        position_value = -position_value
    if unit == "seconds":
        moto.move_speed(108, 108)
        time.sleep(abs(position_value))
    elif unit == "mm":
        position_value = int(position_value)
        moto.move_position(position_value, wait_flag)
    elif unit == "cm":    
        position_value = int(round(position_value * 10.0))
        moto.move_position(position_value, wait_flag)
    elif unit == "inch":
        position_value = int(round(position_value * 25.4))
        moto.move_position(position_value, wait_flag)

def move_angle(dir, angle, unit, wait_flag = False):
    moto.set_max_speed(700 * 32)
    angle_value = angle
    if dir == "left":
        angle_value = -angle_value
    if unit == "seconds":
        moto.move_speed(-108, 108)
        time.sleep(abs(angle_value))
    elif unit == "rotations":
        angle_value = int(round(angle_value * 360 * (nvs.save_angle) / 90.0))
        moto.move_angle(angle_value, wait_flag)
    elif unit == "degrees":    
        angle_value = int(round(angle_value * (nvs.save_angle) / 90.0))
        moto.move_angle(angle_value, wait_flag)

def start_moving(dir):
    if dir == "forward":
        moto.move_speed(108, 108)
    elif dir == "backward":
        moto.move_speed(-108, -108)
    elif dir == "left":
        moto.move_speed(-108, 108)
    elif dir == "right":
        moto.move_speed(108, -108)

def stop_moving():
    return moto.stop(2)

def motor_run(motor, value, unit, speed, dir, wait_flag = False):
    motor_temp = motor
    speed_temp = abs(int(speed))
    if motor_temp == "A+B":
        motor_temp = 2
    elif motor_temp == "A":
        motor_temp = 0
    elif motor_temp == "B":
        motor_temp = 1    
    if unit == "seconds":
        if(dir == "clockwise"):
            moto.motor_speed(motor_temp, speed_temp)
        else:
            moto.motor_speed(motor_temp, speed_temp)
            time.sleep(abs(value))
    elif unit == "rotations":
        moto.set_max_speed(speed_temp * 8 * 32)
        pos = int(round(45.6 * 3.14 * value))
        moto.motor_run(motor_temp, pos, wait_flag)    # WHEEL_DIAMETER_VALUE
    elif unit == "degrees":
        moto.set_max_speed(speed_temp * 8 * 32)
        pos = int(round(45.6 * 3.14 * value / 360.0))
        moto.motor_run(motor_temp, pos, wait_flag)    # WHEEL_DIAMETER_VALUE

def motor_pwm(motor, speed):
    motor_temp = motor
    if motor_temp == "A+B":
        motor_temp = 2
    elif motor_temp == "A":
        motor_temp = 0
    elif motor_temp == "B":
        motor_temp = 1
    moto.motor_pwm(motor_temp, speed)

def get_motion_status(motor):
    return moto.get_motion_status(motor)

def get_motor_speed(motor):
    return moto.get_motor_speed(motor)
    
def stop(motor):
    motor_temp = motor
    if motor_temp == "A+B":
        motor_temp = 2
    elif motor_temp == "A":
        motor_temp = 0
    elif motor_temp == "B":
        motor_temp = 1
    moto.stop(motor_temp)
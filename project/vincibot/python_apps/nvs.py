#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matatalab

OTA_NAMESPACE = "ota_config"
USER_NAMESPACE = "user_config"
MACHINE_NAMESPACE = "machine_info"

FIREFLY_VERSION = "firefly_version"
HARDWARE_VER = "hardware_ver"
WIFI_SSID       = "wifi_ssid"
OTA_BLE         = "ota_ble_control"
WIFI_PASSWORD   = "wifi_password"
OTA_HOST_NAME   = "ota_host_name"
OTA_INFO_URL    = "ota_info_url"
OTA_LANGUAGE    = "ota_language"

SAVE_ANGLE = "save_angle"
SAVE_LEFT_ANGLE = "left_angle"
SAVE_RIGHT_ANGLE = "right_angle"
LEFT_WHEEL = 0
RIGHT_WHEEL = 1
save_angle = 90
save_left_angle = 90
save_right_angle = 90
hardware_version = "1.0.0"

def write(key = '', content = None, namespace = USER_NAMESPACE):
    if (not isinstance(namespace, str)) or (not isinstance(key,str)) or (content is None):
        return   
    matatalab.nvs_write_string(namespace, key, content)          

def read(key = '', namespace = USER_NAMESPACE):
    if (not isinstance(namespace, str)) or (not isinstance(key,str)):
        return
    content_string = matatalab.nvs_read_string(namespace, key)
    if isinstance(content_string, str):
        return content_string

    return None

def set_hardware_version():
    global hardware_version
    hardware_version = version
    write(HARDWARE_VER, hardware_version, MACHINE_NAMESPACE)

def get_hardware_version():
    global hardware_version
    return hardware_version

def init_machine_config():
    global hardware_version
    version = read(HARDWARE_VER, MACHINE_NAMESPACE)
    if version is not None:
        hardware_version = version
    else:
        print("read hardware_version failed! use default value")
        write(HARDWARE_VER, hardware_version, MACHINE_NAMESPACE)

def init_calibration_value():
    global save_angle, save_left_angle, save_right_angle
    left_angle = read(SAVE_LEFT_ANGLE, USER_NAMESPACE)
    if left_angle is not None:
        save_left_angle = int(left_angle)
    else:
        print("read save_left_angle failed! use default value 90")
        save_left_angle = 90
        write(SAVE_LEFT_ANGLE, str(save_left_angle), USER_NAMESPACE)

    right_angle = read(SAVE_RIGHT_ANGLE, USER_NAMESPACE)
    if right_angle is not None:
        save_right_angle = int(right_angle)
    else:
        print("read save_right_angle failed! use default value 90")
        save_right_angle = 90
        write(SAVE_RIGHT_ANGLE, str(save_right_angle), USER_NAMESPACE)

    angle = read(SAVE_ANGLE, USER_NAMESPACE)
    if angle is not None:
        save_angle = int(angle)
    else:
        print("read save_angle failed! use default value 90")
        save_angle = 90
        write(SAVE_ANGLE, str(save_angle), USER_NAMESPACE)

def calibration_angle(wheel, ad):
    global save_angle, save_left_angle, save_right_angle
    if(wheel == LEFT_WHEEL):
        if(ad == 1):
            save_left_angle = save_left_angle + 1
        elif(ad == 0):
            save_left_angle = save_left_angle - 1
        else:
        	save_left_angle = save_left_angle
        print("calibration left angle to %d" %(save_left_angle))
        write(SAVE_LEFT_ANGLE, str(save_left_angle), USER_NAMESPACE)
        save_left_angle = int(read(SAVE_LEFT_ANGLE, USER_NAMESPACE))
    elif(wheel == RIGHT_WHEEL):
        if(ad == 1):
            save_right_angle = save_right_angle + 1
        elif(ad == 0):
            save_right_angle = save_right_angle - 1
        else:
        	save_right_angle = save_right_angle
        print("calibration right angle to %d" %(save_right_angle))
        write(SAVE_RIGHT_ANGLE, str(save_right_angle), USER_NAMESPACE)
        save_right_angle = int(read(SAVE_RIGHT_ANGLE, USER_NAMESPACE))
    else:
        if(ad == 1):
            save_angle = save_angle + 1
        elif(ad == 0):
            save_angle = save_angle - 1
        else:
        	save_angle = save_angle
        print("calibration angle to %d" %(save_angle))
        write(SAVE_ANGLE, str(save_angle), USER_NAMESPACE)
        save_angle = int(read(SAVE_ANGLE, USER_NAMESPACE))



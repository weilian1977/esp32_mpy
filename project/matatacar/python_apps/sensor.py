import time
import matatalab


_sensor = matatalab.sensor()

def get_left_light():
    return _sensor.get_light_value(1);

def get_right_light():
    return _sensor.get_light_value(0);

def get_obstacle_avoidance_value():
    return _sensor.get_obstacle_avoidance_value();

def get_left_infrared_tube():
    return _sensor.get_line_follower_2_value();

def get_right_infrared_tube():
    return _sensor.get_line_follower_1_value();

def get_infrared_tube():
    left_value = get_left_infrared_tube()
    right_value = get_right_infrared_tube()
    if left_value > 2.8:
        left_value = 0x01
    else:
        left_value = 0x00
    if right_value > 2.8:
        right_value = 0x01
    else:
        right_value = 0x00
    return ((right_value << 1) | left_value)

def get_color_raw():
    return _sensor.get_color()

def get_color():
    return _sensor.get_color()

def send_ir_code(addr, command):
    _sensor.send_ir_code(addr, command)

def get_ir_code():
    return _sensor.get_ir_code()
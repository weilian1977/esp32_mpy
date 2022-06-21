#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time
import ble
import matatalab
import drv_led
import drv_motion
import protocol_v2
import action
import drv_system
import audio
import factory
import system_state
import sensor
import nvs

PROTOCOL_V1_MOVE_FORWARD    = 0x00
PROTOCOL_V1_MOVE_LEFT       = 0x01
PROTOCOL_V1_MOVE_RIGHT      = 0x02
PROTOCOL_V1_MOVE_BACKWARD   = 0x03

PROTOCOL_V1_PLAY_DEFAULT_MUSIC    = 0x19
PROTOCOL_V1_PLAY_DEFAULT_DANCE    = 0x1A
PROTOCOL_V1_PLAY_DEFAULT_ACTION   = 0x1B

PROTOCOL_V1_MOVE_FORWARD_MULTI    = 0x20
PROTOCOL_V1_MOVE_LEFT_MULTI       = 0x21
PROTOCOL_V1_MOVE_RIGHT_MULTI      = 0x22
PROTOCOL_V1_MOVE_BACKWARD_MULTI   = 0x23

PROTOCOL_V1_WAIT_TYPE1            = 0x25

PROTOCOL_V1_PLAY_MUSIC            = 0x39
PROTOCOL_V1_PLAY_DANCE            = 0x3A
PROTOCOL_V1_PLAY_ACTION           = 0x3B
PROTOCOL_V1_PLAY_BASS             = 0x50
PROTOCOL_V1_PLAY_TREBLE           = 0x51
PROTOCOL_V1_PLAY_MELODY           = 0x54
PROTOCOL_V1_FORWARD_ADJUST        = 0x60
PROTOCOL_V1_LEFT_ADJUST           = 0x61
PROTOCOL_V1_RIGHT_ADJUST          = 0x62
PROTOCOL_V1_BACKWARD_ADJUST       = 0x63

PROTOCOL_V1_CONTROL               = 0x64
PROTOCOL_V1_SENSOR_EVENT          = 0x66
PROTOCOL_V1_IR_SEND               = 0x67

PROTOCOL_V1_PLAY_BASS_BEAT             = 0x70
PROTOCOL_V1_PLAY_TREBLE_BEAT           = 0x71

PROTOCOL_V1_FACTORY = 0x80
PROTOCOL_V1_CANCEL = 0x84
PROTOCOL_V1_PLAY_VOICE = 0x85

PROTOCOL_V1_CONTROL_MOTO_1    = 0x01
PROTOCOL_V1_CONTROL_MOTO_2    = 0x02
PROTOCOL_V1_CONTROL_MOTO_3    = 0x03
PROTOCOL_V1_CONTROL_MOTO_4    = 0x04
PROTOCOL_V1_CONTROL_LEFT_EYE    = 0x05
PROTOCOL_V1_CONTROL_RIGHT_EYE    = 0x06
PROTOCOL_V1_CONTROL_PLAY_MUSIC_LOOP    = 0x07
PROTOCOL_V1_CONTROL_STOP_MUSIC    = 0x08
PROTOCOL_V1_CONTROL_STOP_MOTO    = 0x0A
PROTOCOL_V1_CONTROL_BOTH_EYE    = 0x0B


MSG_FRAMER_HEADER        = 0xFE
MSG_FRAMER_TRANSLATION   = 0xFD
CHAR_TRANSLATION_OFFSET  = 0x20



LEFT_EYE_ID    = 0
RIGHT_EYE_ID   = 1

note_offset = {
1:0,
2:2,
3:4,
4:5,
5:7,
6:9,
7:11,
}

# current_time = 0
previous_opcode = 0xFE
play_start_time = 0
ble_msg_stop_time = 0
is_first_stop = False
command_start = False

def motion_forward(carble, msg):
    print("motion_forward")
    drv_motion.forward(1, 1)
    if system_state.get_cmd_cancel_flag():
        print("cancel motion_forward")
        return
    time.sleep_ms(550)

def motion_left(carble, msg):
    print("motion_left")
    drv_motion.turn_left(90, 1)
    if system_state.get_cmd_cancel_flag():
        print("cancel motion_left")
        return
    time.sleep_ms(550)

def motion_right(carble, msg):
    print("motion_right")
    drv_motion.turn_right(90, 1)
    if system_state.get_cmd_cancel_flag():
        print("cancel motion_right")
        return
    time.sleep_ms(550)

def motion_backward(carble, msg):
    drv_motion.backward(1, 1)
    if system_state.get_cmd_cancel_flag():
        print("cancel motion_backward")
        return
    time.sleep_ms(550)
    print("motion_backward") 

def play_default_music(carble, msg):
    #action.set_vol(60)
    audio.play_music(1)
    #action.play('/music/music1.mp3')
    print("play_default_music")

def play_default_dance(carble, msg):
    action.action_dance(1)
    time.sleep_ms(100)
    print("play_default_dance")

def play_default_movement(carble, msg):
    action.action_movement(1)
    time.sleep_ms(200)
    print("play_default_movement")

def motion_forward_multi(carble, msg):
    for i in range(msg[1]):
        cmd_cancel_flag = system_state.get_cmd_cancel_flag()
        if(cmd_cancel_flag == True):
            print("cancel motion_forward_multi")
            return
        drv_motion.forward(1, 1)
        time.sleep_ms(200)
    time.sleep_ms(550)
    print("motion_forward_multi")

def motion_left_multi(carble, msg):
    angle = msg[1]
    if(angle == 0x01):
        angle = 90
    drv_motion.turn_left(angle, 1)
    time.sleep_ms(550)
    print("motion_left_multi")

def motion_right_multi(carble, msg):
    angle = msg[1]
    if(angle == 0x01):
        angle = 90
    drv_motion.turn_right(angle, 1)
    time.sleep_ms(550)
    print("motion_right_multi")

def motion_backward_multi(carble, msg):
    for i in range(msg[1]):
        drv_motion.backward(1, 1)
        cmd_cancel_flag = system_state.get_cmd_cancel_flag()
        if(cmd_cancel_flag == True):
            print("cancel motion_backward_multi")
            return
        time.sleep_ms(200)
    time.sleep_ms(200)
    print("motion_backward_multi")

def wait_type1(carble, msg):
    global previous_opcode
    # global current_time
    if((drv_motion.get_motion_status(0) == 1) and (drv_motion.get_motion_status(1) == 1) and (drv_motion.get_motor_speed(0) == -drv_motion.get_motor_speed(1))):
        if previous_opcode == 0x25:
            t = msg[1] * 1.1 - 0.12                    # 100/90
        else:
            t = msg[1] * 1.1 - 0.32
    elif((drv_motion.get_motion_status(0) == 1) or (drv_motion.get_motion_status(1) == 1)):
        if previous_opcode == 0x25:
            t = msg[1] * 1.352 - 0.12                  # 62*2*3.14/72
        else:
            t = msg[1] * 1.352 - 0.32
    else:
        t = msg[1] - 0.12
    time.sleep(t)
    print("wait %f second" %(t))

def play_music(carble, msg):
    print("play_music")
    if(msg[1] > 0) and (msg[1] <= 6):
        audio.play_music(msg[1], False)
    if(msg[1] > 6) and (msg[1] <= 12):
        audio.play_music((msg[1] - 6), False)
    if(msg[1] > 12) and (msg[1] <= 18):
        audio.play_dance((msg[1] - 12), False)

def play_dance(carble, msg):
    action.action_dance(msg[1])
    time.sleep_ms(100)
    print("play_dance")

def play_movement(carble, msg):
    action.action_movement(msg[1])
    time.sleep_ms(200)
    print("play_movement")

def play_alto(carble, msg):
    audio.play_alto(msg[1])
    print("play_bass")

def play_treble(carble, msg):
    audio.play_treble(msg[1])
    print("play_treble")

def play_melody(carble, msg):
    audio.play_melody(msg[1])
    print("play_melody")
    
def motion_adjust_forward(carble, msg):
    print("motion_adjust_forward not support!")

def motion_adjust_left(carble, msg):
    print("motion_adjust_left(%d,%d)" %(msg[1], msg[2]))
    nvs.calibration_angle(nvs.LEFT_WHEEL, msg[2])

def motion_adjust_right(carble, msg):
    print("motion_adjust_right(%d,%d)" %(msg[1], msg[2]))
    nvs.calibration_angle(nvs.RIGHT_WHEEL, msg[2])

def motion_adjust_backward(carble, msg):
    print("motion_adjust_backward not support!")

def music_play(carble, msg):
    time.sleep_ms(100)
    
def music_stop(carble, msg):
    action.action_play_stop()
    time.sleep_ms(100)

def car_action_control(carble, msg):
    # global current_time
    control_code = msg[2]
    if(control_code in [1,2,3,4]):
        # current_time = time.ticks_ms()
        left_speed_level = ((msg[1] >> 4) & 0xF)
        right_speed_level = (msg[1] & 0xF)
        if (left_speed_level == 0xf):
            left_speed = "unchanged"
        else:
            left_speed_level = ((msg[1] >> 4) & 0xF) - 6
            if(0 < left_speed_level) and (left_speed_level <= 6):
                left_speed = (left_speed_level * 18) + 18
            elif(left_speed_level < 0) and (left_speed_level >= -6):
                left_speed = (left_speed_level * 18) - 18
            else:
                left_speed = "stop"
        if(right_speed_level == 0xf):
            right_speed = "unchanged"
        else:
            right_speed_level = (msg[1] & 0xF) - 6
            if(0 < right_speed_level) and (right_speed_level <= 6):
                right_speed = (right_speed_level * 18) + 18
            elif(right_speed_level < 0) and (right_speed_level >= -6):
                right_speed = (right_speed_level * 18) - 18
            else:
                right_speed = "stop"
        drv_motion.move_speed(left_speed, right_speed)
    if(control_code in [1,2]):
        time.sleep_ms(200)

    if(control_code == PROTOCOL_V1_CONTROL_LEFT_EYE):
        drv_led.led_eye_set(0, msg[1])
        time.sleep_ms(100)
        
    if(control_code == PROTOCOL_V1_CONTROL_RIGHT_EYE):
        drv_led.led_eye_set(1, msg[1])
        time.sleep_ms(100)

    if(control_code == PROTOCOL_V1_CONTROL_PLAY_MUSIC_LOOP):
        music_play(carble, msg)
        
    if(control_code == PROTOCOL_V1_CONTROL_STOP_MUSIC):
        music_stop(carble, msg)
        print("stop music")

    if(control_code == PROTOCOL_V1_CONTROL_STOP_MOTO):
        drv_motion.stop(2)
    
    if(control_code == PROTOCOL_V1_CONTROL_BOTH_EYE):
        drv_led.led_eye_set(2, msg[1])
        time.sleep_ms(100)
    print("car_action_control")

def play_alto_beat(carble, msg):
    audio.play_alto(msg[1], msg[2])
    print("play_bass_beat")

def play_treble_beat(carble, msg):
    audio.play_treble(msg[1], msg[2])
    print("play_treble_beat")

def factory_cmd(carble, msg):
    print("factory_cmd")
    carble.set_heart_enable(False)
    factory.factory_process(carble, msg)

def play_start_voice(carble, msg):
    audio.play_system('start.mp3', False, 100)
    print("play_start_voice")
    
def car_cmd_unknow(carble, msg):
    print("car_cmd_unknow",msg) 

def check_new_protocol(carble, msg):
    print("check_new_protocol",msg)
    header = [0xFE, 0x07, 0x7E]
    
    if(msg[0:3]!= bytes(header)):
        return 
    if(len(msg) < header[1]):
        print("len error", len(msg))

    translate_flag = False
    cnt = 0
    #buffer = [].append(msg(0))
    buffer = list(msg)
    cnt = buffer.count(MSG_FRAMER_TRANSLATION)
    for i in range(cnt):
        index = buffer.index(MSG_FRAMER_TRANSLATION)
        buffer[index+1] = buffer[index+1] + CHAR_TRANSLATION_OFFSET
        buffer.pop(index)
    crc_recive = (buffer[msg[1]] << 8) + buffer[msg[1]+1]
    crc_calc = ble.crc16(buffer[1:(msg[1])], 0xFFFF)
    if(crc_recive == crc_calc):
        if(msg[3] == 2):
            carble.set_protocol(ble.PROTOCOL_VER_V2)
            protocol_v2.new_protocol_response_to_uplink(carble)
            print("change to protocol v2")

        else:
            print("protocol v%d not support"%(msg[3]))
        carble.used_buffer_clear()
        return
    print("error.crc_recive %x, crc calc:%x"%(crc_recive, crc_calc))

def sensor_event(carble, msg):
    if msg[1] != 0x10:
        print("not sensor_event!")
        return
    event_monitor_id = msg[2]
    print("event_monitor_id:%d" %(event_monitor_id))
    if event_monitor_id == 0x01:
        pass
    elif event_monitor_id == 0x02:
        pass
    elif event_monitor_id == 0x03:
        pass
    elif event_monitor_id == 0x04:
        while(True):
            if(sensor.get_obstacle_avoidance_value() > 2.8):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait obstacle approaching")

    elif event_monitor_id == 0x05:
        pass
    elif event_monitor_id == 0x06:
        while(True):
            if(sensor.get_left_light() > 1) or (sensor.get_right_light() > 1):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait the light gets stronger")

    elif event_monitor_id == 0x07:
        while(True):
            if(sensor.get_left_light() < 0.5) and (sensor.get_right_light() < 0.5):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait dim light")

    elif event_monitor_id == 0x08:
        pass
    elif event_monitor_id == 0x09:
        pass
    elif event_monitor_id == 0x0a:
        while(True):
            if(sensor.get_obstacle_avoidance_value() < 0.6):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait obstacle principle")

    elif event_monitor_id == 0x0b:    #0xf30c
        while(True):
            ir_code_tuple = sensor.get_ir_code()
            if(ir_code_tuple[3] == 0xf30c):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait message 1")
    elif event_monitor_id == 0x0c:    #0xe718
        while(True):
            ir_code_tuple = sensor.get_ir_code()
            if(ir_code_tuple[3] == 0xe718):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait message 2")
    elif event_monitor_id == 0x0d:    #0xa15e
        while(True):
            ir_code_tuple = sensor.get_ir_code()
            if(ir_code_tuple[3] == 0xa15e):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait message 3")
    elif event_monitor_id == 0x0e:    #0xf708
        while(True):
            ir_code_tuple = sensor.get_ir_code()
            if(ir_code_tuple[3] == 0xf708):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait message 4")
    elif event_monitor_id == 0x0f:    #0xe31c
        while(True):
            ir_code_tuple = sensor.get_ir_code()
            if(ir_code_tuple[3] == 0xe31c):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait message 5")
    elif event_monitor_id == 0x10:    #0xa55a
        while(True):
            ir_code_tuple = sensor.get_ir_code()
            if(ir_code_tuple[3] == 0xa55a):
                return
            else:
                carble.clear_heart_time()
                time.sleep_ms(50)
                print("wait message 6")
    elif event_monitor_id == 0x11:
        pass
    elif event_monitor_id == 0x12:
        pass
    elif event_monitor_id == 0x13:
        pass
    elif event_monitor_id == 0x14:
        pass

def ir_send(carble, msg):
    if msg[1] != 0x01:
        print("not ir_send!")
        return
    ir_command = 0x0000;
    ir_addr = 0xfe00;
    if msg[2] == 1:
        ir_command = 0xf30c
    elif msg[2] == 2:
        ir_command = 0xe718
    elif msg[2] == 3:
        ir_command = 0xa15e
    elif msg[2] == 4:
        ir_command = 0xf708
    elif msg[2] == 5:
        ir_command = 0xe31c
    elif msg[2] == 6:
        ir_command = 0xa55a
    sensor.send_ir_code(ir_addr, ir_command)
    print("ir_send msg:%d, cmd:%#x" %(msg[2], ir_command))

################################################
# opcode:  cmd_len   function
opcode = {
0x00:[1, motion_forward],
0x01:[1, motion_left],
0x02:[1, motion_right],
0x03:[1, motion_backward],

0x19:[1, play_default_music],
0x1A:[1, play_default_dance],
0x1B:[1, play_default_movement],
0x20:[2, motion_forward_multi],
0x21:[2, motion_left_multi],
0x22:[2, motion_right_multi],
0x23:[2, motion_backward_multi],
0x25:[2, wait_type1],
0x39:[2, play_music],
0x3A:[2, play_dance],
0x3B:[2, play_movement],
0x50:[2, play_alto],
0x51:[2, play_treble],
0x54:[2, play_melody],
0x60:[3, motion_adjust_forward],
0x61:[3, motion_adjust_left],
0x62:[3, motion_adjust_right],
0x63:[3, motion_adjust_backward],
0x64:[3, car_action_control],
0x66:[3, sensor_event],
0x67:[3, ir_send],
0x70:[3, play_alto_beat],
0x71:[3, play_treble_beat],
0x80:[0xFF, factory_cmd],

0x85:[1, play_start_voice],
0xFE:[0xFF, check_new_protocol]
}

##0x84:[1, cancel_cmd],

def clear_pro_audio_play_flag():
    global is_first_stop
    global command_start
    is_first_stop = False
    command_start = False

def protocol_v1_process(carble):
    global previous_opcode
    global play_start_time
    global command_start
    global ble_msg_stop_time
    global is_first_stop
    cmd_cancel_flag = system_state.get_cmd_cancel_flag()
    if(cmd_cancel_flag == True):
        command_start = False
        is_first_stop = False
        play_start_time = 0
        system_state.set_cmd_cancel_flag(False)
    
    msg = carble.get_data()
    msglen = len(msg)

    if(msglen == 0):
        return
    func = None

    while msglen:
        drv_system.clear_idle_time()
        [cmd_len, func] = opcode.get(msg[0], [0xFF,car_cmd_unknow])
        if(cmd_len == 0xFF):
            # msglen = 0
            carble.used_buffer_clear()
            command_start = False
            is_first_stop = False
            play_start_time = 0
        elif(cmd_len > msglen):
            #have not enough data
            break
        else:
            carble.used_buffer_clear(cmd_len)

        func(carble, msg) 
        previous_opcode = msg[0]

        cmd_cancel_flag = system_state.get_cmd_cancel_flag()
        if(cmd_cancel_flag == True):
            command_start = False
            is_first_stop = False
            play_start_time = 0
            system_state.set_cmd_cancel_flag(False)

        msg = carble.get_data()
        msglen = len(msg)

    if(func != factory_cmd) and (func != check_new_protocol):
        carble.send_heart()
    carble.clear_heart_time()
    time.sleep_ms(100)
        
def commnication_test():
    buffer=[11,2,3,3]
    buffer = bytes(buffer)
    i = 0
    protocol_v1_process(buffer)
    print(buffer)
        
#commnication_test()    
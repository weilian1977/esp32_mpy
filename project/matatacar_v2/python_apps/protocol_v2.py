#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time
import ble
import matatalab
import drv_led
import drv_motion
import system_def
import action
import drv_system
import audio_play
# import ota
import wifi
import system_state
import sensor

rx_msg = b''

MSG_FRAMER_HEADER        = 0xFE
MSG_FRAMER_TRANSLATION   = 0xFD
CHAR_TRANSLATION_OFFSET  = 0x20

MESSAGE_RX_OK                     = 0
MESSAGE_ERROR_CRC                 = 1
MESSAGE_ERROR_BUFFER_OVERFLOW     = 2
MESSAGE_ERROR_NOT_SUPPORTED_CMD   = 3
MESSAGE_ERROR_INVALID_PARAM       = 4
MESSAGE_ERROR_INVALID_LENGTH      = 5
MESSAGE_ERROR_TIMEOUT             = 6
MESSAGE_ERROR_CONTROL_MODE_ERROR  = 7

SYSTEM_INFO_VERSION       = 0x01
SYSYTEM_INFO_MAC          = 0x02
SYSTEM_INFO_PROTOCOL      = 0x03
SYSTEM_INFO_DEVICE_ID     = 0x04

note_id_offset = {
    131:1,
    147:2,
    165:3,
    175:4,
    196:5,
    220:6,
    247:7,
    262:1,
    294:2,
    330:3,
    349:4,
    392:5,
    440:6,
    494:7,
}

note_freq_table = {
    #C2
    33:24,
    35:25,
    37:26,
    39:27,
    41:28,
    44:29,
    46:30,
    49:31,
    52:32,
    55:33,
    58:34,
    62:35,
    
    #C3
    65:36,
    69:37,
    73:38,
    78:39,
    82:40,
    87:41,
    93:42,
    98:43,
    104:44,
    110:45,
    117:46,
    124:47,
    
    #C4
    131:48,
    139:49,
    147:50,
    156:51,
    165:52,
    175:53,
    185:54,
    196:55,
    208:56,
    220:57,
    233:58,
    247:59,
    
    #C5 中音
    262:60,
    277:61,
    294:62,
    311:63,
    330:64,
    349:65,
    370:66,
    392:67,
    415:68,
    440:69,
    466:70,
    494:71,
    
    #C6
    523:72,
    554:73,
    587:74,
    622:75,
    659:76,
    699:77,
    740:78,
    784:79,
    831:80,
    880:81,
    932:82,
    988:83,
    
    #C7
    1047:84,
    1109:85,
    1175:86,
    1245:87,
    1319:88,
    1397:89,
    1480:90,
    1568:91,
    1661:92,
    1760:93,
    1865:94,
    1976:95,
}

def msg_normal_response(carble, err_type):
    msg = [0x88, err_type]
    ble.message_send(carble, msg)

def get_msg_frame(carble):
    rx_msg = carble.get_data()
    if(len(rx_msg) == 0):
        return None
    if((rx_msg[0] == 0x80) and (rx_msg[1] == 0xaa) and (rx_msg[2] == 0x55)):
        frame_data = rx_msg
        carble.used_buffer_clear()
        return frame_data
    for i in range(len(rx_msg)):
        if(rx_msg[i] == MSG_FRAMER_HEADER):
            break
    if (i == (len(rx_msg) - 1)):
        # not find framer header
        carble.used_buffer_clear(i+1)
        return None
        
    if i != 0:
        carble.used_buffer_clear(i)
        
    rx_msg = rx_msg[i:]
        
    buffer = list(rx_msg)
    payload_len = buffer[1]
    if(payload_len < 2):
        msg_normal_response(carble, MESSAGE_ERROR_INVALID_LENGTH)
        carble.used_buffer_clear(2)
        return None
    ## message 1(0xFE) + 1(len)+payload_len(payload + 2(crc))
    frame_len = 1 + 1 + payload_len
    if(frame_len > len(buffer)):
        #have not recive enough message#
        return None
    frame_data = buffer[:frame_len]
    crc_calc = ble.crc16(bytes(frame_data[1:payload_len]))
    crc_recive = (buffer[payload_len] << 8) + buffer[payload_len+1]
    carble.used_buffer_clear(frame_len)
    
    if(crc_calc != crc_recive):
        print("CRC error. recive crc :0x%x, calc crc: 0x%x"%(crc_recive, crc_calc))
        msg_normal_response(carble, MESSAGE_ERROR_CRC)
        return None
        
    else:
        return bytes(frame_data)


def cmd_system_info(carble, cmd):
    send_msg = [0x01, cmd[0]]
    if(cmd[0] == SYSTEM_INFO_VERSION):
        firmware_version = matatalab.get_version()
        version_list = firmware_version.split('.')
        soft_ver = [int(version_list[0], 16), int(version_list[1], 16), int(version_list[2], 16)]
        stm32_ver = [0x01, 0x01, 0x00]
        music_ver = [0x00, 0x00, system_def.SOFT_MUSIC_VERSION]
        send_msg.extend(soft_ver)
        send_msg.extend(stm32_ver)
        send_msg.extend(music_ver)
    elif(cmd[0] == SYSYTEM_INFO_MAC):
        # send_msg = machine.unique_id()
        ret = matatalab.get_mac()
        send_msg.extend(ret)
    elif(cmd[0] == SYSTEM_INFO_PROTOCOL):
        protocol_ver = [system_def.PROTOCOL_VERSION1, system_def.PROTOCOL_VERSION2, system_def.PROTOCOL_VERSION3]
        send_msg.extend(protocol_ver)
        
    elif(cmd[0] == SYSTEM_INFO_DEVICE_ID):
        device_id = [system_def.DEVICE_ID_CAR]
        send_msg.extend(device_id)
    else:
        msg_normal_response(carble, MESSAGE_ERROR_INVALID_PARAM)
        return
        
    ble.message_send(carble, send_msg)
    
def cmd_read_sn(carble, cmd):
    sn_segment = cmd[0]
    if(sn_segment > 3):
        msg_normal_response(carble, MESSAGE_ERROR_INVALID_PARAM)
    send_msg = [0x02, sn_segment]

    if sn_segment == 0:
        sn = nvs.read("sn0")
    if sn_segment == 1:
        sn = nvs.read("sn1")
    if sn_segment == 2:
        sn = nvs.read("sn2")
    if sn_segment == 3:
        sn = nvs.read("sn3")
    if(sn != None):
        send_msg.extend(str.encode(sn))
    else:
        sn = [0,0,0,0,0,0,0,0]
        send_msg.extend(sn)

    ble.message_send(carble, send_msg)

def cmd_battery_info(carble, cmd):
    if(cmd[0] != system_def.DEVICE_ID_CAR):
        msg_normal_response(carble, MESSAGE_ERROR_INVALID_PARAM)
        return
    send_msg = [0x03, cmd[0], cmd[1]]
    if(cmd[1] == 0x01):
        #get battery soc
        battery_soc = matatalab.get_battery_capacity()
        battery_soc_list = [battery_soc]
        send_msg.extend(battery_soc_list)
        
    if(cmd[1] == 0x02):
        #get battery charge state
        battery_charge_state = matatalab.is_usb_detected()
        battery_charge_state_list = [battery_charge_state]
        send_msg.extend(battery_charge_state_list)

    ble.message_send(carble, send_msg)

def cmd_time_delay(carble, cmd):
    delay = (cmd[0] << 8) | cmd[1]
    time.sleep(delay)
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_motion_contol(carble, cmd):
    parameter = (cmd[1] << 8) | cmd[2]
    print("type %s parameter %d"%(type(parameter), parameter))
    if(cmd[0] == 0x01):
        drv_motion.move_position(parameter, 1)
    elif(cmd[0] == 0x02):
        drv_motion.move_position((-1) * parameter, 1)
    elif(cmd[0] == 0x03):
        drv_motion.move_angle((-1) * parameter, 1)
    elif(cmd[0] == 0x04):
        drv_motion.move_angle(parameter, 1)
    else:
        msg_normal_response(carble, MESSAGE_ERROR_INVALID_PARAM)
        return
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_animotion(carble, cmd):
    if(cmd[0] == 0x01):
        #left speed
        if(cmd[1] == 0x01):
            sign = 1
        else:
            sign = -1
        left_speed = (cmd[2] << 8) | cmd[3]
        drv_motion.move_speed(sign * left_speed, "unchanged")
        
    if(cmd[0] == 0x02):
        #right speed
        if(cmd[1] == 0x01):
            sign = 1
        else:
            sign = -1
        right_speed = (cmd[2] << 8) | cmd[3]
        drv_motion.move_speed("unchanged", sign * right_speed)
        
    if(cmd[0] == 0x03):
        #left speed
        if(cmd[1] == 0x01):
            left_sign = 1
        else:
            left_sign = -1
        left_speed = (cmd[2] << 8) | cmd[3]
        if(cmd[4] == 0x01):
            right_sign = 1
        else:
            right_sign = -1
        right_speed = (cmd[5] << 8) | cmd[6]
        print("left_speed (%d) -- right_speed (%d)" %(left_speed, right_speed))
        drv_motion.move_speed(left_sign * left_speed, right_sign * right_speed)
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_dance(carble, cmd):
    print("dance")
    action.action_dance(cmd[0])
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_action(carble, cmd):
    print("action")
    action.action_movement(cmd[0])
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_motion_adjust(carble, cmd):
    print("cmd_motion_adjust")
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_play_note(carble, cmd):
    print("cmd_play_note")
    freq = (cmd[0] << 8) | cmd[1]
    play_time = (cmd[2] << 8) | cmd[3]
    beat = play_time / 500
    if freq >= 131 and freq <= 247:
        audio_play.play_alto(note_id_offset[freq], beat)
    elif freq >= 262 and freq <= 494:
        audio_play.play_treble(note_id_offset[freq], beat)
    else:
        tone = note_freq_table.get(freq)
        audio_play.play_tone(tone, beat)
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_play_music(carble, cmd):
    print("cmd_play_music")
    if(cmd[0] == 0x04):
        audio_play.play_stop()
        msg_normal_response(carble, MESSAGE_RX_OK)
        return
    elif(cmd[0] == 0x01):
        wait_flag = 1
    elif(cmd[0] == 0x02):
        wait_flag = 0
    elif(cmd[0] == 0x03):
        print("Yanminge test")
        pass
    else:
        msg_normal_response(carble, MESSAGE_ERROR_INVALID_PARAM)
        return
        
    if((0 < cmd[1]) and (cmd[1] <= 0xa)):
        audio_play.play_melody(cmd[1], 1)
    elif((0x10 < cmd[1]) and (cmd[1] <= 0x16)):
        audio_play.play_music((cmd[1] - 0x10), 1)
    elif(0x20 <= cmd[1]) and (cmd[1] < 0x2f):
        audio_play.play_move((cmd[1] - 0x20), 1)
    else:
        print("invalid music")
        msg_normal_response(carble, MESSAGE_ERROR_INVALID_PARAM)
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_eye(carble, cmd):
    print("cmd_eye")
    r = cmd[1]
    g = cmd[2]
    b = cmd[3]

    if(cmd[0] == 0x1):
        drv_led.led_eye_rgb_cfg(drv_led.LEFT_EYE_MODE, r, g, b)
    if(cmd[0] == 0x2):
        drv_led.led_eye_rgb_cfg(drv_led.RIGHT_EYE_MODE, r, g, b)
    if(cmd[0] == 0x3):
        drv_led.led_eye_rgb_cfg(drv_led.BOTH_EYE_MODE, r, g, b)
    
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_led_ring(carble, cmd):
    print("cmd_led_ring")
    

def cmd_emtion(carble, cmd):
    print("cmd_emtion")

def cmd_sensor(carble, cmd):
    print("cmd_sensor")


def cmd_get_parameter(carble, cmd):
    print("cmd_get_parameter")

def cmd_car_service_subscription(carble, cmd):
    print("cmd_car_service_subscription")

def cmd_control_service_subscription(carble, cmd):
    print("cmd_control_service_subscription")

def cmd_event_monitoring(carble, cmd):
    print("cmd_event_monitoring")

def cmd_test(carble, cmd):
    print("cmd_test")

def new_protocol_response_to_uplink(carble):
    send_msg = [0x7E, 0x01, 0x0, 0x0]
    ble.message_send(carble, send_msg)

def cmd_ascii_process(carble, cmd):
    sequence = (cmd[0] << 8) + cmd[1]
    ascii_cmd = cmd[2:-2].decode()
    print("seq:%d,cmd:%s"%(sequence, ascii_cmd))
    try:
        # exec(ascii_cmd)
        ret = eval(ascii_cmd)
        ret_s = '%s'%(ret)
        print(ret_s)
        err_code = 0
        send_msg = [0x70, cmd[0], cmd[1], err_code]
        send_msg.extend(str.encode(ret_s))
        print(send_msg)
        ble.message_send(carble, send_msg)
    except:
        try:
            exec(ascii_cmd)
            print('##%s## call by exec' %ascii_cmd)
            err_code = 0
            send_msg = [0x70, cmd[0], cmd[1], err_code]
            send_msg.extend(str.encode('None'))
            print(send_msg)
            ble.message_send(carble, send_msg)
        except:
            print('##%s## isn\'t callable' %ascii_cmd)
            err_code = 0xFF
            send_msg = [0x70, cmd[0], cmd[1], err_code]
            ret_s = 'exec error'
            send_msg.extend(str.encode(ret_s))
            ble.message_send(carble, send_msg)

def cmd_protocol(carble, cmd):
    if(cmd[0] == 0x01):
        print("change protocol to v1 version. ver: %02x.%02x.%02x"%(cmd[1], cmd[2], cmd[3]))
        carble.set_protocol(ble.PROTOCOL_VER_V1)
    if(cmd[0] == 0x03):
        print("change protocol to v3 version. ver: %02x.%02x.%02x"%(cmd[1], cmd[2], cmd[3]))
        carble.set_protocol(ble.PROTOCOL_VER_V3)
    new_protocol_response_to_uplink(carble)

def cmd_factory(carble, cmd):
    print("cmd_factory")

def cmd_long_trans(carble, cmd):
    print("cmd_long_trans")


def cmd_play_voice(carble, cmd):
    print("cmd_play_voice")
    audio_play.play_system('start.mp3', False, 100)
    msg_normal_response(carble, MESSAGE_RX_OK)

def cmd_unknow(carble, cmd):
    print("cmd_unknow")
    msg_normal_response(MESSAGE_ERROR_NOT_SUPPORTED_CMD)

    
################################################
# opcode:  cmd_len   function
opcode = {

0x01:[4,cmd_system_info],
0x02:[4,cmd_read_sn],
0x03:[4,cmd_battery_info],
0x04:[5,cmd_time_delay],

0x10:[6,cmd_motion_contol],
0x11:[0xff,cmd_animotion],
0x12:[4,cmd_dance],
0x13:[4,cmd_action],
0x14:[5,cmd_motion_adjust],
0x15:[7,cmd_play_note],
0x16:[5,cmd_play_music],
0x17:[7,cmd_eye],
0x18:[0xff,cmd_led_ring],
0x19:[4,cmd_emtion],

0x20:[5,cmd_sensor],
0x28:[5,cmd_get_parameter],

0x30:[8,cmd_car_service_subscription],
0x31:[8,cmd_control_service_subscription],
0x32:[5,cmd_event_monitoring],

0x50:[33,cmd_test],

0x70:[0xff,cmd_ascii_process],

0x7e:[7,cmd_protocol],

0x80:[0xff,cmd_factory],
0x81:[8,cmd_long_trans],

0x85:[3,cmd_play_voice],

}

def message_parse(carble):
    frame_data = get_msg_frame(carble)
    if(frame_data == None):
        return
    print("frame data:",frame_data)

    frame_len = frame_data[1]
    [cmd_len, func] = opcode.get(frame_data[2], [0xff, cmd_unknow])
    if (cmd_len != 0xff) and (cmd_len != frame_len):
        msg_normal_response(carble, MESSAGE_ERROR_INVALID_LENGTH)
    else:
        func(carble,frame_data[3:])

def protocol_v2_process(carble):

    cmd_cancel_flag = system_state.get_cmd_cancel_flag()
    if(cmd_cancel_flag == True):
        system_state.set_cmd_cancel_flag(False)
    msglen = carble.get_recive_data_len()
    if(msglen == 0):
        return
    drv_system.clear_idle_time()
    message_parse(carble)
    cmd_cancel_flag = system_state.get_cmd_cancel_flag()
    if(cmd_cancel_flag == True):
        system_state.set_cmd_cancel_flag(False)


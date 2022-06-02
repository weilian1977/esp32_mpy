# -*- coding:UTF-8 -*-

import system_def
import matatalab
import ble
import drv_system
import audio_play
import nvs
import time
import drv_motion
import system_state

# led = matatalab.leds()

def factory_get_version(carble, msg):
    ret = [0]*9
    firmware_version = matatalab.get_version()
    version_list = firmware_version.split('.')
    index = 0
    for value in version_list:
        ret[index] = int(value, 16)
        index = index + 1
    ret[8] = system_def.SOFT_MUSIC_VERSION
    carble.send_upstream(bytes(ret))
    
def factory_get_mac(carble, msg):
    ret = matatalab.get_mac()
    print('get mac',ret)
    carble.send_upstream(ret)
    
def factory_get_soc(carble, msg):
    ret = matatalab.get_battery_capacity()
    print('get soc', ret)
    soc = [ret]
    carble.send_upstream(bytes(soc))

def factory_set_sn(carble, msg):
    
    if msg[0] == 0:
        sn = msg[1:]
        nvs.write("sn0", sn)
    if msg[0] == 1:
        sn = msg[1:]
        nvs.write("sn1", sn)
    if msg[0] == 2:
        sn = msg[1:]
        nvs.write("sn2", sn)
    if msg[0] == 3:
        sn = msg[1:]
        nvs.write("sn3", sn)
    print("sn%d:%s"%(msg[0],sn))
    ret = [0]
    carble.send_upstream(bytes(ret))

def factory_get_sn(carble, msg):
    if msg[0] == 0:
        sn = nvs.read("sn0")
    if msg[0] == 1:
        sn = nvs.read("sn1")
    if msg[0] == 2:
        sn = nvs.read("sn2")
    if msg[0] == 3:
        sn = nvs.read("sn3")
    if(sn != None):
        carble.send_upstream(sn)
        print("sn%d:%s"%(msg[0],sn))
    else:
        sn = [0,0,0,0,0,0,0,0]
        carble.send_upstream(bytes(sn))

def factory_set_product_type(carble, msg):
    ret = ""
    if msg[0] == 0:
        drv_system.system_info.system_set_product_type("common")
        ret = "common"
    elif msg[0] == 1:
        drv_system.system_info.system_set_product_type("pro")
        ret = "pro"
    carble.send_upstream(ret)

def factory_set_system_led(carble, msg):
    if(len(msg) < 2):
        print("msg len error!")
        ret = [1]
        carble.send_upstream(bytes(ret))
        return
    if(msg[0] == 1):
        if(msg[1] == 0):
            matatalab.indicator_led(matatalab.OFF)
        else:
            matatalab.indicator_led(matatalab.ON)
    ret = [0]
    carble.send_upstream(bytes(ret))

def factory_power_off(carble, msg):
    ret = [0]
    carble.send_upstream(bytes(ret))
    drv_system.power_off()

def factory_speaker_test(carble, msg):
    audio_play.play_move(1)
    ret = [0]
    carble.send_upstream(bytes(ret))

def factory_aging_test_start(carble, item, test_hour):
    factory_test_cancel = [0x80, 0xaa, 0x55, 0x0d, 0x02, 0x00]
    carble.used_buffer_clear()
    test_start_time = time.time()
    ret = [0]
    carble.send_upstream(bytes(ret))
    while((time.ticks_diff(time.time(), test_start_time) < test_hour*3600)):
        if(item & (1 << 0)):
            audio_play.play_melody(1)
            # led.show_all(0, 0, 255)
        if(item & (1 << 1)):
            drv_motion.forward(1,1)
            # led.show_all(0, 255, 0)
            drv_motion.backward(1,1)
            # led.show_all(0, 0, 255)
        if(item & (1 << 2)):
            # led.show_all(255, 0, 0)
            pass
        if(item & (1 << 3)):
            matatalab.indicator_led(matatalab.SINGLE_FLASH)
        
        cmd = carble.get_data()
        if(len(cmd) > 5) and (cmd[0:6] == bytes(factory_test_cancel)):
            ret = [0]
            carble.send_upstream(bytes(ret))
            return
        
        time.sleep(0.1)
    ret = [0]
    carble.send_upstream(bytes(ret))
    drv_system.power_off()

def factory_aging_test_stop(carble):
    audio_play.play_stop()
    drv_motion.stop(2)
    ret = [0]
    carble.send_upstream(bytes(ret))

def factory_aging_test(carble, msg):
    if(msg[0] != 2):
        return
    
    if(msg[1] == 0):
        factory_aging_test_stop(carble)
    else:
        item = msg[1]
        test_hour = msg[2]
        factory_aging_test_start(carble, item, test_hour)

def factory_usb_detect(carble, msg):
    ret = matatalab.is_usb_detected()
    usb_detect = [ret]
    carble.send_upstream(bytes(usb_detect))

def factory_enter_dfu(carble, msg):
    pass

def factory_update_stm32(carble, msg):
    pass

def factory_heart_enable(carble, msg):
    carble.set_heart_enable(True)
    

def factory_get_version_keep_heart(carble, msg):
    factory_get_version(carble, msg)
    carble.set_heart_enable(True)

def factory_get_mac_keep_heart(carble, msg):
    factory_get_mac(carble, msg)
    carble.set_heart_enable(True)

def factory_get_soc_keep_heart(carble, msg):
    factory_get_soc(carble, msg)
    carble.set_heart_enable(True)

def factory_write_sn_keep_heart(carble, msg):
    factory_set_sn(carble, msg)
    carble.set_heart_enable(True)

def factory_read_sn_keep_heart(carble, msg):
    factory_get_sn(carble, msg)
    carble.set_heart_enable(True)
    
def factory_cmd_unknow(carble, msg):
    pass


def factory_emc_test_start():
    audio_play.play_music(1,False)
    # led.show_all(255, 255, 255)
    drv_motion.move_speed(100,100)
    time.sleep(5)
    # led.show_all(0, 0, 0)
    drv_motion.move_speed(0,0)
    time.sleep(1)
    # led.show_all(255, 255, 255)
    drv_motion.move_speed(-100,-100)
    time.sleep(5)
    # led.show_all(0, 0, 0)
    drv_motion.move_speed(0,0)
    time.sleep(1)
        
def factory_emc_test_stop():
    audio_play.play_stop()
    # led.show_all(0, 0, 0)
    drv_motion.move_speed(0,0)

KEY_START = 4
emc_test_flag = True
def factory_emc_key():
    global emc_test_flag
    old_start_key = False
    emc_test_flag = True
    while True:
        start_key = matatalab.get_power_state()
        print("start_key", start_key)
        #当按键旧值与新值不一致时触发
        if(old_start_key != start_key):
            old_start_key = start_key
            if(start_key == True):
                # start 按下
                if(emc_test_flag == False):
                    emc_test_flag = True
                else:
                    emc_test_flag = False
                    
        time.sleep(0.02)
        
        
def factory_emc_process():
    global emc_test_flag
    while(True):
        if(emc_test_flag == True):
            factory_emc_test_start()
        else:
            factory_emc_test_stop()
        time.sleep(0.02)

# opcode:   function
factory_opcode = {
0x01:[factory_get_version],
0x02:[factory_get_mac],
0x03:[factory_get_soc],
0x04:[factory_set_sn],
0x05:[factory_get_sn],
0x06:[factory_set_product_type],
0x08:[factory_set_system_led],
0x0A:[factory_power_off],
0x0B:[factory_speaker_test],
0x0D:[factory_aging_test],
0x0E:[factory_usb_detect],
0x81:[factory_enter_dfu],
0x91:[factory_update_stm32],
0x92:[factory_heart_enable],
0xA1:[factory_get_version_keep_heart],
0xA2:[factory_get_mac_keep_heart],
0xA3:[factory_get_soc_keep_heart],
0xA4:[factory_write_sn_keep_heart],
0xA5:[factory_read_sn_keep_heart],

}

def factory_process(carble, msg):
    if(len(msg) <= 3):
        return
        
    if(msg[0] == 0x80) and (msg[1] == 0xaa) and (msg[2] == 0x55):
        system_state.set_factory_state(True)
        print("factory_process")
        [func] = factory_opcode.get(msg[3], [factory_cmd_unknow])
        func(carble, msg[4:])
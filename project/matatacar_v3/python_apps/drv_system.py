import time
import matatalab
import audio
import os
import drv_motion
import system_state
import drv_led
import nvs
system_idle_time = time.time()
bat_test_flag = False
bat_test_val = 3.3

def system_get_step_distance():
    ret = nvs.read('step_dis')
    if ret is None:
        nvs.write('step_dis', '100')
        ret = 100;
    return int(ret)

def system_set_step_distance(dis):
    nvs.write('step_dis', str(dis))
    system_info.set_step_distance(str(dis));

class machine_info:
    def __init__(self):
        self.name = self.system_get_name()
        self.debug_mode = self.system_get_debug_mode()
        self.product_type = self.system_get_product_type()
        self.step_distance = system_get_step_distance()

    def system_set_name(self, manchine_name):
        nvs.write('machine_name', manchine_name, nvs.MACHINE_NAMESPACE)

    def system_get_name(self):
        machine_name = nvs.read('machine_name', nvs.MACHINE_NAMESPACE)
        if machine_name is None:
            machine_name = "MatataCar"
            self.name = machine_name
            self.system_set_name(machine_name)
        return machine_name

    def get_step_distance(self):
        return self.step_distance

    def set_step_distance(self, dis):
        self.step_distance = dis

    def system_set_debug_mode(self, mode):
        nvs.write('debug_mode', str(mode), nvs.MACHINE_NAMESPACE)
        self.debug_mode = int(mode)

    def system_set_product_type(self, pro_type):
        nvs.write('product_type', pro_type, nvs.MACHINE_NAMESPACE)
        self.product_type = pro_type

    def system_get_debug_mode(self):
        ret = nvs.read('debug_mode', nvs.MACHINE_NAMESPACE)
        if ret is None:
            self.debug_mode = 0
            self.system_set_debug_mode(self.debug_mode)
            ret = 0;
        return int(ret)

    def system_get_product_type(self):
        ret = nvs.read('product_type', nvs.MACHINE_NAMESPACE)
        if ret is None:
            self.product_type = 'common'
            self.system_set_product_type(self.product_type)
            ret = self.product_type
        return ret

    def get_real_time_debug_mode(self):
        return self.debug_mode

    def get_real_time_product_type(self):
        return self.product_type

system_info = machine_info()  

def get_battery_voltage():
    global bat_test_val, bat_test_flag
    if bat_test_flag:
        return bat_test_val
    else:
        return matatalab.get_battery_voltage()

def clear_idle_time():
    global system_idle_time
    system_idle_time = time.time()

def get_idle_time():
    global system_idle_time
    return (time.time() - system_idle_time)

def power_off():
    drv_led.led_eye_rgb_cfg(2, 0, 0, 0)
    matatalab.indicator_led(matatalab.OFF)
    audio.play_stop()  
    drv_motion.stop(2)
    audio.play('/sdcard/music/system/1-off.mp3', True)
    while True:
        matatalab.power_off()
        time.sleep(0.02)

def bat_test(val):
    global bat_test_val, bat_test_flag
    if val == 0:
        bat_test_flag = False
    else:
        bat_test_flag = True
        bat_test_val = val




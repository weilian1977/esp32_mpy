import time
import _thread
from communication import communication_process
from communication import ble_state_monitor
import events.event_manager as event_manager
from event_obj import event_o
import communication
import matatalab
import drv_system
import drv_led as leds
import drv_led_matrix
from utility import if_file_exists
from drv_led_matrix import led_matrix_process
import drv_motion as motion
import nvs
import sensor
import audio
import event
import action
import errno
import button
import machine, neopixel
import script
import random

power_led = neopixel.NeoPixel(machine.Pin(39), 1)
led_matrix = drv_led_matrix._display

KEY_UP = 0
KEY_DOWN = 1
POWER_OFF_TIME = 1000

THREAD_MAIN_SIZE = 8 * 1024
THREAD_COMMUNICATION_SIZE = 8 * 1024
THREAD_LED_MATRIX_SIZE = 16 * 1024
POWER_OFF_VOLTAGE = 3.40
LOW_POWER_VOLTAGE = 3.55

power_off_count = 0
low_power_count = 0
low_power_flag = False
key_pressed = KEY_UP
power_start_time = 0
usb_connect_state = False
power_indicator_time = 0

def usb_state_process():
    global usb_connect_state
    if(matatalab.is_usb_detected() == True):
        usb_connect_state = True
    elif(usb_connect_state == True):
        usb_connect_state = False
        print("matatalab.reset")
        time.sleep(1)
        matatalab.reset()
    else:
        usb_connect_state = False

def power_indicator():
    global power_indicator_time
    if (time.ticks_diff(time.ticks_ms(), power_indicator_time) > 200):
        battery_capacity = matatalab.get_battery_capacity()
        r = 100
        if(battery_capacity < 10):
            g = 0
            b = 0
        elif(battery_capacity >= 10) and (battery_capacity < 50):
            g = (int)((battery_capacity - 10) * 2.5)
            b = 0
        elif(battery_capacity >= 50) and (battery_capacity < 90):
            g = 100
            b = (int)((battery_capacity - 50) * 2.5)
        else:
            g = 100
            b = 100
        if(matatalab.is_usb_detected() == True):
            r = 0
            g = 0
            b = 0
        power_led[0] = (r,g,b)
        power_led.write()
        power_indicator_time = time.ticks_ms()

def power_monitor():
    global power_off_count, low_power_count, low_power_flag, key_pressed, power_start_time

    key_state = matatalab.get_power_state()

    if (key_pressed == KEY_UP) and (key_state == True):
        key_pressed = KEY_DOWN
        power_start_time = time.ticks_ms()
    elif (key_pressed == KEY_DOWN) and (key_state == False) :
        key_pressed = KEY_UP

    if key_pressed == KEY_DOWN:
        if (time.ticks_diff(time.ticks_ms(), power_start_time) > POWER_OFF_TIME):
            # 长按1.2s开关键关机
            print("long pressed power button, shutdown")
            drv_system.power_off()
            time.sleep(10)

    battery_level = matatalab.get_battery_voltage()
    power_indicator()

    #电压低于 POWER_OFF_VOLTAGE 执行关机
    if(battery_level < POWER_OFF_VOLTAGE):
        power_off_count = power_off_count + 1
        if(power_off_count > 100):
            print("shutdown because of low battery")
            drv_system.power_off()
            time.sleep(10)

    #电压低于 LOW_POWER_VOLTAGE, 低电量告警, 两分钟后关机
    elif(battery_level < LOW_POWER_VOLTAGE):
        power_off_count = 0
        low_power_count = low_power_count + 1
        if (low_power_count > 100):
            low_power_flag = True;

        if ((low_power_flag == True)):
            if(low_power_count > 10000):
                print("Low voltage alarm, shutdown about five minutes")
                drv_system.power_off()
                time.sleep(10)

    #电压正常时，清空异常计数
    elif(battery_level > (LOW_POWER_VOLTAGE + 0.1)):
        power_off_count = 0
        low_power_count = 0
        if(low_power_flag == True):
            low_power_flag = False
            if(communication.ble_state == communication.BLE_LINK):
                matatalab.indicator_led(matatalab.ON)
            else:
                matatalab.indicator_led(matatalab.SINGLE_FLASH)

def main():
    global power_indicator_time
    sensor.get_color_calibration_data()
    event_manager.event_system_start()
    time.sleep(0.1)
    event_manager.event_trigger(event_o.EVE_SYSTEM_LAUNCH)
    power_indicator_time = time.ticks_ms()
    while True:
        ble_state_monitor()
        power_monitor()
        usb_state_process()
        time.sleep(0.02)

def start():
    nvs.init_machine_config()
    nvs.init_calibration_value()
    print("try main.py")
    try:
        if(if_file_exists("main.py") == True):
            print('main.py exists')
            execfile('main.py')
        elif(if_file_exists("default.py") == True):
            print("/default.py exists ")
            execfile('/default.py')
        else:
            print("no main.py or default.py file exists")
    except Exception as e:
        print("ERROR : " + str(e))
    except:
        print("no user code or unknow code error, use default main.py")

if __name__ == '__main__':
    _thread.stack_size(THREAD_MAIN_SIZE)
    _thread.start_new_thread(start, ())

    _thread.stack_size(THREAD_MAIN_SIZE)
    _thread.start_new_thread(main, ())

    _thread.stack_size(THREAD_MAIN_SIZE)
    _thread.start_new_thread(audio.media_process, ())

    _thread.stack_size(THREAD_COMMUNICATION_SIZE)
    _thread.start_new_thread(communication_process, (), 1, 2)

    _thread.stack_size(THREAD_LED_MATRIX_SIZE)
    _thread.start_new_thread(led_matrix_process, (), 1, 2)
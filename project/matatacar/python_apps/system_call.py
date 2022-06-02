import time
import matatalab
import _thread
import neopixel
from communication import communication_process
from communication import ble_state_monitor
import communication
import drv_system
import nvs
import sensor
import audio_play as audio_player
import audio_recorder
import drv_motion as motion

KEY_UP = 0
KEY_DOWN = 1
POWER_OFF_TIME = 1000

THREAD_MAIN_SIZE = 8 * 1024
THREAD_COMMUNICATION_SIZE = 8 * 1024
POWER_OFF_VOLTAGE = 3.40
LOW_POWER_VOLTAGE = 3.55

power_off_count = 0
low_power_count = 0
low_power_flag = False
key_pressed = KEY_UP
power_start_time = 0
usb_connect_state = False

def power_monitor():
    global power_off_count, low_power_count, low_power_flag, key_pressed, power_start_time, usb_connect_state

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
            #matatalab.indicator_led(matatalab.SINGLE_FAST_FLASH)
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
    nvs.init_calibration_value()
    while True:
        ble_state_monitor()
        power_monitor()
        time.sleep(0.02)

def start():
    try:
        print("try main.py")
        uos.stat("/main.py")
        execfile("/main.py")
    except:
        print("no user code, use default main.py")
        _thread.stack_size(THREAD_COMMUNICATION_SIZE)
        _thread.start_new_thread(communication_process, ())
        print("communication_process start\r\n")

if __name__ == '__main__':
    _thread.stack_size(THREAD_MAIN_SIZE)
    _thread.start_new_thread(start, ())

    _thread.stack_size(THREAD_MAIN_SIZE)
    _thread.start_new_thread(main, ())
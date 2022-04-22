import time
import matatalab
import _thread
import audio
import neopixel
import events.event_manager as event_manager
from event_obj import event_o

THREAD_MAIN_SIZE = 8 * 1024

def main():
    event_manager.event_system_start()
    time.sleep(0.1)
    event_manager.event_trigger(event_o.EVE_SYSTEM_LAUNCH)
    while True:
        time.sleep(0.02)

def start():
    try:
        print("try main.py")
        uos.stat("/main.py")
        execfile("/main.py")
    except:
        print("no user code, use default process")

if __name__ == '__main__':
    _thread.stack_size(THREAD_MAIN_SIZE)
    _thread.start_new_thread(start, ())

    _thread.stack_size(THREAD_MAIN_SIZE)
    _thread.start_new_thread(main, ())
import random
import struct
import time
from matatalab import ble
import _thread
import action
import system_state

#from ble_advertising import advertising_payload

from micropython import const
_BLE_IRQ_CONNECT                 = 1
_BLE_IRQ_DISCONNECT              = 2
_BLE_IRQ_READ                    = 3

MSG_FRAMER_HEADER        = 0xFE
MSG_FRAMER_TRANSLATION   = 0xFD
CHAR_TRANSLATION_OFFSET  = 0x20

PROTOCOL_VER_V1 = 1
PROTOCOL_VER_V2 = 2
PROTOCOL_VER_V3 = 3

THREAD_BLE_SIZE = 4*1024

heart_msg = 'car0x87'

def crc16(buffer, crc_init = 0xFFFF):
    crc = crc_init & 0xFFFF
    for byte in buffer:
        crc = (crc>>8 | crc << 8)&0xFFFF

        crc ^= byte&0xFFFF

        crc ^= ((crc&0xFF)>>4)&0xFFFF

        crc ^= ((crc <<8)<<4)&0xFFFF

        crc ^= (((crc&0xFF)<<4)<<1)&0xFFFF
    return (crc & 0xFFFF)

##tx_msg  type list
def message_send(ble, tx_msg):
    msglen = len(tx_msg) + 2
    msg = [MSG_FRAMER_HEADER, msglen]
    msg.extend(tx_msg)
    crc = crc16(msg[1:], 0xFFFF)
    crc_list = [(crc >> 8), (crc & 0xFF)]
    msg.extend(crc_list)
    
    for index in range(1, len(msg)):
        if(msg[index] == MSG_FRAMER_HEADER):
            msg[index] = msg[index] - CHAR_TRANSLATION_OFFSET
            msg.insert(index, MSG_FRAMER_TRANSLATION)
            index = index + 1
        if(msg[index] == MSG_FRAMER_TRANSLATION):
            msg[index] = msg[index] - CHAR_TRANSLATION_OFFSET
            msg.insert(index, MSG_FRAMER_TRANSLATION)
            index = index + 1
    # for i in msg:
    #     print("0x%x"%(i),end=' ')
    # print('')
    msglen = len(msg)
    package = int(msglen // 20)
    for i in range(package):
        payload = msg[(i*20):(i+1)*20]
        ble.send_upstream(bytes(payload))
        time.sleep_ms(50)
    if(msglen%20):
        payload = msg[package*20:]
        ble.send_upstream(bytes(payload))

ble_event = ''
def event_copy(e):
    global ble_event
    ble_event = e
def event_print():
    global ble_event
    print("###IRQ", ble_event)
    
class mpyBLE:
    def __init__(self):
        self._ble = ble()
        self._rdata = b''
        (connect_state, protocol, heart_enable) = self._ble.get_ble_state()
        print("connect_state:%d; protocol:%d; heart_enable:%d"%(connect_state, protocol, heart_enable))
        self.protocol_ver = protocol
        self.connect_state = connect_state
        self.heart_enable = heart_enable

        self.clear_heart_time()
        self._ble.irq(self._irq)

    def _irq(self, event):
        if (event == _BLE_IRQ_CONNECT):
            print("event _BLE_IRQ_CONNECT")
            self.connect_state = True
        elif(event == _BLE_IRQ_DISCONNECT):
            print("event _BLE_IRQ_DISCONNECT")
            self.protocol_ver = PROTOCOL_VER_V1
            self.connect_state = False
        elif(event == _BLE_IRQ_READ):
            self.connect_state = True
            self.recive_data()
        
    def send_upstream(self, data, notify=True):
        self._ble.send_data(data)

    def get_event_state(self):
        return self._ble.get_ble_event()

    def recive_data(self):
        cmd_cancel=[0xFE, 0x03, 0x84, 0x99, 0x50]
        
        recv_data = self._ble.get_data()
        if(recv_data == b''):
            return
        print(recv_data)
        if(self.protocol_ver == PROTOCOL_VER_V2):
            if(recv_data == bytes(cmd_cancel)):
                print("recive cancel command v2")
                system_state.set_cmd_cancel_flag(True)
                self.used_buffer_clear()
                action.action_cancel_cmd()
            else:
                buffer = list(recv_data)
                cnt = buffer.count(MSG_FRAMER_TRANSLATION)
                for i in range(cnt):
                    index = buffer.index(MSG_FRAMER_TRANSLATION)
                    buffer[index+1] = buffer[index+1] + CHAR_TRANSLATION_OFFSET
                    buffer.remove(MSG_FRAMER_TRANSLATION)
                self._rdata += bytes(buffer)
                if self.get_recive_data_len() > 512:
                    print("recived data more then 512")
                    self.used_buffer_clear()
        else:
            if(recv_data[0] == 0x84):
                print("recive cancel command v1")
                system_state.set_cmd_cancel_flag(True)
                if len(recv_data) > 1 and recv_data[1] == 0x85:
                    self._rdata = recv_data[1:]
                else:
                    self.used_buffer_clear()
                    action.action_cancel_cmd()
            else:
                self._rdata += recv_data

    def connect_para_update(self, latency = 0, interval_min = 50, interval_max = 50, timeout = 5000):
        #unit ms
        self._ble.connect_para_update(latency, interval_min, interval_max, timeout)

    def get_recive_data_len(self):
        return len(self._rdata)
        
    def get_data(self):
        return self._rdata
        
    def used_buffer_clear(self, n=0):
        if(n == 0):
            self._rdata = b''
        else:
            self._rdata = self._rdata[n:]

    def get_protocol(self):
        return self.protocol_ver
        
    def set_protocol(self, protocol_ver):
        self.protocol_ver = protocol_ver
        self._ble.set_protocol_version(protocol_ver)
        
    def set_heart_enable(self, enable):
        self.heart_enable = enable
        self._ble.set_heart_enable(enable)

    def get_heart_enable(self):
        return self.heart_enable
        
    def get_connect_state(self):
        (connect_state, protocol, heart_enable) = self._ble.get_ble_state()
        if self.connect_state != connect_state:
            self.connect_state = connect_state
        return self.connect_state

    def send_heart(self):
        if(self.protocol_ver == PROTOCOL_VER_V1):
            heart_msg = "Car:[0x87]\n"
            self.send_upstream(heart_msg, notify=True)
            
        if(self.protocol_ver == PROTOCOL_VER_V2):
            heart_msg = [0x87, 0x01]
            message_send(self, heart_msg)
        
        if(self.protocol_ver == PROTOCOL_VER_V3):
            heart_msg = [0x87, 0x01]
            message_send(self, heart_msg)
    
    def clear_heart_time(self):
        self.heart_time = time.ticks_ms()
        
    def get_heart_time(self):
        return (time.ticks_ms() - self.heart_time)

def ble_get_msg():
    b = matatalab.ble()

    while True:
        msg = b.get_data()
        if(msg != b''):
            print("recv len:",len(msg))
            l = [hex(int(i)) for i in msg]
            print(" ".join(l))
        time.sleep_ms(20)

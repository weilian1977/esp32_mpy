import ble
import protocol_v2
import matatalab
import drv_system
import drv_motion

def protocol_v3_process(carble):
    msglen = carble.get_recive_data_len()
    if(msglen == 0):
        return
    drv_system.clear_idle_time()
    frame_data = protocol_v2.get_msg_frame(carble)
    if(frame_data == None):
        return
    print("frame data:",frame_data)

    ## 0xfe + len(1) + cmd + CRC(2)
    cmd = frame_data[2:-2].decode()
    print(cmd)
    try:
        exec(cmd)
    except:
        print('##%s## isn\'t callable'%cmd)

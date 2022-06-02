from audio import recorder
import time
mRecorder = recorder()
def start(path):
    mRecorder.start(path)
    
def stop():
    mRecorder.stop()
    
def recorder_duration_time(path,recorder_time):
    try:
        print(path,recorder_time)
        mRecorder.start(path, recorder_time)
        i = 0
        while mRecorder.is_running():
            print('Recording:',i)
            i += 1
            time.sleep_ms(1000)
        print('write file to',path)
    except :
        print('Playback the recording error')
        return None
    return None
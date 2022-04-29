import time
import speech
import _thread

def speech_recognition():
    while True:
        speech.speech_recognition()
        cmd_id = False
        cmd_id = speech.is_speech_commands("ni hao")
        if(cmd_id):
            print('----cmd_id test= ni hao---\n',cmd_id)
        time.sleep_ms(10)

_thread.start_new_thread(speech_recognition,())
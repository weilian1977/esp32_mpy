import audio_play
import time
import audio_recorder
import speech



speech.stop_speech_recognition()
def test_recorder_time():
    print("recorder until time")
    audio_recorder.recorder_duration_time("/music_recorder/1.AMR",10)
    time.sleep_ms(1000)
    audio_play.play("/music_recorder/1.AMR",8000, 50,True)


def test_recorder():
    print("recorder start and stop")
    time.sleep_ms(1000)
    audio_recorder.start("/music_recorder/2.AMR")
    time.sleep_ms(10000)
    audio_recorder.stop()
    time.sleep_ms(1000)
    audio_play.play("/music_recorder/2.AMR",8000, 50,True)

test_recorder_time()

test_recorder()

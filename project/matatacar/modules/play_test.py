import motion_sensor
import audio_play
import time
import speech
import _thread
import audio_recorder

speech.stop_speech_recognition()

def test_play_alto():
    print("test play_alto")
    audio_play.play_alto(1)
    audio_play.play_alto(2)
    audio_play.play_alto(3)
    audio_play.play_alto(4)
    audio_play.play_alto(5)
    audio_play.play_alto(6)
    audio_play.play_alto(7)

def test_play_treble():
    print("test play_treble")
    audio_play.play_treble(1)
    audio_play.play_treble(2)
    audio_play.play_treble(3)
    audio_play.play_treble(4)
    audio_play.play_treble(5)
    audio_play.play_treble(6)
    audio_play.play_treble(7)


def test_play_all_tone():
    print("test play_all_tone")
    for i in range(1,22):
        audio_play.set_instrument(str(i))
        audio_play.set_tempo(300)
        audio_play.play_tone(60,1)
        audio_play.play_tone(62,1)
        audio_play.play_tone(64,1)
        audio_play.play_tone(65,1)
        audio_play.play_tone(67,1)
        audio_play.play_tone(69,1)
        audio_play.play_tone(71,1)

    

def test_say_chinese_pause_and_resume():
    print("play say chinese pause and resume")
    audio_play.play_say("chinese","欢迎使用乐鑫语音合成")
    time.sleep_ms(2000)
    audio_play.play_pause()
    time.sleep_ms(4000)
    audio_play.play_resume()
    time.sleep_ms(5000)


def test_play_say_chines():
    print("play say chinese")
    audio_play.play_say("chinese","你好,我是乐鑫语音合成助手")

def test_play_say_english():
    print("play say english")
    audio_play.play_say("english","hello,I am a little robot")

def test_play_file_music():
    print("play file music")
    audio_play.play("game_start.mp3")

def test_play_time():
    print("play play_time")
    audio_play.play_time("game_start.mp3",5000)

def test_play_until_done():
    print("play play_until_done")
    audio_play.play_until_done("game_start.mp3")


test_play_alto()

test_play_treble()

test_play_all_tone()

test_say_chinese_pause_and_resume()

test_play_say_chines()

test_play_say_english()

test_play_file_music()

test_play_time()

test_play_until_done()
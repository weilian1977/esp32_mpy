from _audio import player,recorder
import time
import speech
from utility import if_file_exists
import system_state
import _thread
lock = _thread.allocate_lock()

mRecorder = recorder()
mPlayer = player()

record_time = 0
record_path = None
play_path = None
play_rate = 48000
play_sync = True
play_time = 0
play_the_end_path = 0

resample_rate =22050.0
tempo = 2000.0
beats = 1
volume_set = 70
say_language = 'english'
instrument_type = '1-piano'

instruments_voice_table = {
'1-piano':[24, 36, 48, 60, 72, 84, 96, 108],
'2-electric-piano':[60],
'3-organ':[60],
'4-guitar':[60],
'5-electric-guitar':[60],
'6-bass':[36, 48],
'7-pizzicato':[60],
'8-cello':[36, 48, 60],
'9-trombone':[36, 48, 60],
'10-clarinet':[48, 60],
'11-saxophone':[36, 60, 84],
'12-flute':[60, 72],
'13-wooden-flute':[60, 72],
'14-bassoon':[36, 48, 60],
'15-choir':[ 48, 60, 72],
'16-vibraphone':[60, 72],
'17-music-box':[60],
'18-steel-drum':[60],
'19-marimba':[60],
'20-synth-lead':[60],
'21-synth-pad':[60],
}


note_offset = {
1:0,
2:2,
3:4,
4:5,
5:7,
6:9,
7:11,
}

music_dir_table = {
    'system': ['1-off.mp3', '2-on.mp3', '3-start.mp3', '4-ulink.mp3', '5-link.mp3'],
    'move'  : ['1-hi.mp3', '2-why.mp3', '3-dila.mp3', '4-zzz.mp3', '5-byebye.mp3', '6-woo.mp3', '7-applaud.mp3', '8-yoho.mp3', '9-aaa.mp3', '10-ao.mp3', '11-o-no.mp3', '12-du.mp3', '13-hello.mp3', '14-byebye.mp3', '15-waw.mp3'],
    'melody': ['1-melody.mp3', '2-melody.mp3', '3-melody.mp3', '4-melody.mp3', '5-melody.mp3', '6-melody.mp3', '7-melody.mp3', '8-melody.mp3', '9-melody.mp3', '10-melody.mp3'],
    'dance' : ['1-dance.mp3', '2-dance.mp3', '3-dance.mp3', '4-dance.mp3', '5-dance.mp3', '6-dance.mp3'],
    'drums' : ['1-snare.mp3', '2-bass-drum.mp3', '3-side-stick.mp3', '4-crash-cymbal.mp3', '5-open-hi-hat.mp3', '6-closed-hi-hat.mp3', '7-tambourine.mp3','8-hand-clap.mp3', '9-claves.mp3','10-wood-block.mp3','11-cowbell.mp3',\
               '12-triangle.mp3', '13-bongo.mp3', '14-conga.mp3', '15-cabasa.mp3', '16-guiro.mp3', '17-vibraslap.mp3', '18-cuica.mp3' ],
    'effect': ['1-fire engine bell.mp3', '2-police car bell.mp3', '3-ambulance bell.mp3', '4-thunder.mp3', '5-rain.mp3', '6-strong wind.mp3','7-cicada chirping.mp3','8-frog call.mp3','9-elephant.mp3','10-kitten.mp3',\
               '11-duck.mp3','12-hen.mp3','13-sheep.mp3','14-cow.mp3','15-dog.mp3','16-pig.mp3','17-cow.mp3','18-hourse.mp3','19-knock on door.mp3','20-ding ding.mp3','21-wrong.mp3','22-score.mp3','23-get coins.mp3',\
               '24-warning.mp3','25-upgrade.mp3','26-fail.mp3','27-metal knock.mp3','28-water flow.mp3','29-record sound.mp3'],
    'sing'  : ['1-twinkle, twinkle, little star.mp3', '2-jingle bells.mp3', '3-happy birthday to you.mp3', '4-to alice.mp3', '5-ode to joy.mp3', '6-parade of the wooden soldiers.mp3', '7-peter and the wolf.mp3', '8-hunting porca.mp3', '9-die forelle.mp3'],
}

## player

def rates(rate):
    if(rate > 480000):
        rate = 480000
    if(rate < 4800):
        rate = 4800
    mPlayer.rates(rate)

def set_tempo(pct):
    global tempo
    if(pct > 4000):
        pct = 4000
    if(pct < 100):
        pct = 100
    tempo = pct

def get_tempo():
    global tempo
    return tempo

def set_instrument(instrument_type_select):
    global instrument_type
    if instrument_type_select in instruments_voice_table:
        instrument_type = instrument_type_select
        return None
    files = instruments_voice_table.keys()
    for f in files:
        if f.find('-') >= 0:
            fsp = f.split('-', 1)
            if fsp[0] == instrument_type_select:
                instrument_type = f
                return None

def set_play_info(path, sync = False, s_time = 0,rate_info = '', speed_info = 1.0, pitch_info = 1.0):
    global play_path, play_rate, play_sync, play_time
    if(path == None):
        return
    global play_the_end_path
    play_the_end_path = play_the_end_path + 1
    play_the_end_path_flag = play_the_end_path
    lock.acquire(True, 20)
    time.sleep(0.02)
    #print("global = ",play_the_end_path,"local = ",play_the_end_path_flag)
    if(play_the_end_path > play_the_end_path_flag):
        if lock.locked():
            lock.release()
            return
    speech_recognition_stop()
    mPlayer.stop()
    
    while(True):
        if(system_state.get_play_state() == system_state.PLAY_IDLE):
            play_path = path
            play_sync = sync
            play_time = s_time
            system_state.set_audio_stop_flag(False)
            if(rate_info != ''):
                mPlayer.rates(rate_info)
            mPlayer.say_speed(speed_info)
            mPlayer.say_pitch(pitch_info)
            print("play_path = ",play_path)
            system_state.set_play_state(system_state.PLAY_RUN)
            break
        else:
            time.sleep(0.02)
    if lock.locked():
            lock.release()
            play_the_end_path = 0
    if(play_sync == True):
        play_state = system_state.get_play_state()
        while (play_state == system_state.PLAY_RUN):
            play_state = system_state.get_play_state()
            if(play_state == system_state.PLAY_IDLE):
                time.sleep(0.02)
                break
            else:
                time.sleep(0.02) 

def play(path, sync = False, play_time = 0, speed = 1.0, pitch =1.0, rate = ''):
    type = "file:/"
    if(if_file_exists(path) == False):
        print('file no exists')
        path = '/sdcard/ding.mp3'
    play_path = "%s%s" % (type,path)
    set_play_info(play_path,sync,play_time,rate,speed,pitch)
    
def play_times(path, play_time = 0):
    type = "file:/"
    if(if_file_exists(path) == False):
        print('file no exists')
        path = '/sdcard/ding.mp3'
    play_path = "%s%s" % (type,path)
    set_play_info(play_path,True,play_time)
    
def play_until_done(path):
    type = "file:/"
    if(if_file_exists(path) == False):
        print('file no exists')
        path = '/sdcard/ding.mp3'
    play_path = "%s%s" % (type,path)
    set_play_info(play_path,True,0)

def play_say(language,text,sync = True):
    speech_recognition_stop()
    if(language == "english"):
        type = "sam://"
        rates(22050)
    elif(language == "chinese"):
        rates(16000)
        type = "hans://"
    else:
        print("language error")
        return
    end_text = "/.wav"
    play_path = "%s%s%s" % (type,text,end_text)
    set_play_info(play_path,sync)

def set_say_language(language):
    global say_language
    if(language != "english" and language != "chinese"):
        print("language error")
        return
    say_language = language

def get_say_language(language):
    global say_language
    say_language = language

def say(text,sync = False):
    global say_language
    play_say(say_language,text,sync)

def play_pause():
    mPlayer.pause()

def play_resume():
    mPlayer.resume()

def play_stop():
    mPlayer.stop()
    system_state.set_play_state(system_state.PLAY_IDLE)

def set_volume(value):
    global volume_set
    temp = value
    if(temp > 100):
        temp = 100
    if(temp < 0):
        temp = 0
    volume_set = value
    mPlayer.set_vol(temp)
    print("current volume is ",temp)
    
def get_volume():
    temp = mPlayer.get_vol()
    if(temp > 100):
        temp = 100
    print("current volume is ",temp)
    return temp

def add_volume(add_value = 5):
    global volume_set
    temp = volume_set + add_value
    if(temp > 100):
        temp = 100
    mPlayer.set_vol(temp)
    print("current volume is ",temp)

def cut_volume(cut_value = 5):
    global volume_set
    temp = volume_set - cut_value
    if(temp < 0):
        temp = 0
    mPlayer.set_vol(temp)
    print("current volume is ",temp)

def play_instrument_tone(instruments, tone):
    global instruments_voice_table
    voice_file = instruments_voice_table[instruments]
    find_tone = voice_file[0]
    for f in voice_file[::-1]:
        if(f <= tone):
            find_tone = f
            break
    path = "%s%s%s%s%s" % ('/sdcard/music/instruments/',instruments,'/',str(find_tone),'.mp3')
    pitch_interval = pow(2, (tone - find_tone) / 12)
    set_rate =resample_rate * pitch_interval
    print("find tone: %d, set_rate:%d" %(find_tone, set_rate))
    rates(round(set_rate))
    speech_recognition_stop()
    record_stop()
    type = "file:/"
    play_path = "%s%s" % (type,path)
    system_state.set_play_state(system_state.PLAY_END)
    mPlayer.play(play_path, 0, False , 0)
    #set_play_info(play_path, False)

def play_tone(tone, meter, instruments = ''):
    global play_rate,tempo,beats,volume_set
    beats = meter
    time_start = time.ticks_ms()
    mPlayer.set_vol(volume_set + 10)
    if(instruments == ''):
        instruments = instrument_type
        play_instrument_tone(instruments, tone)
    else:
        set_instrument(instruments)
        play_instrument_tone(instrument_type, tone)

    time_space = time.ticks_diff(time.ticks_ms(), time_start)
    time_space=tempo * beats - time_space
    if(time_space < 0):
        time_space = 0
    time.sleep(time_space / 1000)
    play_stop()
    mPlayer.set_vol(volume_set)

def play_alto(tone_id, beat = 1):
    offset = note_offset.get(tone_id, 0)
    play_tone((offset + 60), beat)

def play_treble(tone_id, beat = 1):
    offset = note_offset.get(tone_id, 0)
    play_tone((offset + 72), beat)

def get_tone_offset(tone_id):
    return note_offset.get(tone_id, 0)

def dir_find(path, name):
    files = music_dir_table.get(path, [])
    full_path = '%s%s' %('/sdcard/music/', path)
    name = str(name)
    print("path:%s, file:%s" %(full_path, name))
    for f in files:
        if f.find('-') >= 0:
            fsp = f.split('-', 1)
            fstart = fsp[1].find(name)
            if fsp[0] == name:
                file_full_path = '%s%c%s' %(full_path, '/', f)
                return file_full_path
            elif (fstart >= 0) and (fstart < (len(fsp[1])/2)):
                file_full_path = '%s%c%s' %(full_path, '/', f)
                return file_full_path
    for f in files:
        if (f.find(name) >= 0):
            file_full_path = '%s%c%s' %(full_path, '/', f)
            return file_full_path
    return None

def play_move(name, sync = True, play_time = 0):
    path = dir_find('move', name)
    play(path, sync, play_time)

def play_music(name, sync = True, play_time = 0):
    path = dir_find('sing', name)
    play(path, sync, play_time)

def play_dance(name, sync = True, play_time = 0):
    path = dir_find('dance', name)
    play(path, sync, play_time)

def play_system(name, sync = True, play_time = 0):
    path = dir_find('system', name)
    play(path, sync, play_time)

def play_melody(name, sync = True, play_time = 0):
    path = dir_find('melody', name)
    play(path, sync, play_time)

def rest(meter):
    global beats
    beats = meter

def play_drums(name, meter):
    global tempo,beats
    beats = meter
    path = dir_find('drums', name)
    play(path, False)
    time_start = time.ticks_ms()
    
    time_space = time.ticks_diff(time.ticks_ms(), time_start)
    time_space=tempo * beats - time_space
    if(time_space < 0):
        time_space = 0
    time.sleep(time_space / 1000)
    play_stop()

def effect(name, sync = False , play_time = 0):
    path = dir_find('effect', name)
    play(path, sync, play_time)
    
def sing(name, sync = False , play_time = 0):
    path = dir_find('sing', name)
    play(path, sync, play_time)
    
## recorder
def record_start(path):
    global record_time,record_path
    play_stop()
    speech_recognition_stop()
    record_time = 0
    record_path = path
    system_state.set_record_state(system_state.RECORD_RUN)
    
def record_stop():
    mRecorder.stop()
    system_state.set_record_state(system_state.RECORD_IDLE)
    
def record_duration_time(path,rec_time):
    global record_time,record_path
    try:
        play_stop()
        speech_recognition_stop()
        print(path,rec_time)
        record_path = path
        record_time = rec_time
        system_state.set_record_state(system_state.RECORD_RUN)
        while (1):
            record_state = system_state.get_record_state()
            if(record_state == system_state.RECORD_IDLE):
                break
            else:
                time.sleep(0.02)
    except :
        print('Playback the recording error')
        return None
    return None

## speech
def speech_recognition_start():
    if(system_state.get_speech_state()==system_state.SPEECH_IDLE):
        system_state.set_speech_state(system_state.SPEECH_RUN)
        
def speech_recognition_stop():
    speech.stop_speech_recognition()
    system_state.set_speech_state(system_state.SPEECH_END)

def speech_recognition_change_commands(commands):
    speech.change_commid(commands)

def is_speech_recognition_command(command):
    return speech.is_speech_commands(command)
    
## audio manager
def media_process_running():
    global play_path, play_rate, play_sync, play_time
    while True:
        record_state = system_state.get_record_state()
        play_state = system_state.get_play_state()
        speech_state = system_state.get_speech_state()

        if(play_state == system_state.PLAY_END):
            if(mPlayer.get_state() == False):
                play_stop()
            else:
                time.sleep(0.02)
        elif (record_state == system_state.RECORD_END):
            time.sleep(0.02)
        elif(record_state == system_state.RECORD_RUN) :
            global record_time,record_path
            if (record_time > 0):
                mRecorder.start(record_path,record_time)
                record_time = 0
                system_state.set_record_state(system_state.RECORD_IDLE)
            else:
                mRecorder.start(record_path)
                system_state.set_record_state(system_state.RECORD_END)
        elif (play_state==system_state.PLAY_RUN):
            if(system_state.get_audio_stop_flag() == True):
                system_state.set_play_state(system_state.PLAY_IDLE)
            else:
                mPlayer.play(play_path, 0, play_sync , play_time)
                if(play_sync ==True):
                    system_state.set_play_state(system_state.PLAY_IDLE)
                else:
                    system_state.set_play_state(system_state.PLAY_END)
        elif(speech_state == system_state.SPEECH_RUN):
            speech.start_speech_recognition()
            system_state.set_speech_state(system_state.SPEECH_END)
        elif(speech_state == system_state.SPEECH_END) and (play_state == system_state.PLAY_IDLE) and (record_state == system_state.RECORD_IDLE):
            system_state.set_speech_state(system_state.SPEECH_IDLE)
        else:
            time.sleep(0.02)

def media_process():
    while True:
        try:
            media_process_running()
        except:
            print("media_process catch error")




from audio import player
import time
mPlayer = player()

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
    'music': ['1-music.mp3', '2-music.mp3', '3-music.mp3', '4-music.mp3', '5-music.mp3', '6-music.mp3'],
    'move': ['1-hi.mp3', '2-why.mp3', '3-dila.mp3', '4-zzz.mp3', '5-byebye.mp3', '6-woo.mp3', '7-applaud.mp3', '8-yoho.mp3', '9-aaa.mp3', '10-ao.mp3', '11-o-no.mp3', '12-du.mp3', '13-hello.mp3', '14-byebye.mp3', '15-waw.mp3'],
    'melody': ['1-melody.mp3', '2-melody.mp3', '3-melody.mp3', '4-melody.mp3', '5-melody.mp3', '6-melody.mp3', '7-melody.mp3', '8-melody.mp3', '9-melody.mp3', '10-melody.mp3'],
    'dance': ['1-dance.mp3', '2-dance.mp3', '3-dance.mp3', '4-dance.mp3', '5-dance.mp3', '6-dance.mp3'],
}


resample_rate =22050.0
tempo = 500.0
volume_set = 70
global instrument_type
instrument_type = '1-piano'

def rates(rate):
    if(rate > 480000):
        rate = 480000
    if(rate < 4800):
        rate = 4800
    mPlayer.rates(rate)

def set_tempo(pct):
    if(pct > 4000):
        pct = 4000
    if(pct < 100):
        pct = 100
    tempo = pct

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

def set_play_info(volume_info,rate_info = '', speed_info = 1.0, pitch_info = 1.0,):
    if(rate_info != ''):
        mPlayer.rates(rate_info)
    mPlayer.say_speed(speed_info)
    mPlayer.say_pitch(pitch_info)
    mPlayer.set_vol(volume_info)
    #volume_set = volume_info
            
def play(path, volume = '', sync = True, play_time = 0, speed = 1.0, pitch =1.0, rate = ''):
    global volume_set
    tpye = "file:/"
    play_path = "%s%s" % (tpye,path)
    if(volume !=''):
        #volume = volume_set
        volume_set = volume
    set_play_info(volume_set,rate,speed,pitch)
    mPlayer.play(play_path, 0, sync, play_time)
    
def play_time(path, play_time = 0):
    global volume_set
    tpye = "file:/"
    play_path = "%s%s" % (tpye,path)
    set_play_info(volume_set)
    mPlayer.play(play_path, 0, True, play_time)
    
def play_until_done(path):
    global volume_set
    tpye = "file:/"
    play_path = "%s%s" % (tpye,path)
    set_play_info(volume_set)
    mPlayer.play(play_path, 0, sync = True, time = 0)
    
def play_say(language,text):
    if(language == "english"):
        tpye = "sam://"
        rates(22050)
    elif(language == "chinese"):
        rates(16000)
        tpye = "hans://"
    else:
        print("language error")
        return
    end_text = "/.wav"
    play_path = "%s%s%s" % (tpye,text,end_text)
    mPlayer.play(play_path, pos = 0, sync = True, time = 0)


def play_pause():
    
    mPlayer.pause()
    
    
def play_resume():
    
    mPlayer.resume()
    
    
def play_stop():
    mPlayer.stop()

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
    global instruments_voice_table, pre_rate , volume_set
    voice_file = instruments_voice_table[instruments]
    find_tone = voice_file[0]
    for f in voice_file[::-1]:
        if(f <= tone):
            find_tone = f
            break

    path = "%s%s%s%s%s" % ('/music/instruments/',instruments,'/',str(find_tone),'.mp3')
    
    pitch_interval = pow(2, (tone - find_tone) / 12)
    set_rate =resample_rate * pitch_interval
    print("find tone: %d, set_rate:%d" %(find_tone, set_rate))
    rates(round(set_rate))
    play(path, volume = volume_set,sync =False)

def play_tone(tone, meter, instruments = ''):
    global play_rate
    time_start = time.ticks_ms()
    if(instruments == ''):
        instruments = instrument_type
        play_instrument_tone(instruments, tone)
    else:
        set_instrument(instruments)
        play_instrument_tone(instrument_type, tone)

    time_space = time.ticks_diff(time.ticks_ms(), time_start)

    time_space=tempo * meter - time_space
    if(time_space < 0):
        time_space = 0
    time.sleep(time_space / 1000)
    play_stop()


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
    name = str(name)
    for f in files:
        if f.find('-') >= 0:
            fsp = f.split('-', 1)
            fstart = fsp[1].find(name)
            if fsp[0] == name:
                full_path = "%s%s%s%s" % ('/music/',path,'/',f)
                return full_path
            elif (fstart >= 0) and (fstart < (len(fsp[1])/2)):
                full_path = "%s%s%s%s" % ('/music/',path,'/',f)
                return full_path
    for f in files:
        if (f.find(name) >= 0):
            full_path = "%s%s%s%s" % ('/music/',path,'/',f)
            return full_path
    return None

def play_melody(name, sync = True, play_time = 0):
    global volume_set
    path = dir_find('melody', name)
    play(path, volume_set, sync, play_time)

def say_info(mouth,throat,speed_info,pitch_info):
    mPlayer.say_speed(speed_info)
    mPlayer.say_pitch(pitch_info)
    mPlayer.set_mouth_throat(mouth,throat)
    
def play_test(path):
    tpye = "file:/"
    play_path = "%s%s" % (tpye,path)
    mPlayer.play(play_path, 0, True, 0)


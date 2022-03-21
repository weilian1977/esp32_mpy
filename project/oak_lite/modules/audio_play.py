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

resample_rate =22050.0
tempo = 500.0
note_offset = {
1:0,
2:2,
3:4,
4:5,
5:7,
6:9,
7:11,
}


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

def set_play_info(rate_info = 48000, speed_info = 1.0, pitch_info =1.0, volume_info = 60):
    mPlayer.rates(rate_info)
    mPlayer.say_speed(speed_info)
    mPlayer.say_pitch(pitch_info)
    mPlayer.set_vol(volume_info)
            
def play(path, rate = 48000, volume = 60, sync = True, play_time = 0, speed = 1.0, pitch =1.0):
    tpye = "file:/"
    play_path = "%s%s" % (tpye,path)
    set_play_info(rate,speed,pitch,volume)
    mPlayer.play(play_path, 0, sync, play_time)
    
def play_time(path, play_time = 0):
    tpye = "file:/"
    play_path = "%s%s" % (tpye,path)
    set_play_info()
    mPlayer.play(play_path, 0, True, play_time)
    
def play_until_done(path):
    tpye = "file:/"
    play_path = "%s%s" % (tpye,path)
    set_play_info()
    mPlayer.play(play_path, 0, sync = True, time = 0)
    
def play_say(language,text):
    if(language == "english"):
        tpye = "sam://"
        set_play_info(20500)
    elif(language == "chinese"):
        
        set_play_info(16000)
        tpye = "hans://"
    else:
        print("language error")
        return
    end_text = "/.wav"
    play_path = "%s%s%s" % (tpye,text,end_text)
    mPlayer.play(play_path, pos = 0, sync = False, time = 0)
    

def play_pause():
    
    mPlayer.pause()
    
    
def play_resume():
    
    mPlayer.resume()
    
    
def play_stop():
    mPlayer.stop()
    
def add_vol():
    temp = mPlayer.get_vol()
    temp += 5
    if(temp > 100):
        temp = 100
    mPlayer.set_vol(temp)
    
def cut_vol():
    temp = mPlayer.get_vol()
    temp -= 5
    if(temp < 0):
        temp = 0
    mPlayer.set_vol(temp)

    
def play_instrument_tone(instruments, tone):
    global instruments_voice_table,  pre_rate
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
    play(path, round(set_rate), volume = 60,sync =False)

def play_tone(tone, meter, instruments = ''):
    global play_rate
    time_start = time.ticks_ms()
    if(instruments == ''):
        instruments = instrument_type
    play_instrument_tone(instruments, tone)

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





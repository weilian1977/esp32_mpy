from audio import player

mPlayer = player()


def set_play_info(rate_info = 48000, speed_info = 1, pitch_info =1, volume_info = 60):
    mPlayer.rates(rate_info)
    mPlayer.say_speed(speed_info)
    mPlayer.say_pitch(pitch_info)
    mPlayer.set_vol(volume_info)
            
def play(path, rate = 48000, volume = 60, sync = True, play_time = 0, speed = 1, pitch =1):
    tpye = "file:/"
    play_path = "%s%s" % (tpye,path)
    set_play_info(rate,speed,pitch,volume)
    mPlayer.play(play_path, 0, sync, play_time)
    
def playback_time(path, play_time = 0):
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











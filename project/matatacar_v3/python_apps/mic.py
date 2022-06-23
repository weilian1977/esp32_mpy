from _audio import i2s_mic

i2c_mic_sound = i2s_mic()
def get_loudness(value_type = "maximum"):
    if (value_type != "maximum") and (value_type != "average"):
        return 0
    return i2c_mic_sound.get_loudness(value_type)
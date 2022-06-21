import time
import matatalab
import nvs

color_red = 0
color_green = 0
color_blue = 0
color_bri_ratio = 0

_sensor = matatalab.sensor()

def get_left_light():
    return _sensor.get_light_value(1);

def get_right_light():
    return _sensor.get_light_value(0);

def get_obstacle_avoidance_value():
    return _sensor.get_obstacle_avoidance_value();

def get_left_infrared_tube():
    return _sensor.get_line_follower_2_value();

def get_right_infrared_tube():
    return _sensor.get_line_follower_1_value();

def get_infrared_tube():
    left_value = get_left_infrared_tube()
    right_value = get_right_infrared_tube()
    if left_value < 0.4:
        left_value = 0x01
    else:
        left_value = 0x00
    if right_value < 0.4:
        right_value = 0x01
    else:
        right_value = 0x00
    return ((right_value << 1) | left_value)

def get_color_raw():
    return _sensor.get_color()

def get_color_offset():
    r, g, b = _sensor.get_color()
    r, g, b = r - 220, g - 160, b - 160
    if(r < 0):
        r = 0
    if(g < 0):
        g = 0
    if(b < 0):
        b = 0
    return r,g,b

def max_data(a, b, c):
    max_value = 0
    if a > b:
        if a > c:
            max_value = a
        else:
            max_value = c
    else:
        if b > c:
            max_value = b
        else:
            max_value = c
    return max_value

def set_color_calibration_data(red, green, blue, bri_ratio):
    global color_red, color_green, color_blue, color_bri_ratio
    nvs.write('color_red', str(red))
    nvs.write('color_green', str(green))
    nvs.write('color_blue', str(blue))
    nvs.write('bri_ratio', str(bri_ratio))
    color_red = red;
    color_green = green;
    color_blue = blue;
    color_bri_ratio = bri_ratio;

def color_calibration():
    red_sum = 0
    green_sum = 0
    blue_sum = 0
    cali_count = 5
    for i in range(cali_count):
        r,g,b = get_color_offset()
        red_sum = red_sum + r
        green_sum = green_sum + g
        blue_sum = blue_sum + b
        time.sleep_ms(40)
    red_sum = red_sum / cali_count
    green_sum = green_sum / cali_count
    blue_sum = blue_sum / cali_count

    if((red_sum != 0) and (green_sum != 0) and (blue_sum != 0)):
        max_num = max(red_sum, green_sum, blue_sum)
        red_value = int((max_num * 1000.0) / red_sum)
        green_value = int((max_num * 1000.0) / green_sum)
        blue_value = int((max_num * 1000.0) / blue_sum)
        bri_value = int(max_num * 1000.0 / 255)
        set_color_calibration_data(red_value, green_value, blue_value, bri_value)

def get_color_calibration_data():
    global color_red, color_green, color_blue, color_bri_ratio
    color_red = nvs.read('color_red')
    need_write = False
    if color_red is None:
        color_red = 1000
        need_write = True
    else:
        color_red = int(color_red)

    color_green = nvs.read('color_green')
    if color_green is None:
        color_green = 1000
        need_write = True
    else:
        color_green = int(color_green)

    color_blue = nvs.read('color_blue')
    if color_blue is None:
        color_blue = 1000
        need_write = True
    else:
        color_blue = int(color_blue)

    color_bri_ratio = nvs.read('bri_ratio')
    if color_bri_ratio is None:
        color_bri_ratio = 1000
        need_write = True
    else:
        color_bri_ratio = int(color_bri_ratio)
    if need_write is True:
        set_color_calibration_data(color_red, color_green, color_blue, color_bri_ratio)
    return color_red, color_green, color_blue, color_bri_ratio

def get_real_time_color_calibration_data():
    global color_red, color_green, color_blue, color_bri_ratio
    return color_red, color_green, color_blue, color_bri_ratio

def get_color():
    global color_red, color_green, color_blue, color_bri_ratio
    r, g, b = get_color_offset()
    red_value = int((r * color_red * 1.0) / color_bri_ratio)
    green_value = int((g * color_green * 1.0) / color_bri_ratio)
    blue_value = int((b * color_blue * 1.0) / color_bri_ratio)
    return red_value, green_value, blue_value

def rgb2hsv(r, g, b):
    max_rgb = max(r, g, b)
    r, g, b = r * 1.0 / max_rgb, g * 1.0 / max_rgb, b * 1.0 / max_rgb
    mx = max(r, g, b)
    mn = min(r, g, b)
    m = mx - mn
    if mx == mn:
        h_value = 0
    elif mx == r:
        if g >= b:
            h_value = ((g-b)/m) * 60
        else:
            h_value = ((g-b)/m) * 60 + 360
    elif mx == g:
        h_value = ((b-r)/m) * 60 + 120
    elif mx == b:
        h_value = ((r-g)/m) * 60 + 240

    if mx == 0:
        s_value = 0
    else:
        s_value = m/mx
    v_value = mx

    H = int(h_value / 2)
    S = int(s_value * 255.0)

    if(max_rgb > 255):
        max_rgb = 255
    V = max_rgb

    return H, S, V

def get_color():
    global color_red, color_green, color_blue, color_bri_ratio
    r, g, b = get_color_offset()
    red_value = int((r * color_red * 1.0) / color_bri_ratio)
    green_value = int((g * color_green * 1.0) / color_bri_ratio)
    blue_value = int((b * color_blue * 1.0) / color_bri_ratio)
    return red_value, green_value, blue_value

def get_hsv():
    r, g, b = get_color()
    if((r == 0) and (g == 0) and (b == 0)):
        h,s,v = 0,0,0
    else:
        h, s, v = rgb2hsv(r, g, b)
    return h,s,v

def get_color_id():
    r, g, b = get_color()
    if((r == 0) and (g == 0) and (b == 0)):
        return "black"
    else:
        h, s, v = rgb2hsv(r, g, b)

        if(v > 220) and (s < 30) and (h < 180):
            return "white"
        elif (v < 46) and (s < 255) and (h < 180):
            return "black"
        elif ((v > 46) and v < 220) and (s < 43) and (h < 180):
            return "grey"
        elif (v > 46) and (s > 43) and ((h < 10) or ((h > 156) and (h < 180))):
            return "red"
        elif (v > 46) and (s > 43) and ((h > 11) and (h < 25)):
            return "orange"
        elif (v > 46) and (s > 43) and ((h > 26) and (h < 34)):
            return "yellow"
        elif (v > 46) and (s > 43) and ((h > 35) and (h < 77)):
            return "green"
        elif (v > 46) and (s > 43) and ((h > 78) and (h < 99)):
            return "cyan"
        elif (v > 46) and (s > 43) and ((h > 100) and (h < 124)):
            return "blue"
        elif (v > 46) and (s > 43) and ((h > 125) and (h < 155)):
            return "purple"
        else:
            return "unknown"

def send_ir_code(addr, command):
    _sensor.send_ir_code(addr, command)

def get_ir_code():
    return _sensor.get_ir_code()


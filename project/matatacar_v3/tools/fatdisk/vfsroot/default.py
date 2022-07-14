import sensor, time, draw

mode = 0
line_following_mode = 0
draw_mode = 0
draw_select = 0

display_drawing_led_matrix_flag = 0

def display_drawing_led_matrix():
    display_drawing_delay = 0.5
    print("display_drawing_led_matrix!\n")
    display_drawing_led_matrix_flag = 1
    while True:
        if((draw_mode == 1)|(mode !=2)|(draw_select !=0)):
            print("break!\n")
            break
        led_matrix.clear()
        for i in range(2, 16):
            led_matrix.set_pixel(i, 0, 100)
            led_matrix.set_pixel(i, 1, 100)
            led_matrix.set_pixel(i, 2, 100)

            if(i > 0):
                led_matrix.set_pixel(i-1, 0, 100)
                led_matrix.set_pixel(i-1, 1, 0)
                led_matrix.set_pixel(i-1, 2, 100)

            if(i > 1):
                led_matrix.set_pixel(i-2, 0, 100)
                led_matrix.set_pixel(i-2, 1, 100)
                led_matrix.set_pixel(i-2, 2, 100)

            if(i > 2):
                led_matrix.set_pixel(i-3, 0, 0)
                led_matrix.set_pixel(i-3, 1, 100)
                led_matrix.set_pixel(i-3, 2, 0)
            if((draw_mode == 1)|(mode !=2)|(draw_select !=0)):
                break
            time.sleep(display_drawing_delay)

        for i in range(3, 8):
            led_matrix.set_pixel(13, i, 100)
            led_matrix.set_pixel(14, i, 100)
            led_matrix.set_pixel(15, i, 100)

            led_matrix.set_pixel(13, i-1, 100)
            led_matrix.set_pixel(14, i-1, 0)
            led_matrix.set_pixel(15, i-1, 100)

            led_matrix.set_pixel(13, i-2, 100)
            led_matrix.set_pixel(14, i-2, 100)
            led_matrix.set_pixel(15, i-2, 100)

            if(i == 3):
                led_matrix.set_pixel(13, i-3, 0)
                led_matrix.set_pixel(14, i-3, 0)
                led_matrix.set_pixel(15, i-3, 0)

            if(i == 4):
                led_matrix.set_pixel(15, i-3, 0)

            if(i > 4):
                led_matrix.set_pixel(13, i-3, 0)
                led_matrix.set_pixel(14, i-3, 100)
                led_matrix.set_pixel(15, i-3, 0)
            if((draw_mode == 1)|(mode !=2)|(draw_select !=0)):
                break
            time.sleep(display_drawing_delay)

        for i in range(12, -1, -1):
            led_matrix.set_pixel(i, 5, 100)
            led_matrix.set_pixel(i, 6, 100)
            led_matrix.set_pixel(i, 7, 100)

            if(i < 13):
                led_matrix.set_pixel(i+1, 5, 100)
                led_matrix.set_pixel(i+1, 6, 0)
                led_matrix.set_pixel(i+1, 7, 100)

            if(i < 13):
                led_matrix.set_pixel(i+2, 5, 100)
                led_matrix.set_pixel(i+2, 6, 100)
                led_matrix.set_pixel(i+2, 7, 100)

            if(i == 12):
                led_matrix.set_pixel(i+3, 5, 0)
                led_matrix.set_pixel(i+3, 6, 0)
                led_matrix.set_pixel(i+3, 7, 0)

            if(i == 11):
                led_matrix.set_pixel(i+3, 7, 0)

            if(i < 11):
                led_matrix.set_pixel(i+3, 5, 0)
                led_matrix.set_pixel(i+3, 6, 100)
                led_matrix.set_pixel(i+3, 7, 0)
            if((draw_mode == 1)|(mode !=2)|(draw_select !=0)):
                break
            time.sleep(display_drawing_delay)

        for i in range(4, -1, -1):
            led_matrix.set_pixel(0, i, 100)
            led_matrix.set_pixel(1, i, 100)
            led_matrix.set_pixel(2, i, 100)

            led_matrix.set_pixel(0, i+1, 100)
            led_matrix.set_pixel(1, i+1, 0)
            led_matrix.set_pixel(2, i+1, 100)

            led_matrix.set_pixel(0, i+2, 100)
            led_matrix.set_pixel(1, i+2, 100)
            led_matrix.set_pixel(2, i+2, 100)

            if(i == 4):
                led_matrix.set_pixel(0, i+3, 0)
                led_matrix.set_pixel(1, i+3, 0)
                led_matrix.set_pixel(2, i+3, 0)

            if(i == 3):
                led_matrix.set_pixel(0, i+3, 0)

            if(i < 3):
                led_matrix.set_pixel(0, i+3, 0)
                led_matrix.set_pixel(1, i+3, 100)
                led_matrix.set_pixel(2, i+3, 0)
            if((draw_mode == 1)|(mode !=2)|(draw_select !=0)):
                break
            time.sleep(display_drawing_delay)
    led_matrix.clear()
    display_drawing_led_matrix_flag = 0

drawing_flag = 0
def draw_task():
    global draw_mode, draw_select, drawing_flag
    gcode_file = "/resources/gcode/plane.gcode"
    if(draw_select == 1):
        gcode_file = "/resources/gcode/plane.gcode"
    elif draw_select == 2:
        gcode_file = "/resources/gcode/square.gcode"
    elif draw_select == 3:
        gcode_file = "/resources/gcode/triangle.gcode"
    with open(gcode_file, "r") as f:
        if mode != 2:
            print("draw mode to other mode 0\n")
            f.close()
            drawing_flag = 0
            return
        line = f.readline()
        while line:
            if mode != 2:
                print("draw mode to other mode 1\n")
                break
            print("readline:%s" %line)
            #time.sleep(1)
            draw.processGcode(line)

            #暂停模式 不继续执行
            while((draw_mode == 0) & (mode == 2)):
                print("draw_mode == 0")
                time.sleep(0.5)
            
            if mode != 2:
                print("draw mode to other mode 2\n")
                break
            line = f.readline()
    f.close()
    draw_mode = 0
    drawing_flag = 0
    print("read file end!\r\n")


last_ir_code_value = 0
def get_ir_command():
    global last_ir_code_value
    while True:
        new_ir_code_value = sensor.get_ir_code()

        #print("new_ir_code: " + str(new_ir_code_value))

        #当收到四个方向键，当做最终按键值，直接跳出
        if((new_ir_code_value == "left") | (new_ir_code_value == "right") | (new_ir_code_value == "forward") | (new_ir_code_value == "backward")):
            #print("direction ir code\n")
            ir_code_value = new_ir_code_value
            last_ir_code_value = new_ir_code_value
            return ir_code_value
        #当收到非方向键,必须等待按键抬起，也就是收到0时，才返回.
        #后续考虑实现：或者收到非方向键,超过xx ms内未收到0或其他按键，也返回按键值。
        else:
            if(new_ir_code_value == 0):
                if(last_ir_code_value == 0):
                    ir_code_value = 0
                    last_ir_code_value = new_ir_code_value
                    return ir_code_value
                else:
                    ir_code_value = last_ir_code_value
                    last_ir_code_value = new_ir_code_value
                    return ir_code_value
            else:
                last_ir_code_value = new_ir_code_value

#工作模式分为：遥控模式，巡线模式，画画模式，（巡线模式，画画模式下）只有在暂停模式下可以切换工作模式
#巡线模式分为：开始，暂停
#画画模式分为：选择图案，开始，暂停

def ir_command_process(command):
    global mode, line_following_mode, draw_mode, draw_select, drawing_flag

    #遥控模式
    if mode == 0:
        if  command == "play":
            #mode = 1
            print("mode 0 play ")
            #显示巡线表情面板
            #显示LED
        elif command == "forward":
            #设置前进速度
            print("mode 0 forward ")
        elif command == "backward":
            #设置前进速度
            print("mode 0 backward ")
        elif command == "left":
            #左转
            print("mode 0 left ")
        elif command == "right":
            #右转
            print("mode 0 right ")
        elif command == "mode":
            print("mode 0 mode ")
            mode = 1
            audio.play_say("english", "Line following mode", sync = True)
            #显示Line following表情面板

    #巡线模式
    elif mode == 1:
        #Play巡线开始，暂停模式切换，在暂停模式下，模式按键可切换工作模式
        if command == "mode":
            #在暂停模式下，模式按键可切换工作模式
            if line_following_mode == 0:
                mode = 2
                _thread.start_new_thread(display_drawing_led_matrix, ())
                audio.play_say("english", "Drawing mode", sync = True)
                #_thread.start_new_thread(display_drawing_led_matrix, ())
                #audio.play_say("english", "Drawing mode", sync = True)
                print("mode 1 mode ")
        elif command == "play":
            #Play巡线开始，暂停模式切换
            line_following_mode = not line_following_mode
            print("mode 1 play " + str(line_following_mode))
    #画画模式，开始，暂停模式切换，在画画暂停模式下，1,2,3切换图案，模式按键切换工作模式
    elif mode == 2:
        #Play巡线开始，暂停模式切换，只有暂停模式才可切换工作模式
        if command == "mode":
            if draw_mode == 0:
                mode = 0
                draw_select = 0
                print("mode 2 mode ")
                audio.play_say("english", "Remote control mode", sync = True)
                #显示画画表情面板
                #显示LED
        elif command == "play":
            draw_mode = not draw_mode
            print("draw_mode: " + str(draw_mode))
            if(draw_mode == 1):
                if drawing_flag == 0:
                    _thread.start_new_thread(draw_task, ())
                    drawing_flag = 1
        elif command == "1":
            #显示图案1
            draw_select = 1
            while(display_drawing_led_matrix_flag == 1):
                time.sleep(0.2)
            time.sleep(0.5)
            led_matrix.show_image(bytearray([0x00,0x00,0x20,0x00,0x26,0x00,0x24,0x00,0x24,0x00,0x24,0x00,0x2e,0x00,0x20,0x00]), "None")
        elif command == "2":
            #显示图案2
            draw_select = 2
            while(display_drawing_led_matrix_flag == 1):
                time.sleep(0.2)
            led_matrix.show_image(bytearray([0x00,0x00,0x20,0x00,0x2e,0x00,0x28,0x00,0x2e,0x00,0x22,0x00,0x2e,0x00,0x20,0x00]), "None")
        elif command == "3":
            #显示图案3
            draw_select = 3
            while(display_drawing_led_matrix_flag == 1):
                time.sleep(0.2)
            led_matrix.show_image(bytearray([0x00,0x00,0x20,0x00,0x2e,0x00,0x28,0x00,0x2e,0x00,0x28,0x00,0x2e,0x00,0x20,0x00]), "None")

while True:
    ir_command = get_ir_command()
    ir_command_process(ir_command)
    time.sleep(0.2)

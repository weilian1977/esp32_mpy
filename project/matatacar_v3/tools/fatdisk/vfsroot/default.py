import sensor, time, draw

mode = 0
line_following_mode = 0
draw_mode = 0
draw_select = 0


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
                audio.play_say("english", "Drawing mode", sync = True)
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
            led_matrix.show_image(bytearray([0x00,0x00,0x20,0x00,0x26,0x00,0x24,0x00,0x24,0x00,0x24,0x00,0x2e,0x00,0x20,0x00]), "None")
        elif command == "2":
            #显示图案2
            draw_select = 2
            led_matrix.show_image(bytearray([0x00,0x00,0x20,0x00,0x2e,0x00,0x28,0x00,0x2e,0x00,0x22,0x00,0x2e,0x00,0x20,0x00]), "None")
        elif command == "3":
            #显示图案3
            draw_select = 3
            led_matrix.show_image(bytearray([0x00,0x00,0x20,0x00,0x2e,0x00,0x28,0x00,0x2e,0x00,0x28,0x00,0x2e,0x00,0x20,0x00]), "None")

while True:
    ir_command = get_ir_command()
    ir_command_process(ir_command)
    time.sleep(0.2)

from oak_lite import *
print("value")
value = motion_sensor.get_acceleration('x')
print(value)
while True:
    if motion_sensor.is_tilted_left():
        print("tilted_left")
    if motion_sensor.is_tilted_right():
        print("tilted_right")
    if motion_sensor.is_arrow_up():
        print("arrow_up")
    if motion_sensor.is_arrow_down():
        print("arrow_down")
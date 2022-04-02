import time, event

@event.tilted_left
def on_tilted_left():
    print("tilted left\n")

@event.tilted_right
def on_tilted_right():
    print("tilted right\n")

@event.arrow_up
def on_arrow_up():
    print("arrow up\n")

@event.arrow_down
def on_arrow_up():
    print("arrow down\n")
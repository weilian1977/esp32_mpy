import time, event

@event.button_pressed
def is_button_pressed():
    print("is_button_pressed")
    time.sleep(0.1)

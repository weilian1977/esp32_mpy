from oak_lite import *

while True:         
    if button.is_pressed():
        led.show_rgb(5,5,0)                  
    else: 
        led.show_rgb(0, 0, 0)

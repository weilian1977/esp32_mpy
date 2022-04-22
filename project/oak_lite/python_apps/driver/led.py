import machine, neopixel

class led(object):
    def __init__(self):
        pass

    def show_rgb(self, r, g, b):
        buletooth_led = neopixel.NeoPixel(machine.Pin(8), 1)
        buletooth_led[0] = (r, g, b)
        buletooth_led.write()
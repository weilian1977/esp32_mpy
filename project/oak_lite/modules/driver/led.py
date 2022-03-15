import machine, neopixel


def show_rgb(r , g, b):
    buletooth_led = neopixel.NeoPixel(machine.Pin(8), 1)
    buletooth_led[0] = (r, g, b)
    buletooth_led.write()

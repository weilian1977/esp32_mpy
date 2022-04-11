from matatalab import button

button_o = button()

class button(object):
    def __init__(self):
        pass

    def is_pressed(self):
        return bool(button_o.is_pressed())
    
    def is_released(self):
        return bool(button_o.is_released())
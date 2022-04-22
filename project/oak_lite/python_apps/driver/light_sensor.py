from matatalab import light_sensor

light_sensor_o = light_sensor()

class light_sensor(object):
    def __init__(self):
        pass

    def get_value(self):
        value = light_sensor_o.get_value()
        return  value

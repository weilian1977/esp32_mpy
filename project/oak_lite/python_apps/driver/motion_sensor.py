# from global_objects import gyro_o
# from utility.common import num_range_scale

from matatalab import gyro

gyro_o = gyro()

AXIS_X = 1
AXIS_Y = 2
AXIS_Z = 3

TILE_LEFT_ID = 1
TILE_RIGHT_ID = 2
TILE_FORWARD_ID = 3
TILE_BACKWARD_ID = 4

class motion_sensor(object):
    def __init__(self):
        pass

    def get_acceleration(self, axis):
        value = 0
        if isinstance(axis, str):
            if axis == 'x':
                value = gyro_o.get_acc(AXIS_X)
            elif axis == 'y':
                value = gyro_o.get_acc(AXIS_Y)
            elif axis == 'z':
                value = -1 * gyro_o.get_acc(AXIS_Z)
        return  value

    def get_gyroscope(self, axis):
        value = 0
        if isinstance(axis, str):
            if axis == 'x':
                value = -1 * gyro_o.get_gyro(AXIS_X)
            elif axis == 'y':
                value = -1 * gyro_o.get_gyro(AXIS_Y)
            elif axis == 'z':
                value = gyro_o.get_gyro(AXIS_Z)
        return int(value * 10) / 10

    def get_rotation(self, axis):
        value = 0
        if isinstance(axis, str):
            if axis == 'x':
                value = -1 * gyro_o.get_rotate_angle(AXIS_X)
            elif axis == 'y':
                value = -1 * gyro_o.get_rotate_angle(AXIS_Y)
            elif axis == 'z':
                value = gyro_o.get_rotate_angle(AXIS_Z)
            else:
                return 0
        return value

    def reset_rotation(self, axis = "all"):
        if isinstance(axis, str):
            if axis == 'x':
                value = gyro_o.reset_rotate_angle(AXIS_X)
            elif axis == 'y':
                value = gyro_o.reset_rotate_angle(AXIS_Y)
            elif axis == 'z':
                value = gyro_o.reset_rotate_angle(AXIS_Z)
            elif axis == "all":
                value = gyro_o.reset_rotate_angle(AXIS_X)
                value = gyro_o.reset_rotate_angle(AXIS_Y)
                value = gyro_o.reset_rotate_angle(AXIS_Z)

    def is_tilted_left(self):
        return bool(gyro_o.get_tilt(TILE_LEFT_ID))

    def is_tilted_right(self):
        return bool(gyro_o.get_tilt(TILE_RIGHT_ID))

    def is_tilted_forward(self):
        return bool(gyro_o.get_tilt(TILE_FORWARD_ID))

    def is_tilted_backward(self):
        return bool(gyro_o.get_tilt(TILE_BACKWARD_ID))

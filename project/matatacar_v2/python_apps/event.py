import events.event_manager as event_manager

from event_obj import event_o

def start(callback):
    print("System start")
    event_manager.event_register(event_o.EVE_SYSTEM_LAUNCH, event_o.TRIGGER_ALWAYS_WITH_NO_PARAMETER, callback, None)

def button_a_pressed(callback):
    event_manager.event_register(event_o.EVENT_BUTTON_A_PRESSED, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def button_a_released(callback):
    event_manager.event_register(event_o.EVENT_BUTTON_A_RELEASED, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def button_b_pressed(callback):
    event_manager.event_register(event_o.EVENT_BUTTON_B_PRESSED, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def button_b_released(callback):
    event_manager.event_register(event_o.EVENT_BUTTON_B_RELEASED, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def button_play_pressed(callback):
    event_manager.event_register(event_o.EVENT_BUTTON_PLAY_PRESSED, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def button_play_released(callback):
    event_manager.event_register(event_o.EVENT_BUTTON_PLAY_RELEASED, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def light_left_more_than(threshold):
    def decorator(callback):
        if not isinstance(threshold, (int, float)):
             return
        threshold_data = threshold
        if threshold_data < 0:
            threshold_data = 0
        event_manager.event_register(event_o.EVENT_LIGHT_LEFT_MORE, event_o.TRIGGER_ONCE_BY_VALUE_LARGER, callback, threshold_data)
    return decorator

def light_left_less_than(threshold):
    def decorator(callback):
        if not isinstance(threshold, (int, float)):
            print("left isinstance\n")
            return
        threshold_data = threshold
        if threshold_data < 0:
            print("left  thre\n")
            threshold_data = 0
        event_manager.event_register(event_o.EVENT_LIGHT_LEFT_LESS, event_o.TRIGGER_ONCE_BY_VALUE_SMALLER, callback, threshold_data)
    return decorator

def light_right_more_than(threshold):
    def decorator(callback):
        if not isinstance(threshold, (int, float)):
            print("isinstance\n")
            return
        threshold_data = threshold
        if threshold_data < 0:
            print("right  thre\n")
            threshold_data = 0
        event_manager.event_register(event_o.EVENT_LIGHT_RIGHT_MORE, event_o.TRIGGER_ONCE_BY_VALUE_LARGER, callback, threshold_data)
    return decorator

def light_right_less_than(threshold):
    def decorator(callback):
        if not isinstance(threshold, (int, float)):
             return
        threshold_data = threshold
        if threshold_data < 0:
            threshold_data = 0
        event_manager.event_register(event_o.EVENT_LIGHT_RIGHT_LESS, event_o.TRIGGER_ONCE_BY_VALUE_SMALLER, callback, threshold_data)
    return decorator

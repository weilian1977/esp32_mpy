import events.event_manager as event_manager

from event_obj import event_o

def start(callback):
    print("System start")
    event_manager.event_register(event_o.EVE_SYSTEM_LAUNCH, event_o.TRIGGER_ALWAYS_WITH_NO_PARAMETER, callback, None)

def button_pressed(callback):
    event_manager.event_register(event_o.EVENT_BUTTON, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def tilted_left(callback):
    print("event_register left")
    event_manager.event_register(event_o.EVENT_TILT_LEFT, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def tilted_right(callback):
    print("event_register right")
    event_manager.event_register(event_o.EVENT_TILT_RIGHT, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def arrow_down(callback):
    print("event_register down")
    event_manager.event_register(event_o.EVENT_TILT_FORWARD, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

def arrow_up(callback):
    print("event_register up")
    event_manager.event_register(event_o.EVENT_TILT_BACKWARD, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

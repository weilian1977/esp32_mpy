import events.event_manager as event_manager

from event_obj import event_o

def start(callback):
    print("System start")
    event_manager.event_register(event_o.EVE_SYSTEM_LAUNCH, event_o.TRIGGER_ALWAYS_WITH_NO_PARAMETER, callback, None)

def button_pressed(callback):
    event_manager.event_register(event_o.EVENT_BUTTON, event_o.TRIGGER_ONCE_BY_VALUE_TRUE, callback, None)

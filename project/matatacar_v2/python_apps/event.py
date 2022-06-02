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

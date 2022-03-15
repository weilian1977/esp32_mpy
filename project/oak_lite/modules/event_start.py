import events.event_manager as event_manager
from event_obj import event_o

event_manager.event_system_start()
time.sleep(0.1)
event_manager.event_trigger(event_o.EVE_SYSTEM_LAUNCH)


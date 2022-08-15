import _thread
import time
from event_obj import event_o
from matatalab import stop_script

stop_script_o = stop_script()

EVENT_THREAD_DEFAULT_STACK_SIZE = 1024 * 8
EVENT_THREAD_DEFAULT_PRIORITY = 1

EVENT_STATUS_FATAL_ERROR = -1
EVENT_STATUS_NOT_START = 0
EVENT_STATUS_READY = 1
EVENT_STATUS_RUNNING = 2

STACK_TYPE_INTERNAL = 1
STACK_TYPE_SPIRAM = 2

new_events_info = []
current_events_info = [None] * event_o.EVE_MAX_NUM

def __is_event_id_valid(eve_id):
    if eve_id >= 0 and eve_id < event_o.EVE_MAX_NUM:
        return True
    else:
        return False

def __event_info_set(eve_id, value):
    global current_events_info, new_events_info
    if __is_event_id_valid(eve_id):
        current_events_info[eve_id] = value

def get_event_status(eve_type, para = None):
    global current_events_info, new_events_info
    ret = []
    for item in current_events_info:
        if item and item.is_event_valid():
            if item.event_type == eve_type:
                if para:
                    if para != item.user_para:
                        continue
                ret.append((item.eve_id, item.event_status))

    return ret


class event_class(object):
    def __init__(self, event_type, trigger_type, user_cb, user_para = 0, \
                 stack_size = EVENT_THREAD_DEFAULT_STACK_SIZE, priority = EVENT_THREAD_DEFAULT_PRIORITY, stack_type = STACK_TYPE_SPIRAM):
        self.eve_id = event_o.event_register(event_type, trigger_type, user_para)
        if __is_event_id_valid(self.eve_id):
            self.event_type = event_type
            self.user_cb = user_cb
            self.user_para = user_para
            self.stack_size = stack_size
            self.stack_type = stack_type
            self.priority = priority
            self.event_status = EVENT_STATUS_NOT_START
            self.operation_handle = None
        else:
            self.event_type = None
            self.user_cb = None
            self.user_para = None
            self.stack_type = None
            self.stack_size = None
            self.priority = None
            self.event_status = EVENT_STATUS_FATAL_ERROR
            self.operation_handle = None
            #print_dbg("event register failed")

    def is_event_valid(self):
        if __is_event_id_valid(self.eve_id) and self.event_status != EVENT_STATUS_FATAL_ERROR:
            return True
        else:
            return False

    def redefine(self, event_type, trigger_type, user_cb, user_para):
        if __is_event_id_valid(self.eve_id):
            self.event_type = event_type
            self.user_cb = user_cb
            self.user_para = user_para
            self.operation_handle.redefine()

            event_o.event_redefine(self.eve_id, event_type, trigger_type, user_para)

def __function_type():
    pass

def is_function(func):
    if type(func) == type(__function_type):
        return True
    else:
        return False

class event_operation(object):
    def __init__(self, event_class):
        self.event_class = event_class
        self.eve_id = event_class.eve_id
        self.cb = event_class.user_cb
        self.stack_size = event_class.stack_size
        self.stack_type = event_class.stack_type
        self.priority = event_class.priority

    def redefine(self):
        self.cb = self.event_class.user_cb

    def __event_cb_task(self):
        thread_id = stop_script_o.add_thread()
        # only call this function once at the top of thread function
        while True:
            event_o.clear_sync(self.eve_id)
            KeyboardInterrupt_flag = False
            try:
                while True:
                    if __is_event_id_valid(self.eve_id):
                        self.event_class.event_status = EVENT_STATUS_READY
                        stop_script_o.set_thread_sta(thread_id, stop_script_o.THREAD_RESTARTED)
                        if event_o.wait_trigger(self.eve_id) == True:
                            # Call user callback function
                            if is_function(self.cb):
                                self.event_class.event_status = EVENT_STATUS_RUNNING
                                stop_script_o.set_thread_sta(thread_id, stop_script_o.THREAD_EXECUTING)
                                self.cb()
                            event_o.clear_sync(self.eve_id)
                        else:
                            continue
                    else:
                        #print_dbg("eve_id is null")
                        time.sleep(10)
            # if error occured in the callback, the sema RAM will be freed, 
            # but other function will still use this sema, then a fatal system error happend
            # catch the exception and make this task never out is a temporary solution
            except KeyboardInterrupt:
                stop_script_o.set_thread_sta(thread_id, stop_script_o.THREAD_RESTARTED)
                print("restart the thread proactively", "id is", self.eve_id)
                KeyboardInterrupt_flag = True
            
            except Exception as e:
                # when error occured, set the item in event_sema_list to None, 
                # idicating that this callback had been destroyed
                if not KeyboardInterrupt_flag:
                    stop_script_o.set_thread_sta(thread_id, stop_script_o.THREAD_FATAL_ERROR)
                    
                    if __is_event_id_valid(self.eve_id):
                        __event_info_set(self.eve_id, None)
                    
                    event_o.event_unregister(self.eve_id)
                    print("event:", self.eve_id, "error occured:")
                    print("free the memory of this function")
                    raise e

    def __event_execute_cb(self):
        _thread.stack_size(self.stack_size)
        if self.stack_type == STACK_TYPE_INTERNAL:
            _thread.start_new_thread(self.__event_cb_task, (), self.priority)
        else:
            _thread.start_new_thread(self.__event_cb_task, (), self.priority, STACK_TYPE_SPIRAM)
  
    def event_listening_start(self):
        self.__event_execute_cb()

######################################################################################
def event_trigger(eve_type, parameter = None):
    event_o.trigger_by_type(eve_type, parameter)

def event_register(event_type, trigger_type, user_cb, user_para = 0, \
                   stack_size = EVENT_THREAD_DEFAULT_STACK_SIZE, priority = EVENT_THREAD_DEFAULT_PRIORITY, stack_type = STACK_TYPE_SPIRAM):

    global current_events_info, new_events_info
    if len(new_events_info) > event_o.EVE_MAX_NUM:
        print("too many event are registered", event_o.EVE_MAX_NUM)
        return
    new_events_info.append((event_type, trigger_type, user_cb, user_para, stack_size, priority, stack_type))

def event_system_start():
    global current_events_info, new_events_info
    event_o.trigger_disable()
    for i in range(len(current_events_info)):
        if current_events_info[i]:
            event_o.set_status(current_events_info[i].eve_id, event_o.EVE_STATUS_DISABLE)

    for i in range(len(new_events_info)):
        if current_events_info[i] == None:
            # add new
            event = event_class(*new_events_info[i])
            if event.is_event_valid():
                __event_info_set(event.eve_id, event)
                ope = event_operation(event)
                ope.event_listening_start()
                event.operation_handle = ope
                event_o.set_status(event.eve_id, event_o.EVE_STATUS_ENABLE)
            elif __is_event_id_valid(event.eve_id):
                __event_info_set(event.eve_id, None)
                print("event register failed, id is valid")
            else:
                print("event register failed")
        else:
            # redefine
            current_events_info[i].redefine(new_events_info[i][0], new_events_info[i][1], new_events_info[i][2], new_events_info[i][3])
            event_o.set_status(current_events_info[i].eve_id, event_o.EVE_STATUS_ENABLE)
    
    new_events_info = []
    event_o.trigger_enable()

def clear_register_event_info():
    global new_events_info
    new_events_info = []
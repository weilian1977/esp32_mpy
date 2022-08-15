from matatalab import stop_script
_script_stop = stop_script()

def stop(select):
    if select ==  "all scripts":
        _script_stop.stop_all_script()
    elif select ==  "this script":
        _script_stop.stop_this_script()
    elif select ==  "other scripts":
        _script_stop.stop_other_script()
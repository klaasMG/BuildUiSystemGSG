import threading
from ui_debug import instant_debug_func

def tprint(*args, **kwargs):
    thread = threading.current_thread()
    print(f"[{thread.name} | {thread.ident}]" , *args, **kwargs)
    
def dbg(*args, **kwargs):
    instant_debug_func(print, *args, **kwargs)
    
def tdbg(*args, **kwargs):
    instant_debug_func(tprint, *args, **kwargs)
import threading
import sys

def tprint(*args, **kwargs):
    line = sys._getframe().f_lineno
    thread = threading.current_thread()
    print(f"[{thread.name} | {thread.ident}] on line: {line}", *args, **kwargs)
import threading
from threading import Lock

class HoldLock:
    def __init__(self):
        self.internal_lock = Lock()
        self.locked_from_thread = None
    
    def lock(self, time_out: None | float = None):
        if self.locked_from_thread != threading.get_ident():
            if time_out:
                acquired = self.internal_lock.acquire(blocking=True, timeout=time_out)
            else:
                acquired = self.internal_lock.acquire(blocking=True)
        else:
            acquired = False
        if acquired:
            self.locked_from_thread = threading.get_ident()
        return acquired
    
    def release(self):
        released = False
        if threading.get_ident() == self.locked_from_thread:
            if self.internal_lock.locked():
                self.internal_lock.release()
                self.locked_from_thread = None
                released = True
        return released
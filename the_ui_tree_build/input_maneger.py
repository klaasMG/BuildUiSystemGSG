import queue
import time
from pynput import keyboard, mouse
from enum import Enum

class InputEvent(Enum):
    MouseMove = 0
    MouseClick = 1
    MouseScroll = 2
    KeyPress = 3
    KeyRelease = 4

class InputManager:
    def __init__(self):
        self.mouse_listener = mouse.Listener(
            on_move=self.on_move,
            on_click=self.on_click,
            on_scroll=self.on_scroll
        )
        
        self.keyboard_listener = keyboard.Listener(
            on_press=self.on_press,
            on_release=self.on_release
        )
        self.ui_event_queue = queue.SimpleQueue()
        self.mouse_listener.start()
        self.keyboard_listener.start()

    def on_move(self, x, y):
        self.ui_event_queue.put((InputEvent.MouseMove,x,y))

    def on_click(self, x, y, button, pressed):
        self.ui_event_queue.put((InputEvent.MouseClick, x, y, button, pressed))

    def on_scroll(self, x, y, dx, dy):
        self.ui_event_queue.put((InputEvent.MouseScroll, x, y, dx, dy))

    def on_press(self, key):
        self.ui_event_queue.put((InputEvent.KeyPress,key))

    def on_release(self, key):
        self.ui_event_queue.put((InputEvent.KeyRelease, key))
    
manger = InputManager()

while True:
    time.sleep(1)
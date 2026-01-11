import time
from pynput import keyboard, mouse

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
        self.mouse_listener.start()
        self.keyboard_listener.start()

    def on_move(self, x, y):
        pass

    def on_click(self, x, y, button, pressed):
        pass

    def on_scroll(self, x, y, dx, dy):
        pass

    def on_press(self, key):
        pass

    def on_release(self, key):
        pass
    
manger = InputManager()

while True:
    time.sleep(1)
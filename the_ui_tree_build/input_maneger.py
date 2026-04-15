import queue
from pynput import keyboard, mouse
from enum import Enum
from typing import Callable, Any


class InputEvent(Enum):
    MouseMove = 0
    MouseClick = 1
    MouseScroll = 2
    KeyPress = 3
    KeyRelease = 4

class ActionType(Enum):
    MoveMouse = 0
    Click = 1
    Scroll = 2
    KeyPress = 3
    KeyRelease = 4

class Action:
    def __init__(self, action_type: ActionType, action_data):
        self.action_type: ActionType = action_type
        self.data = action_data

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
        self.ui_event_queue.put(Action(ActionType.MoveMouse, (x, y)))

    def on_click(self, x, y, button, pressed):
        self.ui_event_queue.put(Action(ActionType.Click, (x, y, button, pressed)))

    def on_scroll(self, x, y, dx, dy):
        self.ui_event_queue.put(Action(ActionType.Scroll, (x, y, dx, dy)))

    def on_press(self, key):
        self.ui_event_queue.put(Action(ActionType.KeyPress, key))

    def on_release(self, key):
        self.ui_event_queue.put(Action(ActionType.KeyRelease, key))

class InputRegistry:
    def __init__(self):
        self.registry: dict[tuple[int, Action], dict[Callable, Any]] = {}

    def register_action(self, action: Action, id:int, func: Callable, rules: Any):
        self.registry[id, action] = {func : rules}

    def change_rules(self, id:int, action: Action,func: Callable, rules: Any):
        func_rules_dict = self.registry[id, action]
        func_rules = func_rules_dict[func]
        func_rules[rules] = rules
        self.registry[id, action][func] = func_rules

    def remove_action(self, id:int, action: Action):
        del self.registry[id, action]

    def remove_func(self, id:int, action: Action, func: Callable):
        del self.registry[id, action][func]

    def register_func(self, id: int, action: Action, func: Callable, rules: Any):
        functions = self.registry[id, action]
        functions[func] = rules
        self.registry[id, action] = functions

    def lookup(self, id: int, action: Action):
        func_focus = self.registry[id, action]
        return func_focus
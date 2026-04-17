from libraries.event_queue_system import EventSystem, EventQueue
from enum import Enum
class EventTypeEnum(Enum):
    Resize = "resize"
    Mouse_Pos = "mouse_pos"

event_system = EventSystem()
event_check_resize = "int:0:10000;int:0:5000"
event_system.add_message_type(EventTypeEnum.Resize, event_check = event_check_resize)
event_check_mouse_pos = "int:0:10000;int:0:5000"
event_system.add_message_type(EventTypeEnum.Mouse_Pos, event_check=event_check_mouse_pos)
event_system.handle_event_threaded()
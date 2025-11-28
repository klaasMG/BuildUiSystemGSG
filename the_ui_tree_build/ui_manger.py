from renderer import GSGRenderSystem
import sys
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import numpy as np
from widget_data import WidgetDataType
from event_system import event_system, EventQueue, EventTypeEnum

class GSGWidget:
    FLAG_VISIBLE = 1 << 0
    FLAG_RENDER = 1 << 1
    FLAG_DIRTY = 1 << 2
    FLAG_NEEDS_LAYOUT = 1 << 3
    
    def __init__(self , flags=0 , parent=None):
        self.id = 0
        self.children = {}
        self.flags = flags
        self.last_id = -1
        self.parent = parent
        self.widget_max = 10000
    
    def set_flag(self , flag):
        self.flags |= flag
    
    def clear_flag(self , flag):
        self.flags &= ~flag
    
    def has_flag(self , flag):
        return (self.flags & flag) != 0
    
    def add_child(self , child):
        if child.parent:
            child.parent.remove_child(child)
        self.children[child.id] = child
        child.parent = self
    
    def remove_child(self , child , manager=None):
        if child.id in self.children:
            del self.children[child.id]
        child.parent = None
        if manager:
            manager.remove_widget_subtree(child)
class app(QApplication):
    def __init__(self, *args, event_system = None, **kwargs):
        super().__init__(*args , **kwargs)
        self.event_system = event_system
        self.aboutToQuit.connect(self.on_quit)
    
    def on_quit(self):
        print("Application is quitting")
        if self.event_system is not None:
            self.event_system.stop_event_system()

class GSGUiManager:
    def __init__(self):
        self.depth_layers = 100
        self.widget_data = {}
        self.widget_max = 10000
        self.init_widget_data(widget_data_types={0: (WidgetDataType.POSITION , (self.widget_max * 6 , np.int32)) ,
                                                 1: (WidgetDataType.SHADER_PASS , (self.widget_max , np.int32)) ,
                                                 2: (WidgetDataType.COLOUR , (self.widget_max * 4 , np.int32)) ,
                                                 3: (WidgetDataType.SHAPE , (self.widget_max , np.int32)) ,
                                                 4: (WidgetDataType.ASSETS_ID , (self.widget_max , np.int32)) ,
                                                 5: (WidgetDataType.TEXT_ID , (self.widget_max , np.int32)) ,
                                                 6: (WidgetDataType.PARENT , (self.widget_max , np.int32))})
        self.widgets_by_id = {}
        self.free_ids = []
        self.next_id = 1
        self.GSG_renderer_system = None
        self.ui_manager_queue: EventQueue = event_system.add_queue("ui_manager")
        self.root = GSGWidget(0)
        self.append_widget(self.root,data=None)
        self.width = 600
        self.height = 800
        self.app = app(sys.argv , event_system=event_system)
    
    def run_ui_manager(self):
        self.running = True
        self.GSG_renderer_system = GSGRenderSystem(self)
        self.GSG_renderer_system.show()
        
        self.sqaure = GSGWidget(parent=self.root)
        self.append_widget(self.sqaure, [4, 16, 1, 10, 10, 1, 255, 0, 0, 255, 2, -1, -1, -1, ])
        
        self.frame_timer = QTimer()
        self.frame_timer.timeout.connect(self.update_ui_manager)
        self.frame_timer.start(16)  # ~60 FPS
        
        sys.exit(self.app.exec_())
    
    def update_ui_manager(self):
        event = self.ui_manager_queue.receive_event()
        self.update_widgets()
        self.GSG_renderer_system.update()
        
    def use_event(self, event):
        event_type, data = event
        if event_type == 255:
            return None
        if event_type == EventTypeEnum.Resize:
            self.width = data[0]
            self.height = data[1]
        return None
    
    def update_widgets(self):
        pass
    
    def append_widget(self , widget, data):
        if self.free_ids:
            widget.id = self.free_ids.pop()
        else:
            widget.id = self.next_id
            self.next_id += 1
        self.widgets_by_id[widget.id] = widget
        
        self.set_widget_defaults(widget, data=data if isinstance(data, list) else [])
    
    def update_widget(self , widget , data=None):
        if not data or len(data) != 14 or data == [-1] * 14:
            return
        i = widget.id
        pos = data[0:6]
        col = data[6:10]
        for p , j in enumerate(pos):
            if j == -1:
                pos[p] = self.widget_data[WidgetDataType.POSITION][i * 4 + p]
        for p , j in enumerate(col):
            if j == -1:
                col[p] = self.widget_data[WidgetDataType.COLOUR][i * 4 + p]
        self.widget_data[WidgetDataType.POSITION][i * 6:i * 6 + 6] = pos
        self.widget_data[WidgetDataType.COLOUR][i * 4:i * 4 + 4] = col
        self.widget_data[WidgetDataType.SHADER_PASS][i] = data[10] if data[10] != -1 else self.widget_data[WidgetDataType.SHADER_PASS][i]
        self.widget_data[WidgetDataType.SHAPE][i] = data[11] if data[11] != -1 else self.widget_data[WidgetDataType.SHAPE][i]
        self.widget_data[WidgetDataType.PARENT][i] = widget.parent.id if widget.parent else self.widget_data[WidgetDataType.PARENT][i]
        self.widget_data[WidgetDataType.TEXT_ID][i] = data[12] if data[12] != -1 else self.widget_data[WidgetDataType.TEXT_ID][i]
        self.widget_data[WidgetDataType.ASSETS_ID][i] = data[13] if data[13] != -1 else self.widget_data[WidgetDataType.ASSETS_ID][i]
    
    def set_widget_defaults(self , widget , data=None):
        if not data or len(data) != 14:
            data = [-1] * 14
        i = widget.id
        pos = data[0:6]
        col = data[6:10]
        self.widget_data[WidgetDataType.POSITION][i * 6:i * 6 + 6] = pos
        self.widget_data[WidgetDataType.COLOUR][i * 4:i * 4 + 4] = col
        self.widget_data[WidgetDataType.SHADER_PASS][i] = data[10]
        self.widget_data[WidgetDataType.SHAPE][i] = data[11]
        self.widget_data[WidgetDataType.PARENT][i] = widget.parent.id if widget.parent else -1
        self.widget_data[WidgetDataType.TEXT_ID][i] = data[12]
        self.widget_data[WidgetDataType.ASSETS_ID][i] = data[13]
    
    def clear_widget_data(self , wid):
        default = -1
        self.widget_data[WidgetDataType.POSITION][wid * 6:wid * 6 + 6] = [default , default , default , default ,
                                                                          default , default]
        self.widget_data[WidgetDataType.COLOUR][wid * 4:wid * 4 + 4] = [default , default , default , default]
        self.widget_data[WidgetDataType.SHADER_PASS][wid] = default
        self.widget_data[WidgetDataType.SHAPE][wid] = default
        self.widget_data[WidgetDataType.ASSETS_ID][wid] = default
        self.widget_data[WidgetDataType.TEXT_ID][wid] = default
        self.widget_data[WidgetDataType.PARENT][wid] = default
    
    def init_widget_data(self , widget_data_types: dict):
        for key , (size , dtype) in widget_data_types.values():
            print((key ,size , dtype))
            arr = np.full(size , -1 , dtype=dtype)
            self.widget_data[key] = arr
    
    def remove_widget_subtree(self , root_widget):
        stack = [root_widget]
        while stack:
            w = stack.pop()
            stack.extend(w.children.values())
            
            # remove from manager
            self.widgets_by_id.pop(w.id , None)
            self.free_ids.append(w.id)
            
            # 🔥 clear its data
            self.clear_widget_data(w.id)
            
            # clear children
            w.children.clear()


if __name__ == "__main__":
    manager = GSGUiManager()
    manager.run_ui_manager()
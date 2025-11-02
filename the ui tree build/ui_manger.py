from renderer import GSGRenderSystem
import sys
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer
import numpy as np
from widget_data import WidgetDataType

class GSGWidget:
    FLAG_VISIBLE = 1 << 0
    FLAG_RENDER = 1 << 1
    FLAG_DIRTY = 1 << 2
    FLAG_NEEDS_LAYOUT = 1 << 3
    
    def __init__(self,flags=0, parent = None):
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
        
    def remove_child(self, child, manager=None):
        if child.id in self.children:
            del self.children[child.id]
        child.parent = None
        if manager:
            manager.remove_widget_subtree(child)
    
class GSGUiManager:
    def __init__(self):
        self.widget_data = {}
        self.widget_max = 10000
        self.init_widget_data(widget_data_types={1: (WidgetDataType.POSITION , (self.widget_max * 4 , np.int64)) ,
                                                 2: (WidgetDataType.SHADER_PASS , (self.widget_max , np.int16)) ,
                                                 3: (WidgetDataType.COLOUR , (self.widget_max * 4 , np.int16)) ,
                                                 4: (WidgetDataType.SHAPE , (self.widget_max , np.int16)) ,
                                                 5: (WidgetDataType.ASSETS_ID , (self.widget_max , np.int32)) ,
                                                 6: (WidgetDataType.TEXT_ID , (self.widget_max , np.int32)) ,
                                                 7: (WidgetDataType.PARENT , (self.widget_max , np.int32))})
        self.widgets_by_id = {}
        self.free_ids = []
        self.next_id = 1
        self.GSG_renderer_system = None
        self.root = GSGWidget(0)
        self.append_widget(self.root)
        
    def run_ui_manager(self):
        self.running = True
        self.app = QApplication(sys.argv)
        self.GSG_renderer_system = GSGRenderSystem(self)
        self.GSG_renderer_system.show()
        
        self.frame_timer = QTimer()
        self.frame_timer.timeout.connect(self.update_ui_manager)
        self.frame_timer.start(16)  # ~60 FPS
        
        sys.exit(self.app.exec_())
        
    def update_ui_manager(self):
        self.update_widgets()
        self.GSG_renderer_system.update()
        
    def update_widgets(self):
        pass
        
    def append_widget(self,widget):
        if self.free_ids:
            widget.id = self.free_ids.pop()
        else:
            widget.id = self.next_id
            self.next_id += 1
        self.widgets_by_id[widget.id] = widget
        
        self.set_widget_defaults(widget)
        
    def update_widget(self, widget, data = None):
        if not data or len(data) != 12 or data == [-1] * 12:
            return
        i = widget.id
        pos = data[0:4]
        col = data[4:8]
        for p, j in enumerate(pos):
            if j == -1:
                pos[p] = self.widget_data[WidgetDataType.POSITION][i * 4 + p]
        for p, j in enumerate(col):
            if j == -1:
                pos[p] = self.widget_data[WidgetDataType.COLOUR][i * 4 + p]
        self.widget_data[WidgetDataType.POSITION][i * 4:i * 4 + 4] = pos
        self.widget_data[WidgetDataType.COLOUR][i * 4:i * 4 + 4] = col
        self.widget_data[WidgetDataType.SHADER_PASS][i] = data[8] if data[8] != -1 else self.widget_data[WidgetDataType.SHADER_PASS][i]
        self.widget_data[WidgetDataType.SHAPE][i] = data[9] if data[9] != -1 else self.widget_data[WidgetDataType.SHAPE][i]
        self.widget_data[WidgetDataType.PARENT][i] = widget.parent.id if widget.parent else self.widget_data[WidgetDataType.PARENT][i]
        self.widget_data[WidgetDataType.TEXT_ID][i] = data[10] if data[10] != -1 else self.widget_data[WidgetDataType.TEXT_ID][i]
        self.widget_data[WidgetDataType.ASSETS_ID][i] = data[11] if data[11] != -1 else self.widget_data[WidgetDataType.ASSETS_ID][i]
        if self.GSG_renderer_system:
            self.GSG_renderer_system.vertices[i * 5:i * 5 + 3] = pos[0:3]
        
    def set_widget_defaults(self,widget,data = None):
        if not data or len(data) != 12:
            data = [-1] * 12
        i = widget.id
        pos = data[0:4]
        col = data[4:8]
        self.widget_data[WidgetDataType.POSITION][i * 4:i * 4 + 4] = pos
        self.widget_data[WidgetDataType.COLOUR][i * 4:i * 4 + 4] = col
        self.widget_data[WidgetDataType.SHADER_PASS][i] = data[8]
        self.widget_data[WidgetDataType.SHAPE][i] = data[9]
        self.widget_data[WidgetDataType.PARENT][i] = widget.parent.id if widget.parent else -1
        self.widget_data[WidgetDataType.TEXT_ID][i] = data[10]
        self.widget_data[WidgetDataType.ASSETS_ID][i] = data[11]
        if self.GSG_renderer_system:
            self.GSG_renderer_system.vertices[i * 5:i * 5 + 3] = pos[0:3]
    
    def clear_widget_data(self , wid):
        default = -1
        self.widget_data[WidgetDataType.POSITION][wid * 4:wid * 4 + 4] = [default , default , default , default]
        self.widget_data[WidgetDataType.COLOUR][wid * 4:wid * 4 + 4] = [default , default , default , default]
        self.widget_data[WidgetDataType.SHADER_PASS][wid] = default
        self.widget_data[WidgetDataType.SHAPE][wid] = default
        self.widget_data[WidgetDataType.ASSETS_ID][wid] = default
        self.widget_data[WidgetDataType.TEXT_ID][wid] = default
        self.widget_data[WidgetDataType.PARENT][wid] = default
        
    def init_widget_data(self, widget_data_types: dict):
        for key , (size, dtype) in widget_data_types.values():
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
from renderer import GSGRenderSystem
import sys
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import QTimer

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
        self.widgets_by_id = {}
        self.free_ids = []
        self.next_id = 1
        self.root = GSGWidget(0)
        self.append_widget(self.root)
        self.data = []
        
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
        self.GSG_renderer_system.update()
    def append_widget(self,widget):
        if self.free_ids:
            widget.id = self.free_ids.pop()
        else:
            widget.id = self.next_id
            self.next_id += 1
        self.widgets_by_id[widget.id] = widget
    
    def remove_widget_subtree(self, root_widget):
        stack = [root_widget]
        while stack:
            w = stack.pop()
            # add children to stack
            stack.extend(w.children.values())
            # remove from manager
            self.widgets_by_id.pop(w.id , None)
            self.free_ids.append(w.id)
            # clear children
            w.children.clear()
            
if __name__ == "__main__":
    manager = GSGUiManager()
    manager.run_ui_manager()
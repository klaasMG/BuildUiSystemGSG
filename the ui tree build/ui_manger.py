from renderer import GSGRenderSystem

class GSGWidget:
    FLAG_VISIBLE = 1 << 0
    FLAG_RENDER = 1 << 1
    FLAG_DIRTY = 1 << 2
    FLAG_NEEDS_LAYOUT = 1 << 3
    
    def __init__(self,flags=0):
        self.id = 0
        self.children = {}
        self.flags = flags
        self.last_id = -1
    
    def set_flag(self , flag):
        self.flags |= flag
    
    def clear_flag(self , flag):
        self.flags &= ~flag
    
    def has_flag(self , flag):
        return (self.flags & flag) != 0
    
    def add_child(self , child):
        self.children[child.id] = child
        
    def remove_child(self, child, manager=None):
        if manager:
            manager.remove_widget_subtree(child)
    
class GSGuUiManger:
    def __init__(self):
        self.widgets_by_id = {}
        self.free_ids = []
        self.next_id = 1
        self.root = GSGWidget(0)
        self.append_widget(self.root)
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
            w.parent = None
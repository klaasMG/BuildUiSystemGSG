class GSGWidget:
    FLAG_VISIBLE = 1 << 0
    FLAG_RENDER = 1 << 1
    FLAG_DIRTY = 1 << 2
    FLAG_NEEDS_LAYOUT = 1 << 3
    
    def __init__(self, flags=0, parent=None):
        self.id = 0
        self.children = {}
        self.flags = flags
        self.last_id = -1
        self.parent = parent
        self.widget_max = 10000
    
    def set_flag(self, flag):
        self.flags |= flag
    
    def clear_flag(self, flag):
        self.flags &= ~flag
    
    def has_flag(self, flag):
        return (self.flags & flag) != 0
    
    def add_child(self, child):
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
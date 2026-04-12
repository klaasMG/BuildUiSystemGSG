class GSGWidget:
    def __init__(self,parent=None):
        self.id = 0
        self.children = {}
        self.last_id = -1
        self.parent = parent
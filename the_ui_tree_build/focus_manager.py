import print_wrapper
from GSGwidget import GSGWidget


class FocusManager:
    def __init__(self):
        self.focused_widget = GSGWidget(parent=-1)
        self.focused_widget.id = -1

    def set_focused_widget(self, widget):
        self.focused_widget = widget

    def get_focused_widget(self):
        return self.focused_widget.id
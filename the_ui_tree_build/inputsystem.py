from PyQt5.QtCore import QObject, Qt
from PyQt5.QtWidgets import QApplication
from PyQt5.QtGui import QCursor

class input_system(QObject):
    def __init__(self, opengl_widget, GSG_parent):
        super().__init__()
        self.opengl_widget = opengl_widget
        self.GSG_parent = GSG_parent
    
    def update(self):
        """
        Capture the input state at this moment.
        Returns a snapshot with:
        - keys currently pressed
        - mouse position relative to the widget
        """
        # mouse position relative to widget
        mouse_pos = self.opengl_widget.mapFromGlobal(QCursor.pos())
        
        # keys currently pressed
        keys = set()
        for key in [Qt.Key_W , Qt.Key_A , Qt.Key_S , Qt.Key_D ,
                    Qt.Key_Up , Qt.Key_Down , Qt.Key_Left , Qt.Key_Right]:
            if QApplication.keyboardModifiers() & key:
                keys.add(key)
        # Note: For full key detection you need a more complete solution (Qt doesn't provide full snapshot natively)
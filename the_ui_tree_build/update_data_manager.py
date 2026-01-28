from typing import Any
from widget_data import WidgetDataType
from GSGwidget import GSGWidget
from threading import Lock
from print_wrapper import tprint

class DataHolder:
    def __init__(self,GSGuisystem):
        self.id_dif_data: dict[int, Any] = {}
        self.id_widget_data: dict[int, Any] = {}
        self.GSG_ui_system = GSGuisystem
    
    def update_widget_data(self,widget,data: dict[WidgetDataType,int | list[int] | GSGWidget | str],new_widget:bool):
        widget_id = widget.id
        if new_widget:
            data[WidgetDataType.PARENT] = widget.parent.id
        if widget_id not in self.id_dif_data:
            self.create_dif(widget_id,data)
        else:
            self.update_dif(widget_id,data)
        for widget_id, data in self.id_dif_data.items():
            id_data_list = self.dif_to_data(data)
            self.id_widget_data[widget_id] = id_data_list
        acquired = self.GSG_ui_system.hold_lock.lock()
        if acquired:
            self.GSG_ui_system.hold_lock.release()
        
    
    def create_dif(self, widget_id, data):
        self.id_dif_data[widget_id] = data
    
    def update_dif(self,widget_id,data):
        old_data = self.id_dif_data[widget_id]
        for i in data:
            if i in old_data:
                old_data[i] = data[i]
            else:
                old_data[i] = data[i]
                
    @staticmethod
    def dif_to_data(dif):
        data = [-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,]
        parent = -1
        for widget_data_type, value in dif.items():
            if widget_data_type == WidgetDataType.POSITION:
                data[0:6] = value
            elif widget_data_type == WidgetDataType.COLOUR:
                data[6:10] = value
            elif widget_data_type == WidgetDataType.SHADER_PASS:
                data[10] = value
            elif widget_data_type == WidgetDataType.SHAPE:
                data[11] = value
            elif widget_data_type == WidgetDataType.PARENT:
                parent = value
            elif widget_data_type == WidgetDataType.PATH_OR_DATA:
                data[12] = value
            elif widget_data_type == WidgetDataType.ASSET_OR_TEXT:
                data[13] = value
        return data, parent
from typing import Any
from widget_data import WidgetDataType
from GSGwidget import GSGWidget
from copy import deepcopy
from threading import Lock

class DataHolder:
    def __init__(self):
        self.id_dif_data: dict[int, Any] = {}
        self.data_exchange_lock = Lock()
    
    def take_data(self) -> dict:
        with self.data_exchange_lock:
            for widget_id, value in self.id_dif_data.items():
                self.id_dif_data[widget_id] = self.dif_to_data(value)
            data = deepcopy(self.id_dif_data)  # snapshot for caller
            self.id_dif_data.clear()  # erase internal state
            return data
    
    def update_widget_data(self,widget,data: dict[WidgetDataType,int | list[int] | GSGWidget | str],new_widget:bool):
        with self.data_exchange_lock:
            widget_id = widget.id
            if new_widget:
                data[WidgetDataType.PARENT] = widget.parent.id
            if widget_id not in self.id_dif_data:
                self.create_dif(widget_id,data)
            else:
                self.update_dif(widget_id,data)
    
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
from typing import Any
from widget_data import WidgetDataType

class DataHolder:
    def __init__(self):
        self.id_dif_data: dict[int, Any] = {}
    
    def take_data(self) -> dict:
        data = self.id_dif_data.copy()  # snapshot for caller
        self.id_dif_data.clear()  # erase internal state
        return data
    
    def update_widget_data(self,widget,data: dict[WidgetDataType,int]):
        widget_id = widget.id
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
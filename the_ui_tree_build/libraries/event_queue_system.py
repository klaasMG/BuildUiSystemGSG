# noinspection PyUnresolvedReferences
from event_struct import event_struct
import queue
from threading import Thread, Lock
import time

class EventQueue:
    def __init__(self, queue_id, event_system_parent):
        self.Event_System_parent = event_system_parent
        self.id = queue_id
        self.receive_priority_queue = queue.Queue()
        self.receive_normal_queue = queue.Queue()
        self.send_priority_queue = queue.Queue()
        self.send_normal_queue = queue.Queue()
        
    def send_event(self, event):
        priority = event[0]
        destination = event[1]
        event_type = event[2]
        data = event[3]
        source = self.id
        event_send = event_struct()
        event_send.priority = priority
        event_send.source = source
        event_send.destination = self.Event_System_parent.key_to_id.get(destination, 255)
        event_send.type = self.Event_System_parent.type_to_type_id.get(event_type, 255)
        event_send.data = data
        if priority == 0:
            self.send_priority_queue.put(event_send)
        elif priority == 1:
            self.send_normal_queue.put(event_send)
    
    def receive_event(self):
        try:
            event = self.receive_priority_queue.get_nowait()
        except queue.Empty:
            event = None
        if event is None:
            try:
                event = self.receive_normal_queue.get_nowait()
            except queue.Empty:
                event = None
        return event
    
    def receive_event_internal(self):
        """get event from a send queue for sanitation"""
        try:
            event = self.send_priority_queue.get_nowait()
        except queue.Empty:
            try:
                event = self.send_normal_queue.get_nowait()
            except queue.Empty:
                event = None
        return event
    
    def send_event_internal(self, event, priority):
        """send an event back depending on priority"""
        if priority == 0:
            self.receive_priority_queue.put(event)
        elif priority == 1:
            self.receive_normal_queue.put(event)

class EventSystem:
    def __init__(self):
        self.key_to_id = {}
        self.id_to_queues = {}
        self.type_to_type_id = {"wrong_data": 0}
        self.type_id_to_type_check = {0: []}
        self.queue_next_id = 0
        self.type_next_id = 1
        self.running = True
        self.lock = Lock()
    
    def add_queue(self, queue_key):
        with self.lock:
            self.key_to_id[queue_key] = self.queue_next_id
            self.id_to_queues[self.queue_next_id] = EventQueue(self.queue_next_id , self)
            self.queue_next_id += 1
            return self.get_queue(queue_key)
    
    def delete_queue(self , queue_key):
        raise RuntimeError("Queues are static; cannot delete")
    
    def handle_event_threaded(self):
        event_system_thread = Thread(target = self.handle_events)
        event_system_thread.start()
        
    def stop_event_system(self):
        self.running = False
        
    def handle_events(self):
        while self.running:
            with self.lock:
                for event_queue in self.id_to_queues.values():
                    event = event_queue.receive_event_internal()
                    if not isinstance(event , event_struct):
                        continue
                    
                    event_type = event.type
                    is_data_correct =self.data_check(event_type,event.data)
                    if not is_data_correct or event_type == 255:
                        event.type = 0
                        if event_type == 255:
                            event.data = [0]
                        else:
                            event.data = [1]
                        
                    source = event.source
                    destination = event.destination
                    if source not in self.id_to_queues or destination not in self.id_to_queues:
                        continue
                    
                    priority = event.priority
                    if priority != 0 and priority != 1:
                        priority = 1
                        
                    event = (event.type,event.data)
                    self.route_event(event,destination,priority)
            time.sleep(0.001)
                
    def route_event(self, event, destination, priority):
        event_queue = self.id_to_queues[destination]
        event_queue.send_event_internal(event, priority)
        
    def get_queue(self, queue_key):
        return self.id_to_queues[self.key_to_id[queue_key]]
    
    def data_check(self, event_type, data):
        """
               Validate event data against the parsed field specs.
               """
        spec = self.type_id_to_type_check.get(event_type)
        if spec is None:
            return False  # unknown type
        try:
            if len(spec) != len(data):
                return False  # wrong number of fields
        except TypeError:
            return False
        
        for rule , value in zip(spec , data):
            t = rule[0]
            
            if t == "int":
                min_val , max_val = rule[1] , rule[2]
                if not isinstance(value , int) or not (min_val <= value <= max_val):
                    return False
            
            elif t == "float":
                min_val , max_val = rule[1] , rule[2]
                if not isinstance(value , (int , float)) or not (min_val <= float(value) <= max_val):
                    return False
            
            elif t == "str":
                allowed = rule[1]
                if not isinstance(value , str) or value not in allowed:
                    return False
            
            elif t == "bool":
                allowed = rule[1]
                if not isinstance(value , bool) or value not in allowed:
                    return False
            
            elif t == "any":
                continue  # skip validation, user handles it
            
            else:
                return False  # unknown type
        #for the push
        return True
    
    def add_message_type(self, event_type, event_check):
        self.type_to_type_id[event_type] = self.type_next_id
        self.type_id_to_type_check[self.type_next_id] = self.parse_event_check(event_check)
        self.type_next_id += 1
        
    @staticmethod
    def parse_event_check(event_check):
        """
        full_spec: string with all fields separated by semicolon, e.g.
        "int:-1:23;float:0.0:10.5;str:archer,knight;bool:True,False;any"
        returns: list of parsed rules for each field
        """
        fields = event_check.split(";")
        parsed = []
        
        for field_spec in fields:
            field_spec = field_spec.strip()
            if field_spec.startswith("int:"):
                _ , range_str = field_spec.split(":" , 1)
                min_str , max_str = range_str.split(":")
                parsed.append(("int" , int(min_str) , int(max_str)))
            
            elif field_spec.startswith("float:"):
                _ , range_str = field_spec.split(":" , 1)
                min_str , max_str = range_str.split(":")
                parsed.append(("float" , float(min_str) , float(max_str)))
            
            elif field_spec.startswith("str:"):
                _ , vals_str = field_spec.split(":" , 1)
                allowed = vals_str.split(",")
                parsed.append(("str" , allowed))
            
            elif field_spec.startswith("bool:"):
                _ , vals_str = field_spec.split(":" , 1)
                allowed = []
                for v in vals_str.split(","):
                    if v == "True":
                        allowed.append(True)
                    elif v == "False":
                        allowed.append(False)
                parsed.append(("bool" , allowed))
            
            elif field_spec == "any":
                parsed.append(("any" ,))
            
            else:
                raise ValueError(f"Unknown field spec: {field_spec}")
        
        return parsed
        
    def remove_message_type(self, event_type):
        type_remove = self.type_to_type_id[event_type]
        self.type_to_type_id[event_type] = None
        self.type_id_to_type_check[type_remove] = None
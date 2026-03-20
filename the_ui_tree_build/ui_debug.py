import json
from pathlib import Path

config = Path("debug/debug.json")
try:
    with open(config, "r") as f:
        data = json.load(f)
    is_debug = data.get("debug_enabled", False)
except FileNotFoundError:
    is_debug = False
    
def debug_func(func,  *args, **kwargs):
    if is_debug:
        func(*args, **kwargs)
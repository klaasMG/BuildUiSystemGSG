import json
from pathlib import Path
config = Path("debug.json")
try:
    with open(config, "r") as f:
        data = json.load(f)
    is_debug = data.get("debug_enabled", False)
except FileNotFoundError:
    is_debug = False
    
def debug_func(func, debug, *args, **kwargs):
    if debug:
        func(*args, **kwargs)
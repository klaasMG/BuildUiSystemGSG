import json
import sys
from pathlib import Path
from enum import Enum

class DebugData(Enum):
    DebugEnabled = "debug_enabled"

flags = sys.argv[1:]
debug_config = flags[0]
set_debug_config = flags[1].lower() == "true" if len(flags) > 1 else None

DEBUG_FILE = Path(f"{debug_config}.json")  # je debug content file

def debug_run():
    if not DEBUG_FILE.exists():
        data = { DebugData.DebugEnabled.value: False }
        with open(DEBUG_FILE, "w") as f:
            json.dump(data, f, indent=2)

    with open(DEBUG_FILE, "r") as f:
        data = json.load(f)

    # update alleen als set_debug_config niet None is
    if set_debug_config is not None:
        data[DebugData.DebugEnabled.value] = set_debug_config

    with open(DEBUG_FILE, "w") as f:
        json.dump(data, f, indent=4)

if __name__ == "__main__":
    debug_run()
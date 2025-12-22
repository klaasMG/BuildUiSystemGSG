import json
from pathlib import Path
from enum import Enum

parent_directory = Path(__file__).resolve().parent

class DebugData(Enum):
    DebugEnabled = "debug_enabled"
    Create = "create"
    SetDebug = "set_debug"
    Exit = "exit"

def repl():
    running = True
    while running:
        try:
            cmd = input("work> ").strip()
        except EOFError:
            print("this way")
            break

        if cmd in ("exit", "quit"):
            break

        if not cmd:
            continue
            
        cmd_list = cmd.split()
        first_cmd = cmd_list[0]
        
        if first_cmd == DebugData.Create.value:
            debug_config = cmd_list[1]
            DEBUG_FILE = parent_directory / Path(f"{debug_config}.json")
            data = {DebugData.DebugEnabled.value: False}
            with open(DEBUG_FILE, "w") as f:
                json.dump(data, f, indent=4)
            
        elif first_cmd == DebugData.SetDebug.value:
            debug_config = cmd_list[1]
            DEBUG_FILE = parent_directory / Path(f"{debug_config}.json")
            with open(DEBUG_FILE, "r") as f:
                data = json.load(f)
            
            set_debug_config = cmd_list[2].lower() == "true"
            # update alleen als set_debug_config niet None is
            if set_debug_config is not None:
                data[DebugData.DebugEnabled.value] = set_debug_config
            
            with open(DEBUG_FILE, "w") as f:
                json.dump(data, f, indent=4)
        elif first_cmd == DebugData.Exit.value:
            running = False
        else:
            print(f"unknown command: {cmd}")

if __name__ == "__main__":
    repl()
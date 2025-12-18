import json
import sys
from pathlib import Path

DEBUG_FILE = Path("debug.json")  # your debug content file

def debug_run():
    if not DEBUG_FILE.exists():
        print("No debug file found.")
        return
    with open(DEBUG_FILE, "r") as f:
        data = json.load(f)

    if data.get("debug_enabled"):
        for var in data.get("variables_to_watch", []):
            print(f"Watching variable: {var}")
        for msg in data.get("messages", []):
            print(f"DEBUG: {msg}")

if __name__ == "__main__":
    debug_run()

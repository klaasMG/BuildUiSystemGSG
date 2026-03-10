import os
from pathlib import Path

folder = Path(__file__).parent.resolve()

extensions = {".py", ".cpp", ".h", ".hpp", ".glsl", ".bat"}

comment_markers = {
    ".py": {"single": "#", "multi": [("'''", "'''"), ('"""', '"""')]},
    ".cpp": {"single": "//", "multi": [("/*", "*/")]},
    ".h": {"single": "//", "multi": [("/*", "*/")]},
    ".hpp": {"single": "//", "multi": [("/*", "*/")]},
    ".glsl": {"single": "//", "multi": [("/*", "*/")]},
    ".bat": {"single": "REM", "multi": []}
}

ignore_dirs = {".venv", ".git", "cmake-build-debug", "cmake-build-debug-event-trace", "tooling", "JetBrainsPlugins", r"the_ui_tree_build\__pycache__", ".idea", r"the_code\pybind11"}

total_lines = empty_lines = comment_lines = full_comment_lines = 0
stats_per_lang = {ext: {"total":0,"empty":0,"full_comment":0,"any_comment":0,"loc":0} for ext in extensions}

script_file = Path(__file__).resolve()

ignore_dirs_lower = {x.lower() for x in ignore_dirs}  # normalize for case-insensitive comparison

for root, dirs, files in os.walk(folder):
    # prune ignored directories case-insensitively
    dirs[:] = [d for d in dirs if d.lower() not in ignore_dirs_lower]

    for f_name in files:
        f_path = Path(root) / f_name
        if f_path.resolve() == script_file:
            continue
        if f_path.suffix not in extensions:
            continue
        else:
            print(f"f_path: {f_path}")

        markers = comment_markers[f_path.suffix]
        in_multiline = False

        try:
            with f_path.open("r", encoding="utf-8", errors="ignore") as file:
                for line in file:
                    total_lines += 1
                    stats_per_lang[f_path.suffix]["total"] += 1
                    s = line.strip()
                    if not s:
                        empty_lines += 1
                        stats_per_lang[f_path.suffix]["empty"] += 1
                        continue

                    # multi-line comment
                    if in_multiline:
                        comment_lines += 1
                        full_comment_lines += 1
                        stats_per_lang[f_path.suffix]["any_comment"] += 1
                        stats_per_lang[f_path.suffix]["full_comment"] += 1
                        in_multiline = any(end not in s or s.find(start) > s.find(end) for start,end in markers.get("multi",[]))
                        continue

                    # start multi-line
                    started_multi = False
                    for start,end in markers.get("multi",[]):
                        if start in s:
                            comment_lines += 1
                            stats_per_lang[f_path.suffix]["any_comment"] += 1
                            if s.startswith(start):
                                full_comment_lines += 1
                                stats_per_lang[f_path.suffix]["full_comment"] += 1
                            if end not in s or s.find(start) > s.find(end):
                                in_multiline = True
                            started_multi = True
                            break
                    if started_multi:
                        continue

                    # single line
                    single = markers.get("single")
                    if single and single in s:
                        comment_lines += 1
                        stats_per_lang[f_path.suffix]["any_comment"] += 1
                        if s.startswith(single):
                            full_comment_lines += 1
                            stats_per_lang[f_path.suffix]["full_comment"] += 1
        except:
            continue

        stats_per_lang[f_path.suffix]["loc"] = stats_per_lang[f_path.suffix]["total"] - stats_per_lang[f_path.suffix]["empty"] - stats_per_lang[f_path.suffix]["full_comment"]

# print results
for i,(ext,data) in enumerate(stats_per_lang.items(),1):
    print(f"    {i}. {ext}")
    print(f"        LOC: {data['loc']}")
    print(f"        Empty: {data['empty']}")
    print(f"        Full Comments: {data['full_comment']}")
    print(f"        Any Comments: {data['any_comment']}")
    print(f"        Total: {data['total']}")

print(f"Total lines: {total_lines}")
print(f"Empty lines: {empty_lines}")
print(f"Comment lines (anywhere): {comment_lines}")
print(f"Full comment lines: {full_comment_lines}")
print(f"LOC: {total_lines - full_comment_lines - empty_lines}")
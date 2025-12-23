from pathlib import Path
import shutil

src_folder = Path("C:/Users/klaas/PycharmProjects/PythonProject23/cmake-build-debug")
dest_folder = Path("C:/Users/klaas/PycharmProjects/BuildUiSystemGSG/the_ui_tree_build")
dest_folder.mkdir(exist_ok=True)

for f in src_folder.iterdir():
    if f.is_file() and f.suffix == ".pyd":
        shutil.move(str(f), dest_folder / f.name)